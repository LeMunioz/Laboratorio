#include "mapa.h"
#include <fstream>
#include <sstream>

/*
================================================================
    mapa.cpp - Implementacion del modulo de mapas

    El parser lee el archivo linea por linea buscando bloques
    [MAPA]...[FIN]. Dentro de cada bloque procesa metadatos
    (nombre=, spawn=) y luego las filas del grid de digitos.

    Reglas del parser:
      - Las lineas que comienzan con '#' son comentarios
      - Las lineas vacias se ignoran
      - El grid se lee despues de la linea de dimensiones
      - Cada caracter del grid debe ser un digito '0'-'9'
================================================================
*/

/*
    Convierte una linea de texto en un vector de celdas.
    Solo procesa los caracteres que son digitos; ignora el resto.
    Esto permite tener separadores visuales en el archivo de mapas.
*/
static std::vector<Celda> parsearFila(const std::string& linea) {
    std::vector<Celda> fila;
    for (char c : linea) {
        if (c >= '0' && c <= '9') {
            fila.push_back({ c - '0' });
        }
    }
    return fila;
}

/*
    Lee el archivo completo y construye la lista de mapas.
    Usa una maquina de estados simple: fuera de mapa, dentro de
    mapa (leyendo metadatos), dentro de grid (leyendo filas).
*/
std::vector<Mapa> cargarMapas(const std::string& ruta) {
    std::vector<Mapa> mapas;
    std::ifstream archivo(ruta);

    if (!archivo.is_open()) return mapas;

    std::string linea;
    Mapa        actual;
    bool        enMapa     = false;
    bool        enGrid     = false;
    int         filasLeidas = 0;

    while (std::getline(archivo, linea)) {

        // Ignorar comentarios y lineas vacias
        if (linea.empty() || linea[0] == '#') continue;

        // -- Marcadores de bloque --

        if (linea == "[MAPA]") {
            actual      = Mapa{};
            enMapa      = true;
            enGrid      = false;
            filasLeidas = 0;
            continue;
        }

        if (linea == "[FIN]") {
            if (enMapa) mapas.push_back(actual);
            enMapa = false;
            enGrid = false;
            continue;
        }

        if (!enMapa) continue;

        // -- Metadatos del mapa --

        if (linea.rfind("nombre=", 0) == 0) {
            actual.nombre = linea.substr(7);
            continue;
        }

        if (linea.rfind("spawn=", 0) == 0) {
            std::istringstream ss(linea.substr(6));
            ss >> actual.spawnX >> actual.spawnY >> actual.spawnAngulo;
            continue;
        }

        // -- Dimensiones del grid (primera linea sin '=') --

        if (!enGrid && linea.find('=') == std::string::npos) {
            std::istringstream ss(linea);
            if (ss >> actual.ancho >> actual.alto) {
                actual.celdas.clear();
                enGrid      = true;
                filasLeidas = 0;
            }
            continue;
        }

        // -- Filas del grid --

        if (enGrid && filasLeidas < actual.alto) {
            auto fila = parsearFila(linea);
            fila.resize(actual.ancho, { 0 });   // Completar con celdas vacias si falta ancho
            actual.celdas.push_back(fila);
            filasLeidas++;
        }
    }

    return mapas;
}

/*
    Verifica si la celda en la posicion flotante (x, y) esta libre.
    Se convierte a entero truncando hacia abajo (floor implicito).
*/
bool esPosicionLibre(const Mapa& mapa, float x, float y) {
    int cx = (int)x;
    int cy = (int)y;
    if (cx < 0 || cy < 0 || cx >= mapa.ancho || cy >= mapa.alto) return false;
    return mapa.celdas[cy][cx].tipo == 0;
}

/*
    Retorna el tipo de celda en coordenadas enteras.
    Los limites del mapa se tratan como paredes solidas (tipo 1).
*/
int obtenerTipoCelda(const Mapa& mapa, int cx, int cy) {
    if (cx < 0 || cy < 0 || cx >= mapa.ancho || cy >= mapa.alto) return 1;
    return mapa.celdas[cy][cx].tipo;
}
