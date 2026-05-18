#include "jugador.h"
#include "config.h"
#include "mapa.h"
#include <cmath>
#include <conio.h>

/*
================================================================
    jugador.cpp - Implementacion del modulo del jugador

    El movimiento aplica colision en cada eje por separado.
    Esto produce el efecto de "deslizamiento": si el jugador
    camina hacia una pared en diagonal, se desliza por ella
    en lugar de detenerse completamente.

    Los codigos de las teclas de flecha en Windows son de
    dos bytes: primero llega 224 (prefijo) y luego el codigo
    de la flecha especifica.
================================================================
*/

/*
    Codigos de tecla para Windows con conio.h.
    Agrupados en un namespace para evitar colisiones de nombres.
*/
namespace Tecla {
    constexpr int W         = 'w';
    constexpr int A         = 'a';
    constexpr int S         = 's';
    constexpr int D         = 'd';
    constexpr int ESC       = 27;
    constexpr int PREFIJO   = 224;  // Primer byte de teclas especiales
    constexpr int IZQUIERDA = 75;   // Segundo byte de flecha izquierda
    constexpr int DERECHA   = 77;   // Segundo byte de flecha derecha
}

/*
    Intenta mover al jugador hacia la nueva posicion (nx, ny).
    Evalua la colision en X e Y por separado para permitir
    deslizamiento a lo largo de las paredes.
    El margen evita que el jugador quede "pegado" a las paredes.
*/
static void moverConColision(Jugador& j, const Mapa& mapa, float nx, float ny) {
    constexpr float MARGEN = 0.25f;

    if (esPosicionLibre(mapa, nx, j.y + MARGEN) &&
        esPosicionLibre(mapa, nx, j.y - MARGEN)) {
        j.x = nx;
    }

    if (esPosicionLibre(mapa, j.x + MARGEN, ny) &&
        esPosicionLibre(mapa, j.x - MARGEN, ny)) {
        j.y = ny;
    }
}

/*
    Crea un jugador posicionado en las coordenadas de spawn del mapa.
*/
Jugador crearJugador(const Mapa& mapa) {
    return { mapa.spawnX, mapa.spawnY, mapa.spawnAngulo };
}

/*
    Lee la tecla presionada (si hay alguna) y actualiza el jugador.
    _kbhit() retorna 0 si no hay entrada, evitando bloquear el juego.
*/
bool procesarEntrada(Jugador& j, const Mapa& mapa) {
    if (!_kbhit()) return true;

    int tecla = _getch();

    if (tecla == Tecla::ESC) return false;

    // Vector de movimiento en la direccion de vision
    float dx = std::cos(j.angulo) * VEL_MOVIMIENTO;
    float dy = std::sin(j.angulo) * VEL_MOVIMIENTO;

    // Vector perpendicular para el strafe (desplazamiento lateral)
    float px = std::cos(j.angulo + PI * 0.5f) * VEL_MOVIMIENTO;
    float py = std::sin(j.angulo + PI * 0.5f) * VEL_MOVIMIENTO;

    if      (tecla == Tecla::W) { moverConColision(j, mapa, j.x + dx, j.y + dy); }
    else if (tecla == Tecla::S) { moverConColision(j, mapa, j.x - dx, j.y - dy); }
    else if (tecla == Tecla::A) { moverConColision(j, mapa, j.x - px, j.y - py); }
    else if (tecla == Tecla::D) { moverConColision(j, mapa, j.x + px, j.y + py); }
    else if (tecla == Tecla::PREFIJO) {
        int flecha = _getch();
        if      (flecha == Tecla::IZQUIERDA) j.angulo -= VEL_ROTACION;
        else if (flecha == Tecla::DERECHA)   j.angulo += VEL_ROTACION;
    }

    return true;
}
