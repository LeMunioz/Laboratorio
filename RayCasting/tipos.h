#pragma once
#include <vector>
#include <string>

/*
================================================================
    tipos.h - Definicion de todas las estructuras de datos

    Centraliza los tipos del motor para evitar dependencias
    circulares. Cualquier modulo que necesite estas estructuras
    solo debe incluir este archivo.
================================================================
*/

/*
    Comportamiento especial de una celda de pared.
    Determina como la renderiza el motor y si tiene colision fisica.

    NORMAL : Pared solida convencional.
    ARBOL  : Dos bandas (tronco + follaje) con huecos transparentes.
    ARCO   : Sin colision. El rayo pasa a traves y solo se dibuja
             el marco superior, dejando paso libre por debajo.
*/
enum class ComportamientoPared {
    NORMAL,
    ARBOL,
    ARCO
};

/*
    Descripcion visual y comportamiento de un tipo de pared.

    Campos de color (indices de la libreria frontend):
      colorCerca    - dist < DIST_CERCA
      colorLejos    - DIST_CERCA <= dist < DIST_LEJOS
      colorMuyLejos - dist >= DIST_LEJOS (normalmente gris 8)
      colorSombra   - cara horizontal (!ladoX), independiente de distancia

    Campos de caracter (textura):
      tramaCerca    - dist < DIST_CHAR
      tramaLejos    - dist >= DIST_CHAR

    Otros campos:
      altura        - multiplicador de altura en pantalla (1.0 = normal)
      sinColision   - true: el jugador y el rayo pueden pasar a traves
      comportamiento- define logica de renderizado especial
      nombre        - descripcion para depuracion
*/
struct TipoPared {
    int                 colorCerca;
    int                 colorLejos;
    int                 colorMuyLejos;
    int                 colorSombra;
    float               altura;
    char                tramaCerca;
    char                tramaLejos;
    bool                sinColision;
    ComportamientoPared comportamiento;
    const char*         nombre;
};

/*
    Una celda del mapa. tipo == 0 es espacio vacio.
    tipo > 0 es un indice en la tabla TIPOS_PARED.
*/
struct Celda {
    int tipo;
};

/*
    Datos completos de un mapa cargado desde el archivo.
    Se indexa como: celdas[fila][columna] == celdas[y][x]
*/
struct Mapa {
    std::string                     nombre;
    int                             ancho;
    int                             alto;
    std::vector<std::vector<Celda>> celdas;
    float                           spawnX;
    float                           spawnY;
    float                           spawnAngulo;
};

/*
    Estado del jugador: posicion en coordenadas flotantes
    y angulo de vision en radianes.
*/
struct Jugador {
    float x;
    float y;
    float angulo;
};

/*
    Resultado de lanzar un rayo con el algoritmo DDA.

    El rayo puede registrar DOS impactos en un mismo cast:
      1. Un arco (sinColision=true): el rayo continua despues de el.
      2. La primera pared solida detras del arco (o sin arco previo).

    Esto permite renderizar el fondo visible a traves de un arco
    y luego superponer el marco del arco como overlay.
*/
struct RayoResultado {
    // -- Pared solida --
    float distancia;    // Distancia perpendicular a la pared
    int   tipoPared;    // Indice en TIPOS_PARED
    bool  ladoX;        // true = cara vertical, false = cara horizontal
    bool  golpeo;       // true si el rayo impacto una pared solida

    // -- Arco (pared sin colision) encontrado antes de la pared solida --
    float distanciaArco;
    int   tipoArco;
    bool  ladoXArco;
    bool  hayArco;      // true si se encontro al menos un arco en la trayectoria
};

/*
    Una celda del buffer de pantalla del renderer.
*/
struct CeldaPantalla {
    char ch;
    int  col;
};
