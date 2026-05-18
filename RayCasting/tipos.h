#pragma once
#include <vector>
#include <string>

/*
================================================================
    tipos.h - Definicion de todas las estructuras de datos

    Este archivo centraliza los tipos del motor para evitar
    dependencias circulares entre modulos. Todos los .h y .cpp
    que necesiten estas estructuras deben incluir este archivo.
================================================================
*/

/*
    Descripcion visual de un tipo de pared.
    La altura modifica cuanto espacio vertical ocupa la pared en
    pantalla: 1.0 es normal, 2.0 es el doble de alto, 0.5 es barda.
*/
struct TipoPared {
    int         colorCerca;  // Color cuando el jugador esta cerca
    int         colorLejos;  // Color cuando el jugador esta lejos
    float       altura;      // Multiplicador de altura (1.0 = normal)
    char        trama;       // Caracter base de la textura
    const char* nombre;      // Nombre descriptivo para depuracion
};

/*
    Una celda del mapa. Solo almacena el tipo de pared.
    tipo == 0 significa celda vacia (transitable por el jugador).
    tipo  > 0 es un indice en la tabla TIPOS_PARED.
*/
struct Celda {
    int tipo;
};

/*
    Datos completos de un mapa cargado desde el archivo.
    El grid se indexa como:  celdas[fila][columna]
    equivalente a:           celdas[y][x]
*/
struct Mapa {
    std::string                      nombre;
    int                              ancho;
    int                              alto;
    std::vector<std::vector<Celda>>  celdas;
    float                            spawnX;
    float                            spawnY;
    float                            spawnAngulo;
};

/*
    Estado del jugador: posicion en el mundo (coordenadas flotantes)
    y angulo de vision en radianes.
*/
struct Jugador {
    float x;
    float y;
    float angulo;
};

/*
    Resultado de lanzar un rayo contra el mapa.
    Contiene toda la informacion necesaria para dibujar
    una columna de la pantalla con el algoritmo DDA.
*/
struct RayoResultado {
    float distancia;   // Distancia perpendicular a la pared impactada
    int   tipoPared;   // Tipo de pared golpeada (indice 1-9)
    bool  ladoX;       // true = golpeo cara vertical, false = horizontal
    bool  golpeo;      // true si el rayo impacto alguna pared
};

/*
    Una celda del buffer de pantalla del motor.
    El renderer llena este buffer antes de volcarlo a la consola.
*/
struct CeldaPantalla {
    char ch;   // Caracter a imprimir
    int  col;  // Color (indice de la libreria frontend)
};
