#pragma once
#include "tipos.h"
#include <thread>
#include <mutex>
#include <vector>

/*
================================================================
    renderer.h - Declaraciones del modulo de renderizado

    Implementa el algoritmo DDA (Digital Differential Analysis)
    de raycasting para proyectar la escena 3D en la consola.
    
    NUEVO: Soporte para multihilo con 3 hilos principales:
      - Hilo 1: Raycasting y calculo de distancias
      - Hilo 2: Dibujado en buffer
      - Hilo 3: Minimapa (en standby para futuras funciones)

    Flujo de renderizado por frame:
      1. renderizarFrame() coordina los 3 hilos
      2. Hilo raycaster calcula la geometria
      3. Hilo dibujador rellena el buffer
      4. Hilo minimapa dibuja sobre el buffer
      5. El buffer se vuelca a la consola
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
    Renderiza un frame completo usando paralelismo:
      - Hilo 1: Raycasting en paralelo para multiples columnas
      - Hilo 2: Dibujado en paralelo basado en resultados de raycasting
      - Hilo 3: Minimapa (actualmente en standby)
    Todo se escribe en el buffer interno y luego se vuelca.
*/
void renderizarFrame(const Jugador& jugador, const Mapa& mapa);

/*
    Dibuja la barra de informacion (HUD) debajo de la escena.
    Se llama despues de renderizarFrame() ya que escribe
    directamente en la consola fuera del area del buffer.
*/
void dibujarHUD(const Jugador& jugador, const Mapa& mapa);

/*
    Retorna el caracter apropiado de la trama de un tipo de pared
    segun la distancia al jugador. Maneja tramas personalizadas.
*/
char seleccionarCaracterTramaDistancia(float dist, const std::string& trama);

/*
    Retorna el color de una pared considerando distancia y lado impactado.
    Las caras horizontales se oscurecen ligeramente para simular iluminacion.
*/
int seleccionarColorMuro(float dist, int tipoPared, bool ladoX);

/*
    Dibuja un caracter de cielo en la columna especificada.
    Usa el tipo de cielo del mapa y aplica patrones solo en los extremos.
*/
void dibujarCielo(int x, int y, int idCielo, int distanciaPantalla);

/*
    Dibuja un caracter de suelo en la columna especificada.
    Usa el tipo de suelo del mapa y aplica patrones solo en los extremos.
*/
void dibujarSuelo(int x, int y, int idSuelo, int distanciaPantalla);
