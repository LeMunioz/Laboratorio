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

    REFERENCIA DE COLORES de la libreria frontend ANSI 256:
    Rango 0-15: Colores basicos
    Rango 16-231: Colores extendidos (6x6x6 RGB)
    Rango 232-255: Escala de grises
================================================================
*/

constexpr int NUM_TIPOS_PARED = 8;
constexpr int NUM_PAREDES_ESPECIALES = 2;
constexpr int NUM_TIPOS_CIELO = 3;
constexpr int NUM_TIPOS_SUELO = 2;

/*
    Tabla global de tipos de pared normales (indices 0-7).
    Indice 0 = espacio vacio (no se utiliza directamente).
    Indices 1-7 corresponden a los digitos '1'-'7' del archivo.
*/
extern const std::array<TipoPared, NUM_TIPOS_PARED> TIPOS_PARED;

/*
    Tabla de paredes especiales (arboles, postes, etc).
    Se definen fuera de la tabla normal para no ocupar espacio en todos los mapas.
*/
extern const std::array<ParedEspecial, NUM_PAREDES_ESPECIALES> PAREDES_ESPECIALES;

/*
    Tabla de tipos de cielo disponibles.
*/
extern const std::array<TipoCielo, NUM_TIPOS_CIELO> TIPOS_CIELO;

/*
    Tabla de tipos de suelo disponibles.
*/
extern const std::array<TipoSuelo, NUM_TIPOS_SUELO> TIPOS_SUELO;
