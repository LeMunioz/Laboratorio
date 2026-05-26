#pragma once
#include "tipos.h"

/*
================================================================
    renderer.h - Declaraciones del modulo de renderizado

    Implementa el algoritmo DDA (Digital Differential Analysis)
    de raycasting para proyectar la escena 3D en la consola.

    Flujo de renderizado por frame:
      1. renderizarFrame() llena el buffer con la escena 3D
         y el minimapa superpuesto.
      2. El buffer se vuelca a la consola agrupando caracteres
         del mismo color para minimizar llamadas a la API.
      3. dibujarHUD() dibuja la barra de info debajo del buffer.
================================================================
*/

/*
    Inicializa la pantalla para el juego:
      - Oculta el cursor para evitar parpadeo
      - Inicializa el buffer de pantalla en blanco
    Debe llamarse una vez antes de entrar al bucle del juego.
*/
void inicializarPantalla();

/*
    Lanza un rayo desde la posicion del jugador en el angulo dado.
    Usa el algoritmo DDA para recorrer el grid celda por celda y
    encontrar la primera pared impactada.
    Retorna la distancia perpendicular, el tipo de pared y el lado.
*/
RayoResultado lanzarRayo(const Jugador& jugador, const Mapa& mapa, float angulo);

/*
    Renderiza un frame completo:
      - Escena 3D por columnas (cielo, pared, piso)
      - Minimapa 2D superpuesto en la esquina superior derecha
    Todo se escribe en el buffer interno y luego se vuelca.
*/
void renderizarFrame(const Jugador& jugador, const Mapa& mapa);

/*
    Dibuja la barra de informacion (HUD) debajo de la escena.
    Se llama despues de renderizarFrame() ya que escribe
    directamente en la consola fuera del area del buffer.
*/
void dibujarHUD(const Jugador& jugador, const Mapa& mapa);
