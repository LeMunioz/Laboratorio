#include "renderer.h"
#include "config.h"
#include "config_paredes.h"
#include "mapa.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <thread>
#include <mutex>
#include <queue>

/*
    Declaraciones externas de las funciones de la libreria frontend.
    Sus definiciones son enlazadas desde main.cpp al incluir frontend.cpp.
*/
extern void gotoxy(int x, int y);
extern void color(int c);

/*
================================================================
    renderer.cpp - Implementacion del renderizador por raycasting

    ARQUITECTURA MULTIHILO:
    -----------------------
    El sistema usa 3 hilos principales:
    
    Hilo 1 (RAYCASTER): 
      - Lanza rayos para cada columna de pantalla
      - Calcula distancias y tipos de pared
      - Genera RayoResultado para cada columna
      - Llena cola de columnas completadas
    
    Hilo 2 (DIBUJADOR):
      - Lee resultados del raycaster
      - Dibuja cielo, pared y suelo en el buffer
      - Aplica seleccion de color y trama segun distancia
      - Sincroniza con el raycaster mediante cola thread-safe
    
    Hilo 3 (MINIMAPA):
      - Actualmente en standby (puede hacer futuras funciones)
      - Dibuja el minimapa superpuesto sobre el buffer
      - Se sincroniza con el dibujador antes de volcar
    
    ALGORITMO DDA:
    -----------------------------------------------
    Para cada columna de la pantalla se lanza un rayo desde la
    posicion del jugador. El DDA avanza celda por celda del grid
    eligiendo siempre el eje (X o Y) cuya siguiente interseccion
    con la cuadricula este mas cerca.

    La distancia perpendicular elimina el efecto "ojo de pez".

    SISTEMA DE BUFFER:
    ------------------
    Se mantiene un buffer 2D principal que se llena por los hilos
    de raycasting y dibujado, y se vuelca fila por fila al final.

    TRAMAS PERSONALIZADAS:
    ----------------------
    Cada tipo de pared ahora puede tener una trama (string) que
    define como se ve a diferentes distancias. El sistema selecciona
    el caracter segun la distancia de forma inteligente.

    CIELO Y SUELO:
    ---------------
    El cielo y suelo ahora son configurables por mapa. Los patrones
    se dibujan solo en los extremos de la pantalla para crear
    profundidad visual (menos patron a mayor distancia).
================================================================
*/

// Buffer de pantalla: se llena cada frame antes de volcarse
static CeldaPantalla bufActual[ALTO_PANTALLA][ANCHO_PANTALLA];

// Variables de sincronizacion entre hilos
static std::mutex mutexBuffer;
static std::mutex mutexRayos;
static std::vector<ColumnaDatos> colasRayos[2];  // Double buffering para columnas
static int filaColasActual = 0;

// ----------------------------------------------------------------
// FUNCIONES AUXILIARES DEL BUFFER
// ----------------------------------------------------------------

/*
    Escribe un caracter y su color en la celda (x, y) del buffer.
    Las coordenadas fuera del rango se ignoran silenciosamente.
    Thread-safe: usa mutex para acceso concurrente.
*/
static void setCelda(int x, int y, char ch, int col) {
    if (y < 0 || y >= ALTO_PANTALLA || x < 0 || x >= ANCHO_PANTALLA) return;
    std::lock_guard<std::mutex> lock(mutexBuffer);
    bufActual[y][x] = { ch, col };
}

/*
    Version sin mutex para escrituras secuenciales conocidas
    (uso interno en funciones que ya tienen sincronizacion).
*/
static void setCeldaUnsafe(int x, int y, char ch, int col) {
    if (y < 0 || y >= ALTO_PANTALLA || x < 0 || x >= ANCHO_PANTALLA) return;
    bufActual[y][x] = { ch, col };
}

/*
    volvtea el buffer completo a la consola.
    Optimizacion: una sola llamada a gotoxy() por fila y las llamadas
    a color() se agrupan por bloques consecutivos del mismo color.
    Thread-safe: adquiere lock antes de leer.
*/
static void volcarBuffer() {
    std::lock_guard<std::mutex> lock(mutexBuffer);
    
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
    Retorna el caracter apropiado segun la distancia y la trama.
    Elige inteligentemente del string de trama basado en distancia.
*/
char seleccionarCaracterTramaDistancia(float dist, const std::string& trama) {
    if (trama.empty()) return ' ';
    
    // Seleccionar indice basado en distancia
    int idx = 0;
    if (dist < SOMBRA_MUY_CERCA) idx = 0;
    else if (dist < SOMBRA_CERCA) idx = 1 % trama.length();
    else if (dist < SOMBRA_MEDIA) idx = (2 % trama.length());
    else if (dist < SOMBRA_LEJOS) idx = (3 % trama.length());
    else idx = (4 % trama.length());
    
    // Asegurar que el indice es valido
    idx = idx % trama.length();
    return trama[idx];
}

/*
    Retorna el color de la pared considerando:
    - Distancia al jugador (cerca vs lejos)
    - Lado impactado: las caras horizontales se muestran
      un tono mas oscuro para simular iluminacion direccional
*/
int seleccionarColorMuro(float dist, int tipoPared, bool ladoX) {
    if (tipoPared >= (int)TIPOS_PARED.size()) return 8;
    
    const TipoPared& tp = TIPOS_PARED[tipoPared];
    int col = (dist < SOMBRA_CERCA) ? tp.colorCerca : tp.colorLejos;

    // Las caras horizontales (ladoX == false) son ligeramente mas oscuras
    if (!ladoX && col > 1) col--;

    return col;
}

// ----------------------------------------------------------------
// FUNCIONES DE CIELO Y SUELO
// ----------------------------------------------------------------

/*
    Dibuja un caracter de cielo, aplicando patron solo en los extremos.
    Los patrones se desvanecen hacia el centro para crear efecto de profundidad.
*/
void dibujarCielo(int x, int y, int idCielo, int distanciaPantalla) {
    if (idCielo < 0 || idCielo >= (int)TIPOS_CIELO.size()) {
        setCelda(x, y, ' ', 8);
        return;
    }
    
    const TipoCielo& c = TIPOS_CIELO[idCielo];
    
    // Dibujar patron solo si estamos cerca de los bordes
    char ch = ' ';
    if (!c.trama.empty() && distanciaPantalla > 15) {  // Solo en los extremos
        int idx = (x + y) % c.trama.length();
        ch = c.trama[idx];
    }
    
    setCelda(x, y, ch, c.color);
}

/*
    Dibuja un caracter de suelo, aplicando patron solo en los extremos.
    Los patrones se desvanecen hacia el horizonte para crear profundidad.
*/
void dibujarSuelo(int x, int y, int idSuelo, int distanciaPantalla) {
    if (idSuelo < 0 || idSuelo >= (int)TIPOS_SUELO.size()) {
        setCelda(x, y, '.', 8);
        return;
    }
    
    const TipoSuelo& s = TIPOS_SUELO[idSuelo];
    
    // Dibujar patron solo si estamos cerca del borde inferior
    char ch = ' ';
    if (!s.trama.empty() && distanciaPantalla > 10) {  // Solo en los extremos inferiores
        int idx = (x + y) % s.trama.length();
        ch = s.trama[idx];
    }
    
    setCelda(x, y, ch, s.color);
}

// ----------------------------------------------------------------
// MINIMAPA EN BUFFER
// ----------------------------------------------------------------

/*
    Escribe el minimapa 2D directamente en el buffer de pantalla,
    superpuesto en la esquina superior derecha de la escena.
    Al estar integrado en el buffer no produce parpadeo.  <---- NECESITA ARREGLO :(
    Esta funcion se ejecuta en el hilo del minimapa (Hilo 3).
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
                if (tipo < (int)TIPOS_PARED.size()) {
                    cl = TIPOS_PARED[tipo].colorCerca;
                } else {
                    cl = 8;
                }
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

    std::lock_guard<std::mutex> lock(mutexBuffer);
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
    HILO 1: Realiza raycast en paralelo para multiples columnas.
    Calcula RayoResultado para cada columna y los almacena.
*/
static void hilRaycaster(const Jugador& jugador, const Mapa& mapa, 
                         int columnaInicio, int columnaFin) {
    const int mitad = ALTO_PANTALLA / 2;

    for (int x = columnaInicio; x < columnaFin; x++) {
        float offset  = ((float)x / ANCHO_PANTALLA - 0.5f) * FOV;
        float angRayo = jugador.angulo + offset;

        RayoResultado rayo = lanzarRayo(jugador, mapa, angRayo);

        // Calcular altura de la pared en pantalla
        float multAltura = (rayo.golpeo) ? TIPOS_PARED[rayo.tipoPared].altura : 1.0f;
        float dist       = (rayo.golpeo) ? rayo.distancia : DIST_MAX;
        float altMuro    = ((float)ALTO_PANTALLA / dist) * multAltura;

        ColumnaDatos col;
        col.x = x;
        col.rayo = rayo;
        col.altMuro = altMuro;
        col.buffer.resize(ALTO_PANTALLA);
        col.completa = false;

        {
            std::lock_guard<std::mutex> lock(mutexRayos);
            colasRayos[filaColasActual].push_back(col);
        }
    }
}

/*
    HILO 2: Dibuja las columnas calculadas por el raycaster.
    Lee la cola de rayos y dibuja cielo, muro y piso en el buffer.
*/
static void hilDibujador(const Mapa& mapa) {
    const int mitad = ALTO_PANTALLA / 2;
    
    // Leer todas las columnas calculadas
    std::vector<ColumnaDatos> columnasLocal;
    
    {
        std::lock_guard<std::mutex> lock(mutexRayos);
        columnasLocal = colasRayos[filaColasActual];
    }

    // Procesar cada columna
    for (const auto& col : columnasLocal) {
        int x = col.x;
        const RayoResultado& rayo = col.rayo;
        float altMuro = col.altMuro;

        int inicio = (int)(mitad - altMuro * 0.5f);
        int fin    = (int)(mitad + altMuro * 0.5f);
        inicio     = std::max(0,               inicio);
        fin        = std::min(ALTO_PANTALLA - 1, fin);

        // Apariencia de la pared segun tipo y distancia
        char charMuro  = ' ';
        int  colorMuro = 8;

        if (rayo.golpeo) {
            float dist = rayo.distancia;
            charMuro  = seleccionarCaracterTramaDistancia(dist, TIPOS_PARED[rayo.tipoPared].trama);
            colorMuro = seleccionarColorMuro(dist, rayo.tipoPared, rayo.ladoX);
        }

        // -- Dibujar columna completa en el buffer --

        for (int y = 0; y < ALTO_PANTALLA; y++) {

            if (y < inicio) {
                // CIELO: gradiente descendente
                float t  = (float)y / (float)mitad;
                int distancia = mitad - y;
                dibujarCielo(x, y, mapa.idCielo, distancia);

            } else if (y <= fin) {
                // PARED
                setCelda(x, y, charMuro, colorMuro);

            } else {
                // PISO: gradiente ascendente
                float t  = (float)(y - mitad) / (float)mitad;
                int distancia = y - mitad;
                dibujarSuelo(x, y, mapa.idSuelo, distancia);
            }
        }
    }
}

/*
    HILO 3: Dibuja el minimapa (actualmente en operacion simple).
    En el futuro puede extenderse para otras funciones del HUD.
*/
static void hilMinimapa(const Jugador& jugador, const Mapa& mapa) {
    dibujarMiniMapaEnBuffer(jugador, mapa);
}

/*
    Renderiza la escena 3D completa usando paralelismo:
    - Hilo 1: Raycasting en paralelo
    - Hilo 2: Dibujado en paralelo
    - Hilo 3: Minimapa
*/
void renderizarFrame(const Jugador& jugador, const Mapa& mapa) {
    if (!USAR_MULTIHILO) {
        // Version secuencial (fallback)
        const int mitad = ALTO_PANTALLA / 2;

        for (int x = 0; x < ANCHO_PANTALLA; x++) {
            float offset  = ((float)x / ANCHO_PANTALLA - 0.5f) * FOV;
            float angRayo = jugador.angulo + offset;
            RayoResultado rayo = lanzarRayo(jugador, mapa, angRayo);

            float multAltura = (rayo.golpeo) ? TIPOS_PARED[rayo.tipoPared].altura : 1.0f;
            float dist       = (rayo.golpeo) ? rayo.distancia : DIST_MAX;
            float altMuro    = ((float)ALTO_PANTALLA / dist) * multAltura;

            int inicio = (int)(mitad - altMuro * 0.5f);
            int fin    = (int)(mitad + altMuro * 0.5f);
            inicio     = std::max(0,               inicio);
            fin        = std::min(ALTO_PANTALLA - 1, fin);

            char charMuro  = ' ';
            int  colorMuro = 8;

            if (rayo.golpeo) {
                charMuro  = seleccionarCaracterTramaDistancia(dist, TIPOS_PARED[rayo.tipoPared].trama);
                colorMuro = seleccionarColorMuro(dist, rayo.tipoPared, rayo.ladoX);
            }

            for (int y = 0; y < ALTO_PANTALLA; y++) {
                if (y < inicio) {
                    float t  = (float)y / (float)mitad;
                    int distancia = mitad - y;
                    dibujarCielo(x, y, mapa.idCielo, distancia);
                } else if (y <= fin) {
                    setCelda(x, y, charMuro, colorMuro);
                } else {
                    float t  = (float)(y - mitad) / (float)mitad;
                    int distancia = y - mitad;
                    dibujarSuelo(x, y, mapa.idSuelo, distancia);
                }
            }
        }

        dibujarMiniMapaEnBuffer(jugador, mapa);
        volcarBuffer();
        return;
    }

    // Version paralela con 3 hilos
    
    // Limpiar colas
    {
        std::lock_guard<std::mutex> lock(mutexRayos);
        colasRayos[filaColasActual].clear();
    }

    // Hilo 1: Raycasting en paralelo (dividir trabajo)
    int columnasPerThread = (ANCHO_PANTALLA + NUM_HILOS_RAYCASTER - 1) / NUM_HILOS_RAYCASTER;
    std::vector<std::thread> hilosRaycast;
    
    for (int i = 0; i < NUM_HILOS_RAYCASTER; i++) {
        int inicio = i * columnasPerThread;
        int fin = std::min(inicio + columnasPerThread, ANCHO_PANTALLA);
        if (inicio < ANCHO_PANTALLA) {
            hilosRaycast.emplace_back(hilRaycaster, std::ref(jugador), std::ref(mapa), inicio, fin);
        }
    }

    // Esperar a que terminen todos los raycast threads
    for (auto& t : hilosRaycast) {
        t.join();
    }

    // Hilo 2: Dibujado
    std::thread hiloDibujador(hilDibujador, std::ref(mapa));

    // Hilo 3: Minimapa
    std::thread hiloMinimap(hilMinimapa, std::ref(jugador), std::ref(mapa));

    // Esperar a que terminen dibujado y minimapa
    hiloDibujador.join();
    hiloMinimap.join();

    // Volcar buffer a pantalla (operacion secuencial de escritura)
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
