#pragma once
#include "tipos.h"
#include <vector>
#include <string>

/*
================================================================
    mapa.h - Declaraciones del modulo de gestion de mapas

    Responsabilidades de este modulo:
      - Cargar y parsear mapas desde un archivo de texto
      - Consultar el tipo de celda en coordenadas dadas
      - Verificar si una posicion flotante es transitable

    Ver mapas.txt para el formato detallado del archivo.
================================================================
*/

/*
    Lee el archivo indicado y construye la lista de mapas.
    Cada bloque [MAPA]...[FIN] en el archivo genera un Mapa.
    Retorna un vector vacio si el archivo no existe o esta vacio.
*/
std::vector<Mapa> cargarMapas(const std::string& ruta);

/*
    Retorna true si la posicion flotante (x, y) corresponde a una
    celda vacia (tipo == 0) y esta dentro de los limites del mapa.
*/
bool esPosicionLibre(const Mapa& mapa, float x, float y);

/*
    Retorna el tipo de celda en las coordenadas enteras (cx, cy).
    Retorna 1 (pared solida) si las coordenadas estan fuera del mapa,
    garantizando que el jugador nunca salga de los limites.
*/
int obtenerTipoCelda(const Mapa& mapa, int cx, int cy);
