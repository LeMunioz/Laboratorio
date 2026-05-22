#include "config_paredes.h"

/*
================================================================
    config_paredes.cpp - Valores de la tabla de tipos de pared

    Edita unicamente este archivo para cambiar la apariencia de
    las paredes: colores, altura y caracter de textura.
    No es necesario tocar ningun otro modulo del motor.

    Campos de TipoPared:
        colorCerca  - color cuando el jugador esta cerca
        colorLejos  - color cuando el jugador esta lejos
        altura      - multiplicador de altura de la pared en pantalla
        trama       - string con patron de textura personalizado
        nombre      - nombre descriptivo (solo para depuracion)
================================================================
*/

const std::array<TipoPared, NUM_TIPOS_PARED> TIPOS_PARED = {{
    /*  colorCerca  colorLejos  altura  trama           nombre              */
    {   0,          0,          1.0f,   "",             "Vacio"             },  // 0
    {   96,         102,        1.0f,   "| |",          "Pared Madera"      },  // 1
    {   135,        136,        1.0f,   "{ }",          "Pared Piedra"      },  // 2
    {   332,        324,        1.0f,   "[  ]",         "Pared Ladrillo"    },  // 3
    {   313,        307,        1.0f,   "\\\\",         "Pared Azul"        },  // 4
    {   399,        391,        1.0f,   "==--",         "Pared Acero"       },  // 5
    {   313,        307,        2.5f,   "\\ \\",        "Pared Azul Alta"   },  // 6
    {   332,        324,        0.5f,   "[ ]",          "Pared Ladrillo Baja"}, // 7
}};

/*
    Paredes especiales que se renderizан como composiciones de dos pisos.
    Indice 0 = Arbol (base angosta + copa)
    Indice 1 = Poste (elemento unico pero especial)
*/
const std::array<ParedEspecial, NUM_PAREDES_ESPECIALES> PAREDES_ESPECIALES = {{
    // Arbol: base con copa
    {
        true,           // activa
        "Arbol",        // nombre
        
        // Parte inferior (tronco)
        0.5f,           // alturaInf
        96,             // colorCercaInf
        102,            // colorLejosInf
        "| |",          // tramaInf
        
        // Parte superior (copa)
        1.2f,           // alturaSup
        298,            // colorCercaSup
        290,            // colorLejosSup
        "# #"           // tramaSup
    },
    
    // Poste: columna delgada
    {
        true,           // activa
        "Poste",        // nombre
        
        // Parte inferior (base - basicamente todo el poste)
        1.5f,           // alturaInf
        382,            // colorCercaInf
        375,            // colorLejosInf
        "| | ",         // tramaInf
        
        // Parte superior (no se usa, pero definida)
        0.0f,           // alturaSup
        375,            // colorCercaSup
        375,            // colorLejosSup
        ""              // tramaSup
    }
}};

/*
    Tipos de cielo disponibles para los mapas.
    Los cielos pueden tener patrones visuales como estrellas.
*/
const std::array<TipoCielo, NUM_TIPOS_CIELO> TIPOS_CIELO = {{
    {   307,        "",                 "Cielo"         },  // 0: Cielo normal
    {   15,         " .  .     .",      "Cielo Noche"   },  // 1: Noche con estrellas
    {   6,          ".",                "Techo"         },  // 2: Techo interior
}};

/*
    Tipos de suelo disponibles para los mapas.
    Los suelos pueden tener patrones visuales como pasto o ladrillo.
*/
const std::array<TipoSuelo, NUM_TIPOS_SUELO> TIPOS_SUELO = {{
    {   42,         "/ \\/",            "Pasto"         },  // 0: Hierba
    {   8,          "-=-",              "Piso"          },  // 1: Piso gris
}};
