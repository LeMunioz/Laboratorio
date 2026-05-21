const express = require('express');
const cors = require('cors');
const app = express();
const PORT = 3000;

// Middleware para permitir peticiones de otros orígenes (CORS) y parsear JSON
app.use(cors());
app.use(express.json());

// Servir la página web de manera estática desde una carpeta llamada 'public'
app.use(express.static('public'));

// Estructura en memoria para almacenar el estado actual del río
let estadoRio = {
    nodo_id: "Ninguno",
    nivel_agua: 0.0,
    alerta_critica: false,
    ultima_actualizacion: "Sin datos"
};

// Histórico local para las gráficas de tendencias
let historialLecturas = [];

// ==========================================
// ENDPOINT: Recibir datos del ESP32 (POST)
// ==========================================
app.post('/api/data', (req, res) => {
    const { nodo_id, nivel_agua } = req.body;

    // Validación básica de los datos entrantes
    if (!nodo_id || nivel_agua === undefined) {
        return res.status(400).json({ error: "Datos incompletos o inválidos." });
    }

    // Actualizar el estado actual
    estadoRio.nodo_id = nodo_id;
    estadoRio.nivel_agua = parseFloat(nivel_agua).toFixed(2);
    estadoRio.ultima_actualizacion = new Date().toLocaleTimeString();

    // Motor de Cálculo de Alertas: Evaluar si supera el umbral del 85%
    if (nivel_agua > 85.0) {
        estadoRio.alerta_critica = true;
    } else {
        estadoRio.alerta_critica = false;
    }

    // Guardar en el histórico (limitar a las últimas 20 lecturas para optimizar memoria)
    historialLecturas.push({
        hora: estadoRio.ultima_actualizacion,
        nivel: parseFloat(nivel_agua)
    });
    if (historialLecturas.length > 20) {
        historialLecturas.shift();
    }

    console.log(`[Sensor] Datos recibidos de ${nodo_id}: ${nivel_agua}% | Alerta: ${estadoRio.alerta_critica}`);
    
    res.status(200).json({ status: "Procesado correctamente", alerta: estadoRio.alerta_critica });
});

// ==========================================
// ENDPOINT: Enviar datos a la Página Web (GET)
// ==========================================
app.get('/api/status', (req, res) => {
    res.status(200).json({
        actual: estadoRio,
        historico: historialLecturas
    });
});

// Inicializar el servidor en el puerto configurado
app.listen(PORT, '0.0.0.0',() => {
    console.log(`Servidor SafeRiver corriendo en http://localhost:${PORT}`);
});