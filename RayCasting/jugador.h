#pragma once
#include "tipos.h"

/*
================================================================
    jugador.h - Declaraciones del modulo del jugador

    Este modulo maneja todo lo relacionado con el jugador:
    su estado inicial, su movimiento, las colisiones contra
    paredes y la lectura de entrada del teclado.

    Controles:
        W / S           : Avanzar / Retroceder
        A / D           : Desplazamiento lateral (strafe)
        Flecha Izq/Der  : Girar la camara
        ESC             : Salir del juego
================================================================
*/

/*
    Crea e inicializa un jugador en la posicion de spawn
    definida en el mapa recibido.
*/
Jugador crearJugador(const Mapa& mapa);

/*
    Lee la entrada del teclado (no bloqueante) y actualiza la
    posicion y el angulo del jugador aplicando deteccion de
    colisiones contra las paredes del mapa.

    Retorna false si el usuario presiono ESC (solicita salir).
    Retorna true en cualquier otro caso.
*/
bool procesarEntrada(Jugador& jugador, const Mapa& mapa);
