#include "config_paredes.h"

/*
================================================================
    config_paredes.cpp - Valores de la tabla de tipos de pared

    Edita unicamente este archivo para cambiar la apariencia de
    las paredes: colores, altura y caracter de textura.
    No es necesario tocar ningun otro modulo del motor.

    Campos de TipoPared:
        colorCerca  - color cuando el jugador esta cerca (dist < 5)
        colorLejos  - color cuando el jugador esta lejos (dist >= 5)
        altura      - multiplicador de altura de la pared en pantalla
        trama       - caracter que representa la textura de la pared
        nombre      - nombre descriptivo (solo para depuracion)
================================================================
*/

const std::array<TipoPared, NUM_TIPOS_PARED> TIPOS_PARED = {{
    /*  colorCerca  colorLejos  altura  trama   nombre            */
    {   0,          0,          1.0f,   ' ',    "Vacio"           },  // 0
    {   7,          8,          1.0f,   '#',    "Piedra"          },  // 1
    {   9,          1,          1.0f,   '#',    "Muro Azul"       },  // 2
    {   2,          8,          1.6f,   '|',    "Columna Alta"    },  // 3
    {   4,          8,          1.0f,   '#',    "Ladrillo Rojo"   },  // 4
    {   6,          8,          0.55f,  '-',    "Barda Baja"      },  // 5
    {  10,          8,          1.0f,   '%',    "Madera"          },  // 6
    {  11,          1,          2.2f,   '|',    "Torre Alta"      },  // 7
    {   5,          8,          1.0f,   '#',    "Muro Violeta"    },  // 8
    {  14,          8,          1.0f,   '=',    "Placa de Metal"  },  // 9
}};
