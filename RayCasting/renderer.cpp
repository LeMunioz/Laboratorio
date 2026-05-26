#include "renderer.h"
#include "config.h"
#include "config_paredes.h"
#include "mapa.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <windows.h>

extern void gotoxy(int x, int y);
extern void color(int c);

/*
================================================================
    renderer.cpp - Renderizador 3D por raycasting (DDA)

    PIPELINE POR COLUMNA (3 pasos):
    ─────────────────────────────────
    1. FONDO    : cielo + piso para toda la columna
    2. PARED    : pared solida o arbol encima del fondo
    3. OVERLAY  : marco del arco encima de lo anterior

    ARBOL - tronco delgado via wallX:
    ──────────────────────────────────
    El DDA calcula wallX [0.0, 1.0): posicion de impacto dentro
    de la celda. El tronco solo se dibuja si wallX cae dentro de
    la franja central definida por ARBOL_ANCHO_TRONCO.
    Los rayos fuera de esa franja solo dibujan follaje,
    dando la apariencia de un tronco delgado con copa ancha.

    ARBOL - sin superposicion con paredes cercanas:
    ─────────────────────────────────────────────────
    troncFin (base del tronco) se calcula con altBase (altura=1.0)
    para que coincida exactamente con la linea de suelo de esa
    distancia. Esto garantiza que el tronco no "flote" sobre el piso
    de paredes mas cercanas que se ven en columnas adyacentes.

    ARCO - marco que nunca tapa el horizonte:
    ──────────────────────────────────────────
    El borde inferior del marco del arco se clampea a
    (mitad - ARC_MARGEN_OJO), asegurando que el hueco
    del arco siempre sea visible desde el nivel del ojo
    del jugador, independiente de la distancia.
================================================================
*/

static CeldaPantalla bufActual[ALTO_PANTALLA][ANCHO_PANTALLA];

// ────────────────────────────────────────────────────────────────
// FUNCIONES DEL BUFFER
// ────────────────────────────────────────────────────────────────

static void setCelda(int x, int y, char ch, int col) {
    if (y < 0 || y >= ALTO_PANTALLA || x < 0 || x >= ANCHO_PANTALLA) return;
    bufActual[y][x] = { ch, col };
}

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
    Elige el caracter de la pared segun distancia al jugador.
*/
static char seleccionarCaracter(float dist, const TipoPared& tp) {
    return (dist < DIST_CHAR) ? tp.tramaCerca : tp.tramaLejos;
}

/*
    Elige el color de la pared considerando distancia y cara.
    Las caras horizontales siempre reciben colorSombra.
*/
static int seleccionarColor(float dist, int tipoPared, bool ladoX) {
    const TipoPared& tp = TIPOS_PARED[tipoPared];
    if (!ladoX)           return tp.colorSombra;
    if (dist < DIST_CERCA) return tp.colorCerca;
    if (dist < DIST_LEJOS) return tp.colorLejos;
    return tp.colorMuyLejos;
}

// ────────────────────────────────────────────────────────────────
// RENDERIZADO DE TIPOS ESPECIALES
// ────────────────────────────────────────────────────────────────

/*
    Renderiza una columna de arbol.

    Estructura del arbol (Y crece hacia abajo):
    ─────────────────────────────────────────────
        [cielo visible]
        ┌─────────────────┐  follIni
        │   FOLLAJE ####  │
        └─────────────────┘  follFin = troncIni + overlap
        [espacio visible]     (si hay hueco entre follaje y tronco)
        ┌──────┐              troncIni  ← solo rayos centrales (wallX)
        │TRONCO│
        └──────┘              troncFin = mitad + altBase/2  ← suelo exacto
        [piso]

    El tronco solo se dibuja si wallX esta en la franja central
    [0.5 - ARBOL_ANCHO_TRONCO/2, 0.5 + ARBOL_ANCHO_TRONCO/2].
    Los rayos fuera de esa franja solo dibujan follaje.

    altBase usa dist directamente (sin multiplicador de altura)
    para que el suelo del tronco coincida con la linea de suelo
    geometrica de esa distancia, evitando que "flote".
*/
static void renderizarColumnaArbol(int x, float dist, int tipoPared,
                                   bool ladoX, float wallX) {
    const TipoPared& tp    = TIPOS_PARED[tipoPared];
    const int        mitad = ALTO_PANTALLA / 2;

    // altBase: altura que tendria una pared normal a esta distancia
    float altBase = (float)ALTO_PANTALLA / dist;

    // -- Zona del tronco (coordenadas Y en pantalla) --
    int troncFin = std::min((int)(mitad + altBase * 0.5f), ALTO_PANTALLA - 1);
    int troncIni = std::max((int)(troncFin - altBase * ARBOL_FRAC_TRONCO), 0);

    // -- Zona del follaje --
    float altFollaje = altBase * tp.altura;
    int   follFin    = std::min((int)(troncIni + altBase * ARBOL_FRAC_OVERLAP), ALTO_PANTALLA - 1);
    int   follIni    = std::max((int)(follFin  - altFollaje), 0);

    int  colorFollaje = tp.colorLejos;   // Verde del follaje
    char charFollaje  = tp.tramaLejos;   // '#'

    // -- Determinar si este rayo golpeo la zona del tronco --
    float troncMin = 0.5f - ARBOL_ANCHO_TRONCO * 0.5f;
    float troncMax = 0.5f + ARBOL_ANCHO_TRONCO * 0.5f;
    bool  esTronco = (wallX >= troncMin && wallX <= troncMax);

    // Color del tronco respeta sombra y distancia
    int colorTronco = !ladoX
        ? tp.colorSombra
        : (dist < DIST_CERCA ? tp.colorCerca : tp.colorMuyLejos);
    char charTronco = tp.tramaCerca;   // '|'

    // -- Dibujar follaje (ancho completo) --
    for (int y = follIni; y <= follFin; y++)
        setCelda(x, y, charFollaje, colorFollaje);

    // -- Dibujar tronco solo en la franja central --
    if (esTronco) {
        for (int y = troncIni; y <= troncFin; y++)
            setCelda(x, y, charTronco, colorTronco);
    }
}

/*
    Renderiza el marco superior de un arco como overlay.

    Regla de clamping:
    ──────────────────
    El borde inferior del marco (marcoFin) nunca supera
    (mitad - ARC_MARGEN_OJO). Esto garantiza que el hueco
    del arco siempre sea visible desde el nivel del ojo,
    sin importar cuan cerca este el jugador del arco.

    Si el marco calculado quedaria por debajo del horizonte,
    se recorta hacia arriba. Si el marco es tan pequeno que
    ya no hay pixeles que dibujar, simplemente no se dibuja.
*/
static void renderizarColumnaArco(int x, float dist, int tipoPared, bool ladoX) {
    const TipoPared& tp    = TIPOS_PARED[tipoPared];
    const int        mitad = ALTO_PANTALLA / 2;

    float altTotal = ((float)ALTO_PANTALLA / dist) * tp.altura;

    int arcoIni = std::max(0, (int)(mitad - altTotal * 0.5f));

    // Limite inferior del marco: nunca pasar del horizonte
    int limiteInferiorMarco = mitad - ARC_MARGEN_OJO;

    // Alto del marco superior como fraccion del total
    // (30% de la pared, pero nunca mas alla del limite del ojo)
    int marcoFin = (int)(arcoIni + altTotal * 0.30f);
    marcoFin     = std::min(marcoFin, limiteInferiorMarco);
    marcoFin     = std::min(marcoFin, ALTO_PANTALLA - 1);

    // Si el arco esta tan cerca que arcoIni ya paso el limite, no dibujar
    if (arcoIni > limiteInferiorMarco) return;

    char charArco  = seleccionarCaracter(dist, tp);
    int  colorArco = seleccionarColor(dist, tipoPared, ladoX);

    for (int y = arcoIni; y <= marcoFin; y++)
        setCelda(x, y, charArco, colorArco);
}

// ────────────────────────────────────────────────────────────────
// MINIMAPA
// ────────────────────────────────────────────────────────────────

static void dibujarMiniMapaEnBuffer(const Jugador& jugador, const Mapa& mapa) {
    const int posX = ANCHO_PANTALLA - ANCHO_MINI - 1;
    const int posY = 0;

    int camX = (int)jugador.x - ANCHO_MINI / 2;
    int camY = (int)jugador.y - ALTO_MINI  / 2;
    camX = std::max(0, std::min(camX, mapa.ancho - ANCHO_MINI));
    camY = std::max(0, std::min(camY, mapa.alto  - ALTO_MINI));

    int jMapX = (int)jugador.x - camX;
    int jMapY = (int)jugador.y - camY;

    for (int my = 0; my < ALTO_MINI; my++) {
        for (int mx = 0; mx < ANCHO_MINI; mx++) {
            int tipo = obtenerTipoCelda(mapa, camX + mx, camY + my);
            char ch; int cl;

            if (mx == jMapX && my == jMapY) {
                ch = '@'; cl = 14;
            } else if (tipo <= 0) {
                ch = ' '; cl = 8;
            } else {
                const TipoPared& tp = TIPOS_PARED[tipo];
                switch (tp.comportamiento) {
                    case ComportamientoPared::ARBOL: ch = 'T'; cl = tp.colorLejos;  break;
                    case ComportamientoPared::ARCO:  ch = ':'; cl = tp.colorCerca;  break;
                    default:                         ch = '#'; cl = tp.colorCerca;  break;
                }
            }

            setCelda(posX + mx, posY + my, ch, cl);
        }
    }
}

// ────────────────────────────────────────────────────────────────
// FUNCIONES PUBLICAS
// ────────────────────────────────────────────────────────────────

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
    Algoritmo DDA.

    Para cada celda impactada:
      - Si es vacia: continuar
      - Si sinColision (arco): registrar primer arco, continuar
      - Si solida: registrar y detener

    Calculo de wallX:
    ─────────────────
    Una vez conocida la distancia perpendicular (perpDist),
    se reconstruye el punto exacto de impacto en el mundo:
      impactoY = jugador.y + perpDist * rayDY   (para lado X)
      impactoX = jugador.x + perpDist * rayDX   (para lado Y)
    Se toma la parte fraccionaria para obtener [0.0, 1.0).
*/
RayoResultado lanzarRayo(const Jugador& jugador, const Mapa& mapa, float angulo) {
    RayoResultado res = {};
    res.distancia     = DIST_MAX;
    res.golpeo        = false;
    res.wallX         = 0.0f;
    res.distanciaArco = DIST_MAX;
    res.hayArco       = false;
    res.wallXArco     = 0.0f;

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
        if (sideDistX < sideDistY) {
            sideDistX += deltaDX; mapX += stepX; ladoX = true;
        } else {
            sideDistY += deltaDY; mapY += stepY; ladoX = false;
        }

        int tipo = obtenerTipoCelda(mapa, mapX, mapY);
        if (tipo <= 0) continue;

        float perpDist = ladoX ? (sideDistX - deltaDX) : (sideDistY - deltaDY);
        if (perpDist < 0.001f) perpDist = 0.001f;

        // Calcular wallX: fraccion del punto de impacto dentro de la celda
        float impacto = ladoX
            ? (jugador.y + perpDist * rayDY)
            : (jugador.x + perpDist * rayDX);
        float wx = impacto - std::floor(impacto);

        const TipoPared& tp = TIPOS_PARED[tipo];

        if (tp.sinColision) {
            if (!res.hayArco) {
                res.hayArco       = true;
                res.distanciaArco = perpDist;
                res.tipoArco      = tipo;
                res.ladoXArco     = ladoX;
                res.wallXArco     = wx;
            }
            continue;
        }

        res.golpeo    = true;
        res.tipoPared = tipo;
        res.ladoX     = ladoX;
        res.distancia = perpDist;
        res.wallX     = wx;
        break;
    }

    return res;
}

/*
    Renderiza un frame completo en 3 pasos por columna:
      1. Fondo (cielo + piso)
      2. Pared solida o arbol
      3. Overlay del arco
*/
void renderizarFrame(const Jugador& jugador, const Mapa& mapa) {
    const int mitad = ALTO_PANTALLA / 2;

    for (int x = 0; x < ANCHO_PANTALLA; x++) {
        float offset  = ((float)x / ANCHO_PANTALLA - 0.5f) * FOV;
        float angRayo = jugador.angulo + offset;

        RayoResultado rayo = lanzarRayo(jugador, mapa, angRayo);

        // ── PASO 1: Fondo ─────────────────────────────────────────
        for (int y = 0; y < ALTO_PANTALLA; y++) {
            if (y < mitad) {
                float t  = (float)y / (float)mitad;
                int   cl = (t < 0.20f) ? 1 : COLOR_CIELO;
                setCelda(x, y, CHAR_CIELO, cl);
            } else {
                float t  = (float)(y - mitad) / (float)mitad;
                char  ch = (t > 0.40f) ? CHAR_PISO : ' ';
                int   cl = (t > 0.60f) ? 7 : COLOR_PISO;
                setCelda(x, y, ch, cl);
            }
        }

        // ── PASO 2: Pared solida o arbol ──────────────────────────
        if (rayo.golpeo) {
            const TipoPared& tp   = TIPOS_PARED[rayo.tipoPared];
            float            dist = rayo.distancia;

            switch (tp.comportamiento) {
                case ComportamientoPared::NORMAL: {
                    float altMuro = ((float)ALTO_PANTALLA / dist) * tp.altura;
                    int   inicio  = std::max(0, (int)(mitad - altMuro * 0.5f));
                    int   fin     = std::min(ALTO_PANTALLA - 1, (int)(mitad + altMuro * 0.5f));
                    char  ch      = seleccionarCaracter(dist, tp);
                    int   cl      = seleccionarColor(dist, rayo.tipoPared, rayo.ladoX);
                    for (int y = inicio; y <= fin; y++) setCelda(x, y, ch, cl);
                    break;
                }
                case ComportamientoPared::ARBOL:
                    renderizarColumnaArbol(x, dist, rayo.tipoPared, rayo.ladoX, rayo.wallX);
                    break;
                default:
                    break;
            }
        }

        // ── PASO 3: Overlay del arco ───────────────────────────────
        if (rayo.hayArco) {
            renderizarColumnaArco(x, rayo.distanciaArco, rayo.tipoArco, rayo.ladoXArco);
        }
    }

    dibujarMiniMapaEnBuffer(jugador, mapa);
    volcarBuffer();
}

void dibujarHUD(const Jugador& jugador, const Mapa& mapa) {
    gotoxy(0, ALTO_PANTALLA);
    color(8);
    for (int i = 0; i < ANCHO_PANTALLA; i++) std::cout.put('-');

    gotoxy(1,  ALTO_PANTALLA + 1); color(8);  std::cout << "WASD: Mover";
    gotoxy(14, ALTO_PANTALLA + 1); color(8);  std::cout << "Flechas: Girar";
    gotoxy(30, ALTO_PANTALLA + 1); color(8);  std::cout << "ESC: Salir";
    gotoxy(45, ALTO_PANTALLA + 1); color(14); std::cout << "[ " << mapa.nombre << " ]";

    char posStr[40];
    snprintf(posStr, sizeof(posStr), "X:%.1f  Y:%.1f", jugador.x, jugador.y);
    gotoxy(ANCHO_PANTALLA - 20, ALTO_PANTALLA + 1);
    color(11);
    std::cout << posStr;
    color(15);
}
