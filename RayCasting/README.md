# Motor Raycasting 3D en Consola

Motor 3D por raycasting para consola de Windows. Escrito en C++17,
sin dependencias externas mas alla de `frontend.cpp`.

---

## Compilacion (un solo comando)

```bash
g++ -std=c++17 -O2 -o raycaster main.cpp
```

> `main.cpp` incluye directamente todos los `.cpp` del motor.

---

## Controles

| Tecla              | Accion                       |
|--------------------|------------------------------|
| `W / S`            | Avanzar / Retroceder         |
| `A / D`            | Strafe lateral               |
| `Flecha izq/der`   | Girar camara                 |
| `ESC`              | Salir al siguiente mapa      |

---

## Estructura de archivos

```
main.cpp              <- Bucle principal, configuracion de consola
config.h              <- Constantes globales (pantalla, FOV, distancias)
tipos.h               <- Todas las estructuras compartidas
config_paredes.h/cpp  <- Tabla de tipos de pared (apariencia y comportamiento)
mapa.h/cpp            <- Parser de mapas.txt, consultas al grid
jugador.h/cpp         <- Input, movimiento, colisiones
renderer.h/cpp        <- DDA raycasting, buffer, arbol, arco, minimapa, HUD
frontend.cpp          <- Libreria de consola (gotoxy, color)
mapas.txt             <- Definicion de todos los mapas
```

---

## Sistema de color de 4 estados

Cada tipo de pared tiene cuatro colores segun distancia y lado:

| Condicion                   | Campo usado    |
|-----------------------------|----------------|
| dist < DIST_CERCA (4.0)     | colorCerca     |
| dist < DIST_LEJOS (9.0)     | colorLejos     |
| dist >= DIST_LEJOS          | colorMuyLejos  |
| Cara horizontal (!ladoX)    | colorSombra    |

Y dos caracteres segun distancia:

| Condicion                   | Campo usado    |
|-----------------------------|----------------|
| dist < DIST_CHAR (6.5)      | tramaCerca     |
| dist >= DIST_CHAR           | tramaLejos     |

---

## Tipos de pared disponibles

| Indice | Nombre             | Comportamiento | Nota                         |
|--------|--------------------|----------------|------------------------------|
| 1      | ParedPiedra        | NORMAL         | Altura x2                    |
| 2      | ParedLadrillo      | NORMAL         | Altura x2                    |
| 3      | ParedMadera        | NORMAL         | Altura x2                    |
| 4      | ParedAzul          | NORMAL         | Altura x2                    |
| 5      | ParedAcero         | NORMAL         | Altura x2                    |
| 6      | ParedAzulAlta      | NORMAL         | Altura x3.5, muy alta        |
| 7      | BardaLadrilloBaja  | NORMAL         | Altura x0.5, media pared     |
| 8      | Arbol              | ARBOL          | Tronco + follaje, con huecos |
| 9      | Arco               | ARCO           | Sin colision, marco superior |

---

## Como agregar un nuevo mapa

En `mapas.txt`:

```
[MAPA]
nombre=Mi Cuarto
spawn=2.5 2.5 0.0
8 6
11111111
10000001
10000001
10000001
10000001
11111111
[FIN]
```

---

## Como agregar un nuevo tipo de pared

1. Abre `config_paredes.cpp` y agrega una entrada al array:

```cpp
{ colorCerca, colorLejos, colorMuyLejos, colorSombra,
  altura, tramaCerca, tramaLejos,
  sinColision, ComportamientoPared::NORMAL, "NombrePared" }
```

2. Si superas 10 entradas, incrementa `NUM_TIPOS_PARED` en `config_paredes.h`.
3. Usa el indice (posicion en el array) en tus mapas.

---

## Como crear comportamientos nuevos

1. Agrega un valor al enum `ComportamientoPared` en `tipos.h`.
2. En `renderer.cpp`, agrega un `case` en el `switch` dentro de `renderizarFrame`.
3. Escribe la funcion `renderizarColumna<NuevoTipo>(...)` siguiendo el patron
   de `renderizarColumnaArbol` o `renderizarColumnaArco`.

---

## Ajustes rapidos en config.h

| Constante        | Efecto                                    |
|------------------|-------------------------------------------|
| ANCHO_PANTALLA   | Columnas del area de juego (hoy: 160)     |
| ALTO_PANTALLA    | Filas del area de juego (hoy: 45)         |
| FOV              | Campo de vision (PI/3 = 60 grados)        |
| VEL_MOVIMIENTO   | Velocidad de desplazamiento               |
| VEL_ROTACION     | Velocidad de giro                         |
| DIST_CERCA/LEJOS | Umbrales de cambio de color               |
| DIST_CHAR        | Umbral de cambio de caracter              |
| FRACCION_ARCO    | Tamano del marco del arco (0.30 = 30%)    |
| FRAC_TRONCO      | Altura del tronco relativa a pared base   |
