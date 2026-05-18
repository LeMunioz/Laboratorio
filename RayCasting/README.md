# Motor Raycasting 3D en Consola

Motor de renderizado 3D por raycasting para consola de Windows.
Escrito en C++17, sin dependencias externas mas alla de la
libreria `frontend.cpp` incluida en el proyecto.

---

## Estructura de archivos

```
raycaster/
├── main.cpp              <- Punto de entrada y bucle principal
├── config.h              <- Constantes globales (FOV, velocidad, colores)
├── tipos.h               <- Definicion de todas las estructuras de datos
├── config_paredes.h      <- Declaracion de la tabla de tipos de pared
├── config_paredes.cpp    <- Valores visuales de cada tipo de pared
├── mapa.h / mapa.cpp     <- Carga y consulta de mapas desde archivo
├── jugador.h / jugador.cpp <- Movimiento, strafe, colisiones, input
├── renderer.h / renderer.cpp <- Algoritmo DDA, buffer, HUD, minimapa
├── frontend.cpp          <- Libreria externa (gotoxy, color)
└── mapas.txt             <- Archivo de definicion de mapas
```

---

## Compilacion

Con MinGW (g++) en Windows:

```bash
g++ -std=c++17 -O2 -o raycaster main.cpp
```

Con MSVC desde el Developer Command Prompt:

```bash
cl /std:c++17 /O2 /EHsc main.cpp /Fe:raycaster.exe
```

> `main.cpp` incluye directamente todos los `.cpp` del motor,
> por lo que solo se necesita compilar un archivo.

---

## Controles

| Tecla              | Accion                        |
|--------------------|-------------------------------|
| `W`                | Avanzar                       |
| `S`                | Retroceder                    |
| `A`                | Desplazamiento lateral izq.   |
| `D`                | Desplazamiento lateral der.   |
| `Flecha izquierda` | Girar camara a la izquierda   |
| `Flecha derecha`   | Girar camara a la derecha     |
| `ESC`              | Salir al siguiente mapa       |

---

## Como agregar un nuevo mapa

Edita `mapas.txt` y agrega un bloque con este formato:

```
[MAPA]
nombre=Mi Nuevo Cuarto
spawn=2.5 2.5 0.0
ANCHO ALTO
111111
100001
100001
111111
[FIN]
```

- **spawn**: coordenada X, coordenada Y, angulo en radianes
- **ANCHO ALTO**: dimensiones del grid que viene a continuacion
- **Grid**: digitos del 0 al 9 (0 = vacio, 1-9 = tipo de pared)

Los mapas se recorren en orden. El jugador puede saltar un mapa
presionando `ESC` en la pantalla de bienvenida.

---

## Como agregar un nuevo tipo de pared

1. Abre `config_paredes.cpp`
2. Incrementa `NUM_TIPOS_PARED` en `config_paredes.h` si excedes 10
3. Agrega una nueva entrada al array `TIPOS_PARED`:

```cpp
{ colorCerca, colorLejos, altura, 'caracter', "Nombre" }
```

4. Usa el indice correspondiente (siguiente disponible) en tus mapas

---

## Como ajustar parametros del motor

Todos los parametros globales estan en `config.h`:

| Constante          | Descripcion                                    |
|--------------------|------------------------------------------------|
| `ANCHO_PANTALLA`   | Columnas del area de renderizado               |
| `ALTO_PANTALLA`    | Filas del area de renderizado                  |
| `FOV`              | Campo de vision en radianes (PI/3 = 60 grados) |
| `VEL_MOVIMIENTO`   | Velocidad de desplazamiento del jugador        |
| `VEL_ROTACION`     | Velocidad de giro de la camara                 |
| `DIST_MAX`         | Distancia maxima de vision (en celdas)         |
| `SOMBRA_*`         | Umbrales de distancia para el sombreado        |
| `COLOR_CIELO/PISO` | Color del cielo y el piso                      |

---

## Algoritmo DDA resumido

```
Para cada columna X de la pantalla:
  1. Calcular el angulo del rayo = angulo_jugador + offset_columna
  2. Lanzar el rayo con DDA:
       Avanzar por el eje (X o Y) cuya proxima interseccion
       con el grid este mas cerca, hasta golpear una pared.
  3. Calcular distancia perpendicular (evita efecto ojo de pez)
  4. Calcular altura de la pared = ALTO_PANTALLA / distancia * multiplicador
  5. Dibujar:  [cielo] [pared con color y trama] [piso]
```

---

## Posibles expansiones

- **Sprites 2.5D**: ordenar objetos por distancia y proyectarlos como columnas
- **Texturas reales**: mapear una imagen a las columnas de la pared
- **Puertas animadas**: celdas especiales con estado abierto/cerrado
- **Sistema de niveles**: cargar el siguiente mapa al pisar una celda especial
- **Audio**: integrar Beep() o una libreria de sonido para pasos y ambiente
- **Selector de mapa**: menu interactivo al inicio para elegir el cuarto
