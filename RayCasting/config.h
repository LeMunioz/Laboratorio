#pragma once

/*
================================================================
    config.h - Configuracion global del motor de raycasting

    Modifica estas constantes para ajustar el comportamiento
    del juego sin tocar el codigo de los modulos internos.
================================================================
*/

// -- Constante matematica --
constexpr float PI = 3.14159265f;

// -- Dimensiones del area de renderizado en la consola --
constexpr int ANCHO_PANTALLA = 160;
constexpr int ALTO_PANTALLA  = 45;

// -- Campo de vision del jugador (en radianes) --
// PI/3 = 60 grados, PI/2 = 90 grados
constexpr float FOV = PI / 3.0f;

// -- Velocidades de movimiento y rotacion --
constexpr float VEL_MOVIMIENTO = 0.2f;
constexpr float VEL_ROTACION   = 0.1f;

// -- Distancia maxima a la que el motor detecta paredes --
constexpr float DIST_MAX = 20.0f;

/*
    Umbrales de distancia para seleccionar color y caracter.

    < DIST_CERCA  -> colorCerca   + tramaCerca
    < DIST_LEJOS  -> colorLejos   + tramaCerca
    >= DIST_LEJOS -> colorMuyLejos + tramaLejos

    El colorSombra reemplaza a todos cuando !ladoX (cara horizontal).
    El cambio de caracter ocurre en DIST_CHAR independientemente del color.
*/
constexpr float DIST_CERCA  = 4.0f;
constexpr float DIST_LEJOS  = 9.0f;
constexpr float DIST_CHAR   = 6.5f;   // Umbral para cambiar de tramaCerca a tramaLejos

// -- Colores del cielo y el piso (indices de la libreria frontend) --
constexpr int COLOR_CIELO = 1;   // Azul oscuro
constexpr int COLOR_PISO  = 8;   // Gris

// -- Caracteres del cielo y el piso --
constexpr char CHAR_CIELO = ' ';
constexpr char CHAR_PISO  = '.';

// -- Dimensiones del minimapa superpuesto en pantalla --
constexpr int ANCHO_MINI = 28;
constexpr int ALTO_MINI  = 12;

// -- Fraccion de la altura del arco que forma el marco superior --
// 0.30 = solo se dibuja el 30% superior de la pared, dejando paso debajo
constexpr float FRACCION_ARCO = 0.15f;

// -- Fracciones del arbol para tronco y follaje --
constexpr float FRAC_TRONCO  = 0.2f;   // Altura del tronco relativa a la pared base
constexpr float FRAC_FOLLAJE_OVERLAP = 0.05f;  // Cuanto se superpone el follaje al tronco
