#pragma once
#include <vector>
#include <string>
#include <mutex>

/*
================================================================
    tipos.h - Definicion de todas las estructuras de datos

    Este archivo centraliza los tipos del motor para evitar
    dependencias circulares entre modulos. Todos los .h y .cpp
    que necesiten estas estructuras deben incluir este archivo.
    
    CAMBIOS: Ahora soporta parallelismo con mutex y buffers
    compartidos, paredes especiales, y sistemas de cielo/suelo.
================================================================
*/

/*
    Descripcion visual de un tipo de pared.
    La altura modifica cuanto espacio vertical ocupa la pared en
    pantalla: 1.0 es normal, 2.0 es el doble de alto, 0.5 es barda.
    La trama ahora puede ser una cadena personalizada.
*/
struct TipoPared {
    int         colorCerca;  // Color cuando el jugador esta cerca
    int         colorLejos;  // Color cuando el jugador esta lejos
    float       altura;      // Multiplicador de altura (1.0 = normal)
    std::string trama;       // Patron de textura personalizado
    const char* nombre;      // Nombre descriptivo para depuracion
};

/*
    Pared especial que consiste en dos partes apiladas (ej: arbol).
    Se renderiza como dos muros superpuestos con diferentes alturas y colores.
*/
struct ParedEspecial {
    bool        activa;      // Si esta habilitada
    std::string nombre;      // Nombre (ej: "Arbol", "Poste")
    
    // Parte inferior
    float       alturaInf;   // Altura multiplicadora
    int         colorCercaInf;
    int         colorLejosInf;
    std::string tramaInf;
    
    // Parte superior
    float       alturaSup;   // Altura multiplicadora
    int         colorCercaSup;
    int         colorLejosSup;
    std::string tramaSup;
};

/*
    Configuracion de cielo para un mapa.
    Define como se renderiza la mitad superior de la pantalla.
*/
struct TipoCielo {
    int         color;       // Color base del cielo
    std::string trama;       // Patron opcional (ej: estrellas)
    const char* nombre;      // Nombre descriptivo
};

/*
    Configuracion de suelo para un mapa.
    Define como se renderiza la mitad inferior de la pantalla.
*/
struct TipoSuelo {
    int         color;       // Color base del suelo
    std::string trama;       // Patron opcional (ej: pasto)
    const char* nombre;      // Nombre descriptivo
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
    int                              idCielo;  // Indice del tipo de cielo
    int                              idSuelo;  // Indice del tipo de suelo
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

/*
    Buffer compartido para el resultado de rayos de una columna.
    Usado para comunicacion entre el hilo de raycasting y el de dibujado.
*/
struct ColumnaDatos {
    int                  x;             // Columna X
    RayoResultado        rayo;          // Resultado del raycast
    float                altMuro;       // Altura calculada del muro
    std::vector<CeldaPantalla> buffer;  // Pixeles a dibujar (ALTO_PANTALLA elementos)
    bool                 completa;      // Flag: columna lista para dibujar
};

/*
    Buffer compartido para datos del minimapa.
    Usado para comunicacion entre el hilo principal y el de minimapa.
*/
struct MinimapaDatos {
    std::vector<std::vector<CeldaPantalla>> buffer;  // Buffer 2D del minimapa
    bool                 completo;      // Flag: minimapa listo
    std::mutex           mutex;         // Proteccion de acceso concurrente
};

/*
    Buffer compartido para toda la escena renderizada.
    Usado para comunicacion entre hilo de dibujado y volcado final.
*/
struct BufferEscena {
    std::vector<std::vector<CeldaPantalla>> pantalla;  // Buffer completo
    std::mutex           mutex;         // Proteccion de acceso concurrente
};
