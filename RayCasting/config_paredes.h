#pragma once
#include "tipos.h"
#include <array>

/*
================================================================
    config_paredes.h - Declaracion de la tabla de tipos de pared

    La definicion de los valores esta en config_paredes.cpp.
    Para agregar o modificar un tipo de pared:
      1. Edita el array en config_paredes.cpp
      2. Usa el indice correspondiente (1-9) en el archivo mapas.txt

    REFERENCIA DE COLORES de la libreria frontend:
      1  = Azul Rey      2  = Verde         3  = Azul Cyan
      4  = Rojo          5  = Violeta       6  = Amarillo Ocre
      7  = Gris Claro    8  = Gris          9  = Azul
      10 = Verde Pasto   11 = Celeste       12 = Rojo Claro
      13 = Morado        14 = Crema         15 = Blanco
================================================================
*/

constexpr int NUM_TIPOS_PARED = 10;

/*
    Tabla global indexada por el digito del mapa.
    Indice 0 = espacio vacio (no se utiliza directamente).
    Indices 1-9 corresponden a los digitos '1'-'9' del archivo.
*/
extern const std::array<TipoPared, NUM_TIPOS_PARED> TIPOS_PARED;
