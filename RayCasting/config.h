#pragma once

/*
================================================================
    config.h - Configuracion global del motor de raycasting
================================================================
*/

constexpr float PI = 3.14159265f;

// -- Dimensiones del area de renderizado --
constexpr int ANCHO_PANTALLA = 200;
constexpr int ALTO_PANTALLA  = 60;

// -- Campo de vision (radianes) --
constexpr float FOV = PI / 3.0f;

// -- Velocidades --
constexpr float VEL_MOVIMIENTO = 0.25f;
constexpr float VEL_ROTACION   = 0.1f;

// -- Distancia maxima de vision --
constexpr float DIST_MAX = 20.0f;

/*
    Umbrales de distancia para el sistema de color de 4 estados:
      dist < DIST_CERCA       -> colorCerca
      dist < DIST_LEJOS       -> colorLejos
      dist >= DIST_LEJOS      -> colorMuyLejos
      !ladoX (cara horizontal) -> colorSombra (ignora distancia)

    Umbral de caracter:
      dist < DIST_CHAR        -> tramaCerca
      dist >= DIST_CHAR       -> tramaLejos
*/
constexpr float DIST_CERCA = 4.0f;
constexpr float DIST_LEJOS = 9.0f;
constexpr float DIST_CHAR  = 6.5f;

// -- Colores del cielo y piso --
constexpr int  COLOR_CIELO = 1;
constexpr int  COLOR_PISO  = 8;
constexpr char CHAR_CIELO  = '+';
constexpr char CHAR_PISO   = '.';

// -- Minimapa --
constexpr int ANCHO_MINI = 28;
constexpr int ALTO_MINI  = 12;

/*
    ARCO

    El marco del arco NUNCA baja del ojo del jugador (mitad de pantalla).
    ARC_MARGEN_OJO define cuantos pixeles por encima del horizonte
    se garantiza que quede el borde inferior del marco.
    Un valor de 2 deja siempre al menos 2 filas de apertura visible.
*/
constexpr int ARC_MARGEN_OJO = 2;

/*
    ARBOL
    ─────
    ARBOL_ANCHO_TRONCO : fraccion del ancho de celda que ocupa el tronco
                         (0.0=nada, 1.0=ancho completo)
                         Los rayos que impactan fuera de este rango
                         solo dibujan follaje; dentro dibujan tambien tronco.
    ARBOL_FRAC_TRONCO  : fraccion de altBase que usa el tronco en altura.
    ARBOL_FRAC_OVERLAP : superposicion entre la base del follaje y la cima
                         del tronco (evita huecos entre ambas bandas).
*/
constexpr float ARBOL_ANCHO_TRONCO = 0.5f;
constexpr float ARBOL_FRAC_TRONCO  = 0.30f;
constexpr float ARBOL_FRAC_OVERLAP = 0.08f;

/*
    Aliases de compatibilidad: nombres usados en versiones anteriores
    del renderer. Apuntan a las mismas constantes con nombre nuevo.
*/
constexpr float FRAC_TRONCO          = ARBOL_FRAC_TRONCO;
constexpr float FRAC_FOLLAJE_OVERLAP = ARBOL_FRAC_OVERLAP;
constexpr float FRACCION_ARCO        = 0.1f;
