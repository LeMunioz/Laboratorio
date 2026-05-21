#include <WiFi.h>
#include <HTTPClient.h>

// Pines para el Sensor Ultrasónico
const int TRIG_PIN = 5;       // GPIO 5
const int ECHO_PIN = 18;      // GPIO 18

// Constantes de Calibración Física (en centímetros)
const float DISTANCIA_VACIO = 400.0; // Distancia desde el sensor hasta el fondo del río (río seco)
const float DISTANCIA_LLENO = 50.0;  // Distancia máxima segura antes de desbordar (río al 100%)


//DATOS DE LA RED   ---------> AQUI PONEMOS LO DE LABSO
const char* ssid = "TheMoon";
const char* password = "Corredinsensatos";
const char* serverUrl = "http://192.168.1.75:3000/api/data"; // Recuerda cambiarlo por tu IP local

volatile float ultimoNivelCalculado = 0.0;

void vTaskSensorRead(void *pvParameters);
void vTaskWiFiTransmit(void *pvParameters);

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  xTaskCreatePinnedToCore(vTaskSensorRead, "LecturaSensor", 2048, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(vTaskWiFiTransmit, "TransmisionWiFi", 4096, NULL, 2, NULL, 0);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

// NUEVA TAREA: Medición por ultrasonido y conversión a porcentaje
void vTaskSensorRead(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(300000); // 5 minutos

  for(;;) {
    // 1. Generar el pulso de disparo (Trigger) de 10 microsegundos
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // 2. Medir el tiempo que tarda en regresar el eco (en microsegundos)
    // Se usa un timeout de 30000 µs (aprox. 5 metros máximo de rango)
    long duracion = pulseIn(ECHO_PIN, HIGH, 30000); 
    
    // 3. Calcular la distancia en centímetros (Velocidad del sonido = 343 m/s)
    float distanciaMedida = (duracion * 0.0343) / 2;

    float nivelPorcentaje = 0.0;

    // Si la lectura es válida (mayor a 0 y dentro del rango del puente)
    if (distanciaMedida > 0 && distanciaMedida <= DISTANCIA_VACIO) {
      // OJO: A menor distancia medida por el sensor, significa que el agua está más ALTA.
      // Fórmula matemática inversa para el ultrasonido:
      nivelPorcentaje = ((DISTANCIA_VACIO - distanciaMedida) / (DISTANCIA_VACIO - DISTANCIA_LLENO)) * 100.0;
      
      // Acotar límites
      if (nivelPorcentaje < 0.0) nivelPorcentaje = 0.0;
      if (nivelPorcentaje > 100.0) nivelPorcentaje = 100.0;
    } else {
      Serial.println("Error: Lectura de ultrasonido fuera de rango.");
      // Mantiene el último nivel calculado o puedes definir un código de error
      nivelPorcentaje = ultimoNivelCalculado; 
    }
    
    ultimoNivelCalculado = nivelPorcentaje;
    
    Serial.print("Distancia medida: "); Serial.print(distanciaMedida); Serial.print(" cm -> ");
    Serial.print("Nivel del río: "); Serial.print(ultimoNivelCalculado); Serial.println("%");

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}