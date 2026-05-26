#include "config_paredes.h"

/*
================================================================
    config_paredes.cpp - Valores visuales de cada tipo de pared

    Edita SOLO este archivo para cambiar la apariencia de las
    paredes. No es necesario modificar ningun otro modulo.

    REFERENCIA DE COMPORTAMIENTOS:
      ComportamientoPared::NORMAL  -> Pared solida estandar
      ComportamientoPared::ARBOL   -> Tronco (colorCerca) + Follaje (colorLejos)
                                      tramaCerca = char del tronco
                                      tramaLejos = char del follaje
      ComportamientoPared::ARCO    -> Sin colision, solo marco superior
                                      sinColision debe ser true

    CAMPOS:
      colorCerca    colorLejos    colorMuyLejos    colorSombra
      altura        tramaCerca    tramaLejos
      sinColision   comportamiento    nombre
================================================================
*/

const std::array<TipoPared, NUM_TIPOS_PARED> TIPOS_PARED = {{

    /*  cCerca  cLejos  cMuyLejos  cSombra  altura  tramaCerca tramaLejos sinCol  comportamiento                nombre        */

    {   0,      0,      0,         0,       1.0f,   ' ',       ' ',       false,  ComportamientoPared::NORMAL,  "Vacio"               },  // 0

    {   128,    136,    8,         16,      2.0f,   '#',       '-',       false,  ComportamientoPared::NORMAL,  "ParedPiedra"         },  // 1

    {   76,     68,     8,         85,      2.0f,   '+',       '-',       false,  ComportamientoPared::NORMAL,  "ParedLadrillo"       },  // 2

    {   96,     102,    8,         16,      2.0f,   '%',       '/',       false,  ComportamientoPared::NORMAL,  "ParedMadera"         },  // 3

    {   315,    307,    8,         25,      2.0f,   '#',       '-',       false,  ComportamientoPared::NORMAL,  "ParedAzul"           },  // 4

    {   135,    136,    8,         24,      2.0f,   '=',       '-',       false,  ComportamientoPared::NORMAL,  "ParedAcero"          },  // 5

    {   191,    185,    8,         23,      3.5f,   '|',       ':',       false,  ComportamientoPared::NORMAL,  "ParedAzulAlta"       },  // 6

    {   76,     68,     8,         84,      0.5f,   '@',       'O',       false,  ComportamientoPared::NORMAL,  "BardaLadrilloBaja"   },  // 7

    /*
        ARBOL: El renderer usa los campos de esta forma:
          colorCerca / colorMuyLejos / colorSombra -> color del tronco
          colorLejos                               -> color del follaje
          tramaCerca                               -> caracter del tronco
          tramaLejos                               -> caracter del follaje
          altura                                   -> multiplicador de altura del follaje
    */
    //CTroncoC, CHojasC, CTroncoL, CTroncoS,
    {   96,     162,    6,        88,      2.0f,   '|',       '8',       false,  ComportamientoPared::ARBOL,   "Arbol"               },  // 8

    /*
        ARCO: sinColision = true. El rayo pasa a traves y solo
        se renderiza el marco superior (FRACCION_ARCO del alto total).
        El jugador puede caminar por el hueco inferior.
    */
    {   128,    136,    8,         16,      2.5f,   '#',       '-',       true,   ComportamientoPared::ARCO,    "Arco"                },  // 9

}};
