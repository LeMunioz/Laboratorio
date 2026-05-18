#pragma once

/*
================================================================
    config.h - Configuracion global del motor de raycasting

    Modifica estas constantes para ajustar el comportamiento
    del juego sin necesidad de tocar el codigo de los modulos.
    Todas las unidades de distancia son en celdas del mapa.
================================================================
*/

// -- Constante matematica --
constexpr float PI = 3.14159265f;

// -- Dimensiones del area de renderizado en la consola --
// Ajusta estos valores segun el tamano de tu ventana de terminal
constexpr int ANCHO_PANTALLA = 120;
constexpr int ALTO_PANTALLA  = 36;

// -- Campo de vision del jugador (en radianes) --
// PI/3 = 60 grados, PI/2 = 90 grados
constexpr float FOV = PI / 3.0f;

// -- Velocidades de movimiento y rotacion --
constexpr float VEL_MOVIMIENTO = 0.08f;
constexpr float VEL_ROTACION   = 0.05f;

// -- Distancia maxima a la que el motor detecta paredes --
constexpr float DIST_MAX = 20.0f;

// -- Umbrales de distancia para el sombreado de paredes --
// Las paredes cambian de caracter segun que tan lejos esten
constexpr float SOMBRA_MUY_CERCA = 2.5f;
constexpr float SOMBRA_CERCA     = 5.0f;
constexpr float SOMBRA_MEDIA     = 8.0f;
constexpr float SOMBRA_LEJOS     = 12.0f;

// -- Colores del cielo y el piso (indices de la libreria frontend) --
constexpr int COLOR_CIELO = 1;   // Azul Rey (oscuro)
constexpr int COLOR_PISO  = 8;   // Gris

// -- Caracteres del cielo y el piso --
constexpr char CHAR_CIELO = ' ';
constexpr char CHAR_PISO  = '.';

// -- Dimensiones del minimapa superpuesto en pantalla --
constexpr int ANCHO_MINI = 22;
constexpr int ALTO_MINI  = 10;
