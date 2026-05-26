#include "mapa.h"
#include "config_paredes.h"
#include <fstream>
#include <sstream>

/*
================================================================
    mapa.cpp - Implementacion del modulo de mapas

    El parser lee el archivo linea por linea buscando bloques
    [MAPA]...[FIN]. Dentro de cada bloque procesa metadatos
    (nombre=, spawn=) y luego las filas del grid de digitos.

    Reglas del parser:
      - Las lineas que empiezan con '#' son comentarios
      - Las lineas vacias se ignoran
      - El grid se lee despues de la linea de dimensiones
      - Cada caracter del grid debe ser un digito '0'-'9'
================================================================
*/

/*
    Convierte una linea de texto en un vector de Celdas.
    Solo procesa caracteres que son digitos; ignora el resto.
    Esto permite agregar separadores visuales en el archivo.
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
    Usa una maquina de estados simple: fuera de mapa, dentro
    de mapa (metadatos) y dentro de grid (filas de celdas).
*/
std::vector<Mapa> cargarMapas(const std::string& ruta) {
    std::vector<Mapa> mapas;
    std::ifstream archivo(ruta);

    if (!archivo.is_open()) return mapas;

    std::string linea;
    Mapa        actual;
    bool        enMapa      = false;
    bool        enGrid      = false;
    int         filasLeidas = 0;

    while (std::getline(archivo, linea)) {

        if (linea.empty() || linea[0] == '#') continue;

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

        if (linea.rfind("nombre=", 0) == 0) {
            actual.nombre = linea.substr(7);
            continue;
        }

        if (linea.rfind("spawn=", 0) == 0) {
            std::istringstream ss(linea.substr(6));
            ss >> actual.spawnX >> actual.spawnY >> actual.spawnAngulo;
            continue;
        }

        // Primera linea sin '=' dentro del bloque: dimensiones del grid
        if (!enGrid && linea.find('=') == std::string::npos) {
            std::istringstream ss(linea);
            if (ss >> actual.ancho >> actual.alto) {
                actual.celdas.clear();
                enGrid      = true;
                filasLeidas = 0;
            }
            continue;
        }

        if (enGrid && filasLeidas < actual.alto) {
            auto fila = parsearFila(linea);
            fila.resize(actual.ancho, { 0 });
            actual.celdas.push_back(fila);
            filasLeidas++;
        }
    }

    return mapas;
}

/*
    Verifica si la posicion flotante (x, y) es transitable.

    Una celda es transitable si:
      a) Esta vacia (tipo == 0), O
      b) Su tipo tiene sinColision == true (ej: arcos)

    Las coordenadas fuera de los limites del mapa NO son transitables.
*/
bool esPosicionLibre(const Mapa& mapa, float x, float y) {
    int cx = (int)x;
    int cy = (int)y;

    if (cx < 0 || cy < 0 || cx >= mapa.ancho || cy >= mapa.alto) return false;

    int tipo = mapa.celdas[cy][cx].tipo;

    if (tipo == 0) return true;

    // Consultar la tabla: algunos tipos (ej: ARCO) no tienen colision fisica
    return TIPOS_PARED[tipo].sinColision;
}

/*
    Retorna el tipo de celda en coordenadas enteras.
    Los limites del mapa se tratan como tipo 1 (pared solida).
*/
int obtenerTipoCelda(const Mapa& mapa, int cx, int cy) {
    if (cx < 0 || cy < 0 || cx >= mapa.ancho || cy >= mapa.alto) return 1;
    return mapa.celdas[cy][cx].tipo;
}
