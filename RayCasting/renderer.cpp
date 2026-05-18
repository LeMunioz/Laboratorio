#include "renderer.h"
#include "config.h"
#include "config_paredes.h"
#include "mapa.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <windows.h>

/*
    Declaraciones externas de las funciones de la libreria frontend.
    Sus definiciones son enlazadas desde main.cpp al incluir frontend.cpp.
*/
extern void gotoxy(int x, int y);
extern void color(int c);

/*
================================================================
    renderer.cpp - Implementacion del renderizador por raycasting

    ALGORITMO DDA (Digital Differential Analysis):
    -----------------------------------------------
    Para cada columna de la pantalla se lanza un rayo desde la
    posicion del jugador. El DDA avanza celda por celda del grid
    eligiendo siempre el eje (X o Y) cuya siguiente interseccion
    con la cuadricula este mas cerca.

    La distancia perpendicular elimina el efecto "ojo de pez" que
    ocurriria al usar la distancia euclidiana directa al punto de
    impacto: perpWallDist = sideDistX - deltaDX  (para lado X)

    SISTEMA DE BUFFER:
    ------------------
    En lugar de llamar gotoxy() por cada celda, el renderer llena
    un buffer 2D y luego lo vuelca fila por fila, agrupando
    caracteres consecutivos del mismo color en una sola llamada
    a color() seguida de varios puts(). Esto reduce drasticamente
    la cantidad de llamadas a la API de la consola de Windows.
================================================================
*/

// Buffer de pantalla: se llena cada frame antes de volcarse
static CeldaPantalla bufActual[ALTO_PANTALLA][ANCHO_PANTALLA];

// ----------------------------------------------------------------
// FUNCIONES AUXILIARES DEL BUFFER
// ----------------------------------------------------------------

/*
    Escribe un caracter y su color en la celda (x, y) del buffer.
    Las coordenadas fuera del rango se ignoran silenciosamente.
*/
static void setCelda(int x, int y, char ch, int col) {
    if (y < 0 || y >= ALTO_PANTALLA || x < 0 || x >= ANCHO_PANTALLA) return;
    bufActual[y][x] = { ch, col };
}

/*
    Vuelca el buffer completo a la consola.
    Optimizacion: una sola llamada a gotoxy() por fila y las llamadas
    a color() se agrupan por bloques consecutivos del mismo color.
*/
static void volcarBuffer() {
    for (int fila = 0; fila < ALTO_PANTALLA; fila++) {
        gotoxy(0, fila);
        int colorActual = -1;

        for (int col = 0; col < ANCHO_PANTALLA; col++) {
            const CeldaPantalla& cp = bufActual[fila][col];

            if (cp.col != colorActual) {
                color(cp.col);
                colorActual = cp.col;
            }

            std::cout.put(cp.ch);
        }
    }
    std::cout.flush();
}

// ----------------------------------------------------------------
// FUNCIONES DE SELECCION DE APARIENCIA
// ----------------------------------------------------------------

/*
    Retorna el caracter apropiado segun la distancia a la pared.
    Las paredes cercanas muestran la trama definida en el tipo;
    las lejanas usan caracteres menos densos para simular profundidad.
*/
static char seleccionarCaracterMuro(float dist, char tramaBase) {
    if (dist < SOMBRA_MUY_CERCA) return tramaBase;
    if (dist < SOMBRA_CERCA)     return '#';
    if (dist < SOMBRA_MEDIA)     return '+';
    if (dist < SOMBRA_LEJOS)     return ':';
    return '.';
}

/*
    Retorna el color de la pared considerando:
    - Distancia al jugador (cerca vs lejos)
    - Lado impactado: las caras horizontales se muestran
      un tono mas oscuro para simular iluminacion direccional
*/
static int seleccionarColorMuro(float dist, int tipoPared, bool ladoX) {
    const TipoPared& tp = TIPOS_PARED[tipoPared];
    int col = (dist < SOMBRA_CERCA) ? tp.colorCerca : tp.colorLejos;

    // Las caras horizontales (ladoX == false) son mas oscuras
    if (!ladoX && col > 1) col--;

    return col;
}

// ----------------------------------------------------------------
// MINIMAPA EN BUFFER
// ----------------------------------------------------------------

/*
    Escribe el minimapa 2D directamente en el buffer de pantalla,
    superpuesto en la esquina superior derecha de la escena.
    Al estar integrado en el buffer no produce parpadeo.
*/
static void dibujarMiniMapaEnBuffer(const Jugador& jugador, const Mapa& mapa) {
    // Posicion del minimapa en coordenadas de pantalla
    const int posX = ANCHO_PANTALLA - ANCHO_MINI - 1;
    const int posY = 0;

    // Calcular el desplazamiento del mapa para centrar en el jugador
    int camX = (int)jugador.x - ANCHO_MINI / 2;
    int camY = (int)jugador.y - ALTO_MINI  / 2;
    camX = std::max(0, std::min(camX, mapa.ancho - ANCHO_MINI));
    camY = std::max(0, std::min(camY, mapa.alto  - ALTO_MINI));

    // Posicion del jugador relativa a la ventana del minimapa
    int jMapX = (int)jugador.x - camX;
    int jMapY = (int)jugador.y - camY;

    for (int my = 0; my < ALTO_MINI; my++) {
        for (int mx = 0; mx < ANCHO_MINI; mx++) {
            int tipo = obtenerTipoCelda(mapa, camX + mx, camY + my);

            char ch;
            int  cl;

            if (mx == jMapX && my == jMapY) {
                // Marcador del jugador
                ch = '@';
                cl = 14;   // Crema / amarillo
            } else if (tipo > 0) {
                // Celda con pared
                ch = '#';
                cl = TIPOS_PARED[tipo].colorCerca;
            } else {
                // Celda vacia
                ch = ' ';
                cl = 8;
            }

            setCelda(posX + mx, posY + my, ch, cl);
        }
    }
}

// ----------------------------------------------------------------
// FUNCIONES PUBLICAS DEL RENDERER
// ----------------------------------------------------------------

/*
    Prepara la consola para el modo de juego:
    oculta el cursor y limpia el buffer con espacio en blanco.
*/
void inicializarPantalla() {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hCon, &ci);

    for (int f = 0; f < ALTO_PANTALLA; f++)
        for (int c = 0; c < ANCHO_PANTALLA; c++)
            bufActual[f][c] = { ' ', 8 };

    system("cls");
}

/*
    Implementacion del algoritmo DDA.
    Calcula la distancia perpendicular a la primera pared que
    intersecta el rayo lanzado en el angulo dado.
*/
RayoResultado lanzarRayo(const Jugador& jugador, const Mapa& mapa, float angulo) {
    RayoResultado res = { DIST_MAX, 0, false, false };

    float rayDX = std::cos(angulo);
    float rayDY = std::sin(angulo);

    // Distancia que recorre el rayo entre dos intersecciones de eje
    // Si la direccion en un eje es 0, la distancia es infinita (1e30)
    float deltaDX = (rayDX == 0.0f) ? 1e30f : std::abs(1.0f / rayDX);
    float deltaDY = (rayDY == 0.0f) ? 1e30f : std::abs(1.0f / rayDY);

    // Celda del mapa donde comienza el rayo
    int mapX = (int)jugador.x;
    int mapY = (int)jugador.y;

    int   stepX, stepY;
    float sideDistX, sideDistY;

    // Inicializar el paso y la distancia lateral hasta la primera interseccion
    if (rayDX < 0) {
        stepX     = -1;
        sideDistX = (jugador.x - mapX) * deltaDX;
    } else {
        stepX     = 1;
        sideDistX = (mapX + 1.0f - jugador.x) * deltaDX;
    }

    if (rayDY < 0) {
        stepY     = -1;
        sideDistY = (jugador.y - mapY) * deltaDY;
    } else {
        stepY     = 1;
        sideDistY = (mapY + 1.0f - jugador.y) * deltaDY;
    }

    // Avanzar celda por celda hasta encontrar una pared o agotar la distancia maxima
    bool ladoX   = false;
    int  maxPasos = (int)(DIST_MAX * 2);

    for (int i = 0; i < maxPasos; i++) {
        // Avanzar por el eje cuya proxima interseccion este mas cerca
        if (sideDistX < sideDistY) {
            sideDistX += deltaDX;
            mapX      += stepX;
            ladoX      = true;
        } else {
            sideDistY += deltaDY;
            mapY      += stepY;
            ladoX      = false;
        }

        int tipo = obtenerTipoCelda(mapa, mapX, mapY);

        if (tipo > 0) {
            res.golpeo    = true;
            res.tipoPared = tipo;
            res.ladoX     = ladoX;

            // Distancia perpendicular: evita el efecto ojo de pez
            res.distancia = ladoX
                ? (sideDistX - deltaDX)
                : (sideDistY - deltaDY);

            // Evitar division por cero en el calculo de altura
            if (res.distancia < 0.001f) res.distancia = 0.001f;
            break;
        }
    }

    return res;
}

/*
    Renderiza la escena 3D completa:
    Para cada columna de pantalla lanza un rayo y calcula la altura
    de la pared visible, luego dibuja tres zonas: cielo, pared, piso.
    Incluye el minimapa superpuesto antes de volcar el buffer.
*/
void renderizarFrame(const Jugador& jugador, const Mapa& mapa) {
    const int mitad = ALTO_PANTALLA / 2;

    for (int x = 0; x < ANCHO_PANTALLA; x++) {

        // Angulo del rayo desviado del centro segun la columna actual
        float offset  = ((float)x / ANCHO_PANTALLA - 0.5f) * FOV;
        float angRayo = jugador.angulo + offset;

        RayoResultado rayo = lanzarRayo(jugador, mapa, angRayo);

        // Calcular altura de la pared en pantalla
        float multAltura = (rayo.golpeo) ? TIPOS_PARED[rayo.tipoPared].altura : 1.0f;
        float dist       = (rayo.golpeo) ? rayo.distancia : DIST_MAX;
        float altMuro    = ((float)ALTO_PANTALLA / dist) * multAltura;

        int inicio = (int)(mitad - altMuro * 0.5f);
        int fin    = (int)(mitad + altMuro * 0.5f);
        inicio     = std::max(0,               inicio);
        fin        = std::min(ALTO_PANTALLA - 1, fin);

        // Apariencia de la pared segun tipo y distancia
        char charMuro  = ' ';
        int  colorMuro = 8;

        if (rayo.golpeo) {
            charMuro  = seleccionarCaracterMuro(dist, TIPOS_PARED[rayo.tipoPared].trama);
            colorMuro = seleccionarColorMuro(dist, rayo.tipoPared, rayo.ladoX);
        }

        // -- Dibujar columna completa en el buffer --

        for (int y = 0; y < ALTO_PANTALLA; y++) {

            if (y < inicio) {
                // CIELO: gradiente descendente (mas oscuro arriba, mas claro al horizonte)
                float t  = (float)y / (float)mitad;
                int   cl = (t < 0.25f) ? 1 : COLOR_CIELO;
                setCelda(x, y, CHAR_CIELO, cl);

            } else if (y <= fin) {
                // PARED
                setCelda(x, y, charMuro, colorMuro);

            } else {
                // PISO: gradiente ascendente (mas oscuro al horizonte, mas visible abajo)
                float t  = (float)(y - mitad) / (float)mitad;
                char  ch = (t > 0.45f) ? CHAR_PISO : ' ';
                int   cl = (t > 0.65f) ? 7 : COLOR_PISO;
                setCelda(x, y, ch, cl);
            }
        }
    }

    // Superponer el minimapa sobre la escena antes de volcar
    dibujarMiniMapaEnBuffer(jugador, mapa);

    volcarBuffer();
}

/*
    Dibuja la barra de estado debajo de la escena 3D.
    Escribe directamente en la consola ya que esta fuera del
    area del buffer (por debajo de la fila ALTO_PANTALLA).
*/
void dibujarHUD(const Jugador& jugador, const Mapa& mapa) {
    // Separador horizontal
    gotoxy(0, ALTO_PANTALLA);
    color(8);
    for (int i = 0; i < ANCHO_PANTALLA; i++) std::cout.put('-');

    // Controles
    gotoxy(1, ALTO_PANTALLA + 1);
    color(8);
    std::cout << "WASD: Mover";

    gotoxy(14, ALTO_PANTALLA + 1);
    color(8);
    std::cout << "Flechas: Girar";

    gotoxy(30, ALTO_PANTALLA + 1);
    color(8);
    std::cout << "ESC: Salir";

    // Nombre del mapa activo
    gotoxy(45, ALTO_PANTALLA + 1);
    color(14);
    std::cout << "[ " << mapa.nombre << " ]";

    // Posicion actual del jugador
    char posStr[32];
    snprintf(posStr, sizeof(posStr), "X:%.1f  Y:%.1f", jugador.x, jugador.y);
    gotoxy(ANCHO_PANTALLA - 20, ALTO_PANTALLA + 1);
    color(11);
    std::cout << posStr;

    color(15);
}
