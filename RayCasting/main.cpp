#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

// Libreria de la consola para color y posicionamiento
#include "../Frontend/frontend.cpp"

// Modulos del motor
#include "config.h"
#include "tipos.h"
#include "config_paredes.h"
#include "mapa.h"
#include "jugador.h"
#include "renderer.h"

// Implementaciones de los modulos
#include "config_paredes.cpp"
#include "mapa.cpp"
#include "jugador.cpp"
#include "renderer.cpp"

/*
================================================================
    main.cpp - Punto de entrada y bucle principal del juego

    Estructura del programa:
      1. Configurar la consola (tamano, titulo, codepage)
      2. Cargar todos los mapas desde el archivo externo
      3. Inicializar la pantalla y el jugador
      4. Ejecutar el bucle principal del juego:
            a. Procesar entrada del teclado
            b. Renderizar la escena 3D
            c. Dibujar el HUD informativo
      5. Mostrar pantalla de bienvenida entre mapas
      6. Restaurar la consola al salir

    Para agregar nuevos mapas: edita mapas.txt
    Para cambiar la apariencia de paredes: edita config_paredes.cpp
    Para ajustar velocidades y FOV: edita config.h
================================================================
*/

// -- Ruta al archivo de mapas (relativa al ejecutable) --
static const std::string RUTA_MAPAS = "mapas.txt";

/*
    Configura la ventana de la consola de Windows:
    - Titulo de la ventana
    - Tamano del buffer y de la ventana visible
    - Codepage UTF-8 para compatibilidad de caracteres
    - Deshabilita el modo de edicion para no bloquear el input
*/
static void configurarConsola() {
    SetConsoleTitle("Motor Raycasting 3D - Consola");

    // Ajustar el tamano del buffer de la consola
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT ventana = { 0, 0,
        (short)(ANCHO_PANTALLA - 1),
        (short)(ALTO_PANTALLA + 3) };
    COORD tamBuffer = { (short)ANCHO_PANTALLA, (short)(ALTO_PANTALLA + 4) };

    SetConsoleWindowInfo(hCon, TRUE, &ventana);
    SetConsoleScreenBufferSize(hCon, tamBuffer);
    SetConsoleWindowInfo(hCon, TRUE, &ventana);

    // Deshabilitar el modo de edicion rapida para evitar pausas al hacer clic
    DWORD modo;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hIn, &modo);
    SetConsoleMode(hIn, modo & ~ENABLE_QUICK_EDIT_MODE);

    SetConsoleOutputCP(65001);   // UTF-8
}

/*
    Muestra una pantalla de presentacion antes de iniciar un mapa.
    Espera a que el jugador presione ENTER para continuar.
    Retorna false si el jugador presiona ESC (salir al menu).
*/
static bool pantallaBienvenida(const Mapa& mapa, int indice, int total) {
    system("cls");
    color(14);
    gotoxy(2, 2);
    std::cout << "================================================";
    gotoxy(2, 3);
    std::cout << "   MOTOR RAYCASTING 3D  -  CONSOLA";
    gotoxy(2, 4);
    std::cout << "================================================";

    color(11);
    gotoxy(2, 6);
    std::cout << "  Mapa " << (indice + 1) << " de " << total
              << " : " << mapa.nombre;

    color(7);
    gotoxy(2, 8);
    std::cout << "  CONTROLES:";
    gotoxy(4, 9);
    std::cout << "W / S         -> Avanzar / Retroceder";
    gotoxy(4, 10);
    std::cout << "A / D         -> Desplazamiento lateral";
    gotoxy(4, 11);
    std::cout << "Flechas </>   -> Girar la camara";
    gotoxy(4, 12);
    std::cout << "ESC           -> Salir al siguiente mapa";

    color(14);
    gotoxy(2, 14);
    std::cout << "  Presiona ENTER para comenzar  (ESC = saltar)...";
    color(15);

    // Esperar ENTER o ESC
    while (true) {
        int c = _getch();
        if (c == '\r' || c == '\n') return true;
        if (c == 27)               return false;
    }
}

/*
    Ejecuta el bucle principal de juego para un mapa concreto.
    Retorna cuando el jugador presiona ESC.
*/
static void bucleJuego(const Mapa& mapa) {
    Jugador jugador = crearJugador(mapa);
    inicializarPantalla();

    bool corriendo = true;

    while (corriendo) {
        corriendo = procesarEntrada(jugador, mapa);
        renderizarFrame(jugador, mapa);
        dibujarHUD(jugador, mapa);
    }
}

/*
    Muestra un mensaje de error cuando no se pueden cargar los mapas.
*/
static void mostrarErrorMapas() {
    system("cls");
    color(12);
    gotoxy(2, 2);
    std::cout << "ERROR: No se pudo cargar el archivo '" << RUTA_MAPAS << "'.";
    color(7);
    gotoxy(2, 4);
    std::cout << "Asegurate de que el archivo existe en el mismo directorio";
    gotoxy(2, 5);
    std::cout << "que el ejecutable y tiene el formato correcto.";
    gotoxy(2, 7);
    color(14);
    std::cout << "Presiona cualquier tecla para salir...";
    color(15);
    _getch();
}

/*
    Restaura la configuracion de la consola al estado normal.
    Se llama siempre al final, incluso si ocurre un error.
*/
static void restaurarConsola() {
    system("cls");
    color(15);
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci = { 25, TRUE };
    SetConsoleCursorInfo(hCon, &ci);
    gotoxy(0, 0);
}

// ----------------------------------------------------------------
// PUNTO DE ENTRADA
// ----------------------------------------------------------------

int main() {
    configurarConsola();

    // Cargar todos los mapas desde el archivo externo
    std::vector<Mapa> mapas = cargarMapas(RUTA_MAPAS);

    if (mapas.empty()) {
        mostrarErrorMapas();
        restaurarConsola();
        return 1;
    }

    // Recorrer los mapas en orden
    int total = (int)mapas.size();

    for (int i = 0; i < total; i++) {
        // Mostrar la pantalla de bienvenida del mapa actual
        bool continuar = pantallaBienvenida(mapas[i], i, total);
        if (!continuar) continue;   // El jugador saltó este mapa con ESC

        // Ejecutar el bucle de juego para este mapa
        bucleJuego(mapas[i]);
    }

    // Pantalla de fin al recorrer todos los mapas
    system("cls");
    color(14);
    gotoxy(2, 2);
    std::cout << "  Fin del recorrido. Gracias por jugar!";
    color(15);
    gotoxy(2, 4);
    std::cout << "  Presiona cualquier tecla para salir...";
    _getch();

    restaurarConsola();
    return 0;
}
