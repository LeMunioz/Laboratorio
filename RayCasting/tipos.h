#pragma once
#include <vector>
#include <string>

/*
================================================================
    tipos.h - Definicion de todas las estructuras de datos
================================================================
*/

enum class ComportamientoPared {
    NORMAL,
    ARBOL,
    ARCO
};

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

struct Celda {
    int tipo;
};

struct Mapa {
    std::string                     nombre;
    int                             ancho;
    int                             alto;
    std::vector<std::vector<Celda>> celdas;
    float                           spawnX;
    float                           spawnY;
    float                           spawnAngulo;
};

struct Jugador {
    float x;
    float y;
    float angulo;
};

/*
    Resultado del DDA para una columna de pantalla.

    Pared solida:
      golpeo    - true si se encontro pared solida
      distancia - distancia perpendicular a esa pared
      tipoPared - indice en TIPOS_PARED
      ladoX     - true = cara vertical (Y constante en la cara)
      wallX     - posicion de impacto dentro de la celda [0.0, 1.0)
                  Se usa para calcular si el rayo golpeo el centro
                  de un arbol (zona del tronco) o los bordes (follaje).

    Arco encontrado ANTES de la pared solida:
      hayArco       - true si hay al menos un arco en la trayectoria
      distanciaArco - distancia perpendicular al arco
      tipoArco      - indice en TIPOS_PARED
      ladoXArco     - cara del arco
      wallXArco     - posicion dentro de la celda del arco
*/
struct RayoResultado {
    // Pared solida
    float distancia;
    int   tipoPared;
    bool  ladoX;
    bool  golpeo;
    float wallX;

    // Arco (sinColision)
    float distanciaArco;
    int   tipoArco;
    bool  ladoXArco;
    bool  hayArco;
    float wallXArco;
};

struct CeldaPantalla {
    char ch;
    int  col;
};
