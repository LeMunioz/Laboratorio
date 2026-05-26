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
    Declaraciones de la libreria frontend (enlazadas desde main.cpp).
*/
extern void gotoxy(int x, int y);
extern void color(int c);

/*
================================================================
    renderer.cpp - Renderizador 3D por raycasting (algoritmo DDA)

    PIPELINE DE RENDERIZADO POR COLUMNA:
    ─────────────────────────────────────
    Para cada columna X (0..ANCHO_PANTALLA-1):

      1. Lanzar rayo con DDA.
         El DDA avanza celda a celda eligiendo siempre el eje
         cuya proxima interseccion con el grid este mas cerca.
         Si encuentra un ARCO (sinColision=true): lo registra
         y CONTINUA. Si encuentra pared solida: se detiene.

      2. Paso "Fondo": rellenar la columna completa con cielo/piso.

      3. Paso "Pared": si golpeo una pared solida, dibujar encima:
           NORMAL -> banda vertical centrada en el horizonte
           ARBOL  -> tronco (banda baja) + follaje (banda alta)

      4. Paso "Overlay de arco": si habia un arco en la trayectoria,
         dibujar el marco superior encima de lo que ya hay.

    SISTEMA DE COLOR DE 4 ESTADOS:
    ─────────────────────────────
      dist < DIST_CERCA  : colorCerca
      dist < DIST_LEJOS  : colorLejos
      dist >= DIST_LEJOS : colorMuyLejos  (normalmente gris 8)
      !ladoX (sombra)    : colorSombra  (sustituye a todo lo anterior)

    SISTEMA DE CARACTER DE 2 ESTADOS:
    ──────────────────────────────────
      dist < DIST_CHAR   : tramaCerca
      dist >= DIST_CHAR  : tramaLejos
================================================================
*/

// Buffer interno de pantalla; se llena cada frame y luego se vuelca
static CeldaPantalla bufActual[ALTO_PANTALLA][ANCHO_PANTALLA];

// ────────────────────────────────────────────────────────────────
// FUNCIONES DEL BUFFER
// ────────────────────────────────────────────────────────────────

/*
    Escribe un caracter y color en la celda (x, y) del buffer.
    Coordenadas fuera de rango se ignoran silenciosamente.
*/
static void setCelda(int x, int y, char ch, int col) {
    if (y < 0 || y >= ALTO_PANTALLA || x < 0 || x >= ANCHO_PANTALLA) return;
    bufActual[y][x] = { ch, col };
}

/*
    Vuelca el buffer a la consola.
    Optimizacion: una llamada a gotoxy() por fila y las llamadas
    a color() se agrupan por bloques de igual color consecutivos.
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

// ────────────────────────────────────────────────────────────────
// SELECCION DE COLOR Y CARACTER
// ────────────────────────────────────────────────────────────────

/*
    Selecciona el caracter de la pared segun la distancia.
    Cerca del jugador se usa tramaCerca, lejos tramaLejos.
*/
static char seleccionarCaracter(float dist, const TipoPared& tp) {
    return (dist < DIST_CHAR) ? tp.tramaCerca : tp.tramaLejos;
}

/*
    Selecciona el color de la pared segun distancia y lado impactado.
    Las caras horizontales (sombra) usan siempre colorSombra.
*/
static int seleccionarColor(float dist, int tipoPared, bool ladoX) {
    const TipoPared& tp = TIPOS_PARED[tipoPared];

    if (!ladoX) return tp.colorSombra;

    if (dist < DIST_CERCA) return tp.colorCerca;
    if (dist < DIST_LEJOS) return tp.colorLejos;
    return tp.colorMuyLejos;
}

// ────────────────────────────────────────────────────────────────
// RENDERIZADO DE TIPOS ESPECIALES
// ────────────────────────────────────────────────────────────────

/*
    Dibuja un arbol en la columna X.

    El arbol se compone de dos bandas sobre el fondo ya dibujado:
      - FOLLAJE: banda alta, mas grande, color tp.colorLejos, char tp.tramaLejos
      - TRONCO:  banda baja anclada al suelo, delgada, color tp.colorCerca, char tp.tramaCerca

    La separacion entre ambas bandas deja ver el fondo (cielo),
    produciendo el efecto de transparencia del follaje.

    Disposicion en pantalla (Y crece hacia abajo):
      suelo = mitad + altBase/2
      troncFin = suelo
      troncIni = suelo - altBase * FRAC_TRONCO
      follFin  = troncIni + altBase * FRAC_FOLLAJE_OVERLAP  (leve solapamiento)
      follIni  = follFin - altBase * tp.altura
*/
static void renderizarColumnaArbol(int x, float dist, int tipoPared, bool ladoX) {
    const TipoPared& tp    = TIPOS_PARED[tipoPared];
    const int        mitad = ALTO_PANTALLA / 2;

    float altBase = (float)ALTO_PANTALLA / dist;

    // -- Tronco --
    int troncFin = std::min((int)(mitad + altBase * 0.5f), ALTO_PANTALLA - 1);
    int troncIni = std::max((int)(troncFin - altBase * FRAC_TRONCO), 0);

    // Color del tronco: respeta sombra y distancia
    int colorTronco = !ladoX
        ? tp.colorSombra
        : (dist < DIST_CERCA ? tp.colorCerca : tp.colorMuyLejos);

    char charTronco = tp.tramaCerca;   // Tronco siempre usa tramaCerca ('|')

    // -- Follaje: se apoya sobre la cima del tronco --
    float altFollaje = altBase * tp.altura;
    int follFin = std::min((int)(troncIni + altBase * FRAC_FOLLAJE_OVERLAP), ALTO_PANTALLA - 1);
    int follIni = std::max((int)(follFin - altFollaje), 0);

    int  colorFollaje = tp.colorLejos;   // Follaje siempre usa colorLejos (verde)
    char charFollaje  = tp.tramaLejos;   // Follaje usa tramaLejos ('#')

    // Dibujar follaje primero (puede quedar debajo del tronco en borde)
    for (int y = follIni; y <= follFin; y++)
        setCelda(x, y, charFollaje, colorFollaje);

    // Dibujar tronco encima
    for (int y = troncIni; y <= troncFin; y++)
        setCelda(x, y, charTronco, colorTronco);
}

/*
    Dibuja el marco superior de un arco como overlay en la columna X.

    Solo se pinta la fraccion FRACCION_ARCO del alto total que tendria
    la pared si fuera solida. La parte inferior queda como estaba en
    el buffer (fondo o pared detras), creando el hueco del arco.

    Esto se llama DESPUES de haber pintado el fondo y la pared
    solida detras del arco, por lo que la escena se ve a traves.
*/
static void renderizarColumnaArco(int x, float dist, int tipoPared, bool ladoX) {
    const TipoPared& tp    = TIPOS_PARED[tipoPared];
    const int        mitad = ALTO_PANTALLA / 2;

    float altTotal = ((float)ALTO_PANTALLA / dist) * tp.altura;

    int arcoIni = std::max(0, (int)(mitad - altTotal * 0.5f));
    int arcoFin = std::min(ALTO_PANTALLA - 1,
                           (int)(arcoIni + altTotal * FRACCION_ARCO));

    char charArco  = seleccionarCaracter(dist, tp);
    int  colorArco = seleccionarColor(dist, tipoPared, ladoX);

    for (int y = arcoIni; y <= arcoFin; y++)
        setCelda(x, y, charArco, colorArco);
}

// ────────────────────────────────────────────────────────────────
// MINIMAPA EN BUFFER
// ────────────────────────────────────────────────────────────────

/*
    Escribe el minimapa 2D en la esquina superior derecha del buffer.
    Cada celda del mapa se representa con un caracter segun su tipo:
      '@' = jugador
      'T' = arbol
      ':' = arco (transitable)
      '#' = pared normal (color del tipo)
      ' ' = espacio vacio
*/
static void dibujarMiniMapaEnBuffer(const Jugador& jugador, const Mapa& mapa) {
    const int posX = ANCHO_PANTALLA - ANCHO_MINI - 1;
    const int posY = 0;

    // Centrar la ventana del minimapa en el jugador
    int camX = (int)jugador.x - ANCHO_MINI / 2;
    int camY = (int)jugador.y - ALTO_MINI  / 2;
    camX = std::max(0, std::min(camX, mapa.ancho - ANCHO_MINI));
    camY = std::max(0, std::min(camY, mapa.alto  - ALTO_MINI));

    int jMapX = (int)jugador.x - camX;
    int jMapY = (int)jugador.y - camY;

    for (int my = 0; my < ALTO_MINI; my++) {
        for (int mx = 0; mx < ANCHO_MINI; mx++) {
            int tipo = obtenerTipoCelda(mapa, camX + mx, camY + my);

            char ch;
            int  cl;

            if (mx == jMapX && my == jMapY) {
                ch = '@'; cl = 14;   // Jugador: crema
            } else if (tipo <= 0) {
                ch = ' '; cl = 8;   // Vacio
            } else {
                const TipoPared& tp = TIPOS_PARED[tipo];
                switch (tp.comportamiento) {
                    case ComportamientoPared::ARBOL:
                        ch = 'T'; cl = tp.colorLejos;    // Verde del follaje
                        break;
                    case ComportamientoPared::ARCO:
                        ch = ':'; cl = tp.colorCerca;    // Marco, transitable
                        break;
                    default:
                        ch = '#'; cl = tp.colorCerca;
                        break;
                }
            }

            setCelda(posX + mx, posY + my, ch, cl);
        }
    }
}

// ────────────────────────────────────────────────────────────────
// FUNCIONES PUBLICAS
// ────────────────────────────────────────────────────────────────

/*
    Inicializa la pantalla para el modo de juego:
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

    Avanza celda a celda por el grid eligiendo siempre el eje
    (X o Y) cuya proxima interseccion con la cuadricula este mas cerca.

    Comportamiento ante distintos tipos de celda:
      tipo == 0              -> ignorar, continuar
      sinColision == true    -> registrar como arco y continuar
      sinColision == false   -> registrar como pared solida y detener
*/
RayoResultado lanzarRayo(const Jugador& jugador, const Mapa& mapa, float angulo) {
    RayoResultado res = {};
    res.distancia     = DIST_MAX;
    res.golpeo        = false;
    res.distanciaArco = DIST_MAX;
    res.hayArco       = false;

    float rayDX = std::cos(angulo);
    float rayDY = std::sin(angulo);

    float deltaDX = (rayDX == 0.0f) ? 1e30f : std::abs(1.0f / rayDX);
    float deltaDY = (rayDY == 0.0f) ? 1e30f : std::abs(1.0f / rayDY);

    int mapX = (int)jugador.x;
    int mapY = (int)jugador.y;

    int   stepX, stepY;
    float sideDistX, sideDistY;

    if (rayDX < 0) { stepX = -1; sideDistX = (jugador.x - mapX) * deltaDX; }
    else            { stepX =  1; sideDistX = (mapX + 1.0f - jugador.x) * deltaDX; }

    if (rayDY < 0) { stepY = -1; sideDistY = (jugador.y - mapY) * deltaDY; }
    else            { stepY =  1; sideDistY = (mapY + 1.0f - jugador.y) * deltaDY; }

    bool ladoX    = false;
    int  maxPasos = (int)(DIST_MAX * 2);

    for (int i = 0; i < maxPasos; i++) {

        // Avanzar por el eje con interseccion mas proxima
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
        if (tipo <= 0) continue;

        // Distancia perpendicular (elimina efecto ojo de pez)
        float perpDist = ladoX ? (sideDistX - deltaDX) : (sideDistY - deltaDY);
        if (perpDist < 0.001f) perpDist = 0.001f;

        const TipoPared& tp = TIPOS_PARED[tipo];

        if (tp.sinColision) {
            // Registrar el primer arco; el rayo sigue adelante
            if (!res.hayArco) {
                res.hayArco       = true;
                res.distanciaArco = perpDist;
                res.tipoArco      = tipo;
                res.ladoXArco     = ladoX;
            }
            continue;
        }

        // Pared solida: detener el rayo
        res.golpeo    = true;
        res.tipoPared = tipo;
        res.ladoX     = ladoX;
        res.distancia = perpDist;
        break;
    }

    return res;
}

/*
    Renderiza un frame completo en tres pasos por columna:

      Paso 1 (Fondo)   : cielo con gradiente + piso con gradiente
      Paso 2 (Pared)   : pared solida o arbol encima del fondo
      Paso 3 (Overlay) : marco del arco encima de todo

    Despues superpone el minimapa y vuelca el buffer a la consola.
*/
void renderizarFrame(const Jugador& jugador, const Mapa& mapa) {
    const int mitad = ALTO_PANTALLA / 2;

    for (int x = 0; x < ANCHO_PANTALLA; x++) {

        float offset  = ((float)x / ANCHO_PANTALLA - 0.5f) * FOV;
        float angRayo = jugador.angulo + offset;

        RayoResultado rayo = lanzarRayo(jugador, mapa, angRayo);

        // ── PASO 1: Fondo (toda la columna) ──────────────────────

        for (int y = 0; y < ALTO_PANTALLA; y++) {
            if (y < mitad) {
                // Cielo: mas oscuro en el techo, aclara hacia el horizonte
                float t  = (float)y / (float)mitad;
                int   cl = (t < 0.20f) ? 1 : COLOR_CIELO;
                setCelda(x, y, CHAR_CIELO, cl);
            } else {
                // Piso: mas oscuro en el horizonte, aclara hacia los pies
                float t  = (float)(y - mitad) / (float)mitad;
                char  ch = (t > 0.40f) ? CHAR_PISO : ' ';
                int   cl = (t > 0.60f) ? 7 : COLOR_PISO;
                setCelda(x, y, ch, cl);
            }
        }

        // ── PASO 2: Pared solida o arbol ─────────────────────────

        if (rayo.golpeo) {
            const TipoPared& tp   = TIPOS_PARED[rayo.tipoPared];
            float            dist = rayo.distancia;

            switch (tp.comportamiento) {

                case ComportamientoPared::NORMAL: {
                    float altMuro = ((float)ALTO_PANTALLA / dist) * tp.altura;
                    int   inicio  = std::max(0, (int)(mitad - altMuro * 0.5f));
                    int   fin     = std::min(ALTO_PANTALLA - 1, (int)(mitad + altMuro * 0.5f));

                    char charMuro  = seleccionarCaracter(dist, tp);
                    int  colorMuro = seleccionarColor(dist, rayo.tipoPared, rayo.ladoX);

                    for (int y = inicio; y <= fin; y++)
                        setCelda(x, y, charMuro, colorMuro);
                    break;
                }

                case ComportamientoPared::ARBOL:
                    renderizarColumnaArbol(x, dist, rayo.tipoPared, rayo.ladoX);
                    break;

                default:
                    break;   // ARCO con golpeo solido: no deberia ocurrir
            }
        }

        // ── PASO 3: Overlay del arco (encima de fondo + pared) ───

        if (rayo.hayArco) {
            renderizarColumnaArco(x, rayo.distanciaArco, rayo.tipoArco, rayo.ladoXArco);
        }
    }

    dibujarMiniMapaEnBuffer(jugador, mapa);
    volcarBuffer();
}

/*
    Dibuja la barra de informacion (HUD) por debajo del area de juego.
    Escribe directamente en la consola fuera del buffer.
*/
void dibujarHUD(const Jugador& jugador, const Mapa& mapa) {
    gotoxy(0, ALTO_PANTALLA);
    color(8);
    for (int i = 0; i < ANCHO_PANTALLA; i++) std::cout.put('-');

    gotoxy(1,  ALTO_PANTALLA + 1); color(8);  std::cout << "WASD: Mover";
    gotoxy(14, ALTO_PANTALLA + 1); color(8);  std::cout << "Flechas: Girar";
    gotoxy(30, ALTO_PANTALLA + 1); color(8);  std::cout << "ESC: Salir";

    gotoxy(45, ALTO_PANTALLA + 1); color(14);
    std::cout << "[ " << mapa.nombre << " ]";

    char posStr[40];
    snprintf(posStr, sizeof(posStr), "X:%.1f  Y:%.1f", jugador.x, jugador.y);
    gotoxy(ANCHO_PANTALLA - 20, ALTO_PANTALLA + 1);
    color(11);
    std::cout << posStr;

    color(15);
}
