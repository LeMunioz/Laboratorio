#pragma once
#include "tipos.h"
#include <array>

/*
================================================================
    config_paredes.h - Declaracion de la tabla de tipos de pared

    Para agregar o modificar un tipo de pared:
      1. Edita el array en config_paredes.cpp
      2. Si excedes NUM_TIPOS_PARED, incrementa el valor aqui
      3. Usa el indice correspondiente (1-N) en mapas.txt

    Indices actuales:
      0  = Vacio              (reservado, no usar en mapas)
      1  = ParedPiedra        NORMAL
      2  = ParedLadrillo      NORMAL
      3  = ParedMadera        NORMAL
      4  = ParedAzul          NORMAL
      5  = ParedAcero         NORMAL
      6  = ParedAzulAlta      NORMAL  (muy alta)
      7  = BardaLadrilloBaja  NORMAL  (muy baja)
      8  = Arbol              ARBOL   (tronco + follaje, sin huecos laterales)
      9  = Arco               ARCO    (sin colision, solo marco superior)
================================================================
*/

constexpr int NUM_TIPOS_PARED = 10;

extern const std::array<TipoPared, NUM_TIPOS_PARED> TIPOS_PARED;
