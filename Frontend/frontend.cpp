#include <iostream>
#include <vector>
#include <cctype>
#include <string>
#include <conio.h>
#include <windows.h>
using namespace std;

/*
+------------------------------------------------------------------+
¦         LIBRERIA DE FRONT-END PARA C++                          ¦
¦         Hecho por: Angel Eduardo Muñoz Perez                    ¦
¦                  & Javier Garnica Escamilla                     ¦
+------------------------------------------------------------------+

    FUNCIONES:
        1) Imprimir en colores en consola  - - - - -  1
        2) Imprimir con coordenadas        - - - - -  2
        3) Validación de entrada de datos  - - - - -  3
        4) Menús interactivos              - - - - -  4
        5) Botones y cajas interactivos    - - - - -  5
        6) Tamaño de consola               - - - - -  6
        7) Animación de textos             - - - - -  7


///////////////////////////////////////////////////////////////////
1) IMPRIMIR EN COLORES EN CONSOLA
///////////////////////////////////////////////////////////////////
    Todo lo que se imprima despues cambiara de color segun el valor.
    COLORES DISPONIBLES:
        [1]  AZUL REY     [2]  VERDE        [3]  AZUL CYAN
        [4]  ROJO         [5]  VIOLETA      [6]  AMARILLO OCRE
        [7]  GRIS CLARO   [8]  GRIS         [9]  AZUL
        [10] VERDE PASTO  [11] CELESTE      [12] ROJO CLARO
        [13] MORADO       [14] CREMA        [15] BLANCO
        [16++] FONDO DE COLOR

    Sintaxis:
        color(12);
        cout << "Hola en rojo claro";


///////////////////////////////////////////////////////////////////
2) IMPRIMIR CON COORDENADAS EN CONSOLA
///////////////////////////////////////////////////////////////////
    Mueve el cursor antes de imprimir.
        gotoxy(x, y)  ?  columna x, renglón y
        gotox(x)      ?  solo columna x, renglón actual

    Sintaxis:
        gotoxy(10, 5);
        cout << "Texto en 10,5";


///////////////////////////////////////////////////////////////////
3) VALIDACIÓN DE ENTRADA DE DATOS
///////////////////////////////////////////////////////////////////
    Validan lo que escribe el usuario evitando errores.

    ENTEROS:
        int n = leerOpcion(minimo, maximo);

    CARACTERES:
        char c = leerCharOpcional({'s', 'n', 'a'});
        // Solo acepta los caracteres del vector (mayús. también)


///////////////////////////////////////////////////////////////////
4) MENÚS INTERACTIVOS
///////////////////////////////////////////////////////////////////
    Menús navegables con flechas y ENTER.

    TIPOS DE MENÚ  (MenuTipo::):
        Vertical    ?  opciones apiladas de arriba a abajo
        Horizontal  ?  opciones en una sola línea
        Auto        ?  elige automáticamente según el ancho disponible
        Matriz      ?  cuadrícula 2D, navega en las 4 direcciones

    SINTAXIS BÁSICA ----------------------------------------------

        Menu miMenu({"Op 1", "Op 2", "Op 3"}, MenuTipo::Vertical);
        int elegido = miMenu.mostrar();

    PARAMETROS DE ESTILO (opcionales, se pasan a mostrar()) ------

        miMenu.mostrar(colorSeleccion, mostrarIndicador)
            colorSeleccion  ?  número de color (1-15) del ítem activo
                               por defecto: 2 (verde)
            mostrarIndicador?  true  muestra "[>] Opción"
                               false muestra solo "Opción"
                               por defecto: true

    POSICION MANUAL ----------------------------------------------
        Si no se llama a setPos(), el menu se dibuja donde esté el
        cursor en ese momento.

            miMenu.setPos(x, y);
            miMenu.mostrar();

    MENU MATRIZ --------------------------------------------------
        Para crear una cuadricula, pasa un vector de vectores y el
        número de columnas. Se navega con las 4 flechas.
        Retorna {fila, columna} de la celda seleccionada.

            MenuMatriz mm({
                {"[ ]", "[ ]", "[ ]"},
                {"[ ]", "[ ]", "[ ]"}
            });
            pair<int,int> pos = mm.mostrar();
            int fila    = pos.first;
            int columna = pos.second;

        Tambien acepta setPos(), color e indicador:
            mm.setPos(x, y);
            mm.mostrar(colorSeleccion, mostrarIndicador);


///////////////////////////////////////////////////////////////////
5) BOTONES Y CAJAS INTERACTIVOS
///////////////////////////////////////////////////////////////////
    drawBox(x, y, ancho, alto)    ?  dibuja un rectángulo con bordes
    drawButton(x, y, texto, activo) ?  botón con estado activo/inactivo


///////////////////////////////////////////////////////////////////
6) TAMAÑO DE CONSOLA
///////////////////////////////////////////////////////////////////
    setConsoleSize(columnas, renglones);


///////////////////////////////////////////////////////////////////
7) ANIMACIÓN DE TEXTOS
///////////////////////////////////////////////////////////////////
    escribirAnimado(x, y, texto, delayMs)  ?  efecto máquina de escribir
    textoFade(x, y, texto)                 ?  aparece de gris a blanco
*/


// -------------------------------------------------- //
//  SECCIÓN 2 — COORDENADAS
// -------------------------------------------------- //

void gotoxy(int x, int y){
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hcon, dwPos);
}

void gotox(int x){
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hcon, &csbi);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = csbi.dwCursorPosition.Y;
    SetConsoleCursorPosition(hcon, dwPos);
}


// -------------------------------------------------- //
//  SECCIÓN 1 — COLORES
// -------------------------------------------------- //

void color(int c){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, c);
}


// -------------------------------------------------- //
//  SECCIÓN 3 — VALIDACIÓN DE ENTRADAS
// -------------------------------------------------- //

int leerOpcion(int min, int max) {
    int opcion;
    while (true) {
        cin >> opcion;
        if (!cin.fail() && opcion >= min && opcion <= max) break;
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Entrada inválida. Intenta nuevamente: ";
    }
    return opcion;
}

char leerCharOpcional(vector<char> opciones) {
    char c;
    while (true) {
        cin >> c;
        c = tolower(c);
        for (auto o : opciones) {
            if (c == o) return c;
        }
        cout << "Entrada inválida. Intenta nuevamente: ";
    }
}


// -------------------------------------------------- //
//  SECCIÓN 4 — MENÚS INTERACTIVOS
// -------------------------------------------------- //

enum class MenuTipo {
    Vertical,
    Horizontal,
    Auto
};


//  Clase Menu  (Vertical / Horizontal / Auto)
class Menu {
private:
    vector<string> opciones;
    int seleccionado;
    MenuTipo tipo;

    int posX = -1, posY = -1;

    // Borra el area donde esta dibujado el menu antes de redibujar
    void limpiarArea(int lineas) {
        for (int i = 0; i < lineas; i++) {
            gotoxy(posX, posY + i);
            cout << string(120, ' ');
        }
    }

    // Dibuja todas las opciones en columna
    // Si mostrarIndicador=true  ? "[>] Opcion" / "[ ] Opcion"
    // Si mostrarIndicador=false ? solo "Opcion" con color activo/inactivo
    void dibujarVertical(int colorSeleccion, bool mostrarIndicador) {
        for (int i = 0; i < (int)opciones.size(); i++) {
            gotoxy(posX, posY + i);

            if (i == seleccionado) {
                color(colorSeleccion);
                if (mostrarIndicador) cout << "[>] ";
                cout << opciones[i];
            } else {
                color(15);
                if (mostrarIndicador) cout << "[ ] ";
                cout << opciones[i];
            }
        }
        color(15);
    }

    // Dibuja todas las opciones en una sola línea.
    // NOTA: NO se agrega espaciado automático entre elementos.
    //       Si necesitas separación, inclúyela en el texto de cada opción.
    void dibujarHorizontal(int colorSeleccion, bool mostrarIndicador) {
        gotoxy(posX, posY);
        for (int i = 0; i < (int)opciones.size(); i++) {
            if (i == seleccionado) {
                color(colorSeleccion);
                if (mostrarIndicador) cout << "[>] ";
                cout << opciones[i];
            } else {
                color(15);
                if (mostrarIndicador) cout << "[ ] ";
                cout << opciones[i];
            }
        }
        color(15);
    }

    // Calcula si todas las opciones caben en una línea horizontal
    bool cabeEnLinea() {
        int ancho = 0;
        for (auto& op : opciones)
            ancho += (int)op.size() + 4; // +4 por el "[>] " o "[ ] "
        return ancho < 110;
    }

public:
    Menu(const vector<string>& opciones, MenuTipo tipo = MenuTipo::Auto)
        : opciones(opciones), seleccionado(0), tipo(tipo) {}

    // Fija la posicion donde se dibujara el menu.
    // Si no se llama, se dibuja donde esté el cursor al invocar mostrar().
    void setPos(int x, int y) {
        posX = x;
        posY = y;
    }

    // Retorna el texto de la opcion actualmente seleccionada
    string obtenerSeleccion() {
        return opciones[seleccionado];
    }

    // -- mostrar() --------------------------------------------------
    // Muestra el menu y espera a que el usuario presione ENTER.
    // Retorna el índice (0-inicio) de la opcion elegida.
    //
    // Parametros:
    //   colorSeleccion  ?  color del ítem activo        (default: 2 = verde)
    //   mostrarIndicador?  muestra "[>]" / "[ ]"        (default: true)
    int mostrar(int colorSeleccion = 2, bool mostrarIndicador = true) {
        const int ENTER  = 13;
        const int PREFIX = 224;

        // Si no se fijo posicion, dibujar donde esté el cursor ahora
        if (posX == -1 || posY == -1) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            posX = csbi.dwCursorPosition.X;
            posY = csbi.dwCursorPosition.Y;
        }

        // Resolver tipo Auto
        MenuTipo tipoFinal = tipo;
        if (tipo == MenuTipo::Auto) {
            tipoFinal = cabeEnLinea() ? MenuTipo::Horizontal : MenuTipo::Vertical;
        }

        while (true) {
            limpiarArea(tipoFinal == MenuTipo::Vertical ? (int)opciones.size() : 1);

            if (tipoFinal == MenuTipo::Vertical)
                dibujarVertical(colorSeleccion, mostrarIndicador);
            else
                dibujarHorizontal(colorSeleccion, mostrarIndicador);

            int tecla = getch();

            if (tecla == ENTER)
                return seleccionado;

            if (tecla == PREFIX) {
                int flecha = getch();
                switch (flecha) {
                    case 72: case 75: seleccionado--; break; // arriba / izquierda
                    case 80: case 77: seleccionado++; break; // abajo  / derecha
                }
                if (seleccionado < 0)                      seleccionado = (int)opciones.size() - 1;
                if (seleccionado >= (int)opciones.size())  seleccionado = 0;
            }
        }
    }
};


// -------------------------------------------------
//  Clase MenuMatriz  (cuadrícula 2D navegable)
// -------------------------------------------------
//
//  Permite crear un menú en forma de tabla donde el usuario
//  se desplaza con las 4 flechas (? ? ? ?) y confirma con ENTER.
//
//  Cada celda es un string. Las filas deben tener el mismo
//  número de columnas.
//
//  EJEMPLO DE USO:
//
//      MenuMatriz mm({
//          {" A1 ", " A2 ", " A3 "},
//          {" B1 ", " B2 ", " B3 "},
//          {" C1 ", " C2 ", " C3 "}
//      });
//      mm.setPos(5, 3);
//      pair<int,int> resultado = mm.mostrar(10, false);
//      int filaElegida    = resultado.first;
//      int columnaElegida = resultado.second;
//
class MenuMatriz {
private:
    vector<vector<string>> celdas; // [fila][columna]
    int filas;
    int columnas;
    int filaActual    = 0;
    int columnaActual = 0;

    int posX = -1, posY = -1;

    // Borra toda el area de la cuadricula antes de redibujar
    void limpiarArea() {
        for (int fila = 0; fila < filas; fila++) {
            gotoxy(posX, posY + fila);
            cout << string(120, ' ');
        }
    }

    // Dibuja la cuadrícula completa.
    // La celda activa se resalta con colorSeleccion.
    // Si mostrarIndicador=true, la celda activa lleva "[>]" adelante.
    void dibujar(int colorSeleccion, bool mostrarIndicador) {
        for (int fila = 0; fila < filas; fila++) {
            gotoxy(posX, posY + fila);
            for (int col = 0; col < columnas; col++) {
                bool estaSeleccionado = (fila == filaActual && col == columnaActual);

                if (estaSeleccionado) {
                    color(colorSeleccion);
                    if (mostrarIndicador) cout << "[>]";
                    cout << celdas[fila][col];
                } else {
                    color(15);
                    if (mostrarIndicador) cout << "[ ]";
                    cout << celdas[fila][col];
                }
            }
        }
        color(15);
    }

public:
    MenuMatriz(const vector<vector<string>>& celdas)
        : celdas(celdas) {
        filas    = (int)celdas.size();
        columnas = (filas > 0) ? (int)celdas[0].size() : 0;
    }

    // Fija la posicion donde se dibujara la cuadricula.
    // Si no se llama, se dibuja donde este el cursor al invocar mostrar().
    void setPos(int x, int y) {
        posX = x;
        posY = y;
    }

    // Retorna el texto de la celda actualmente seleccionada
    string obtenerSeleccion() {
        return celdas[filaActual][columnaActual];
    }

    // -- mostrar() --------------------------------------------------
    // Muestra la cuadrícula y espera a que el usuario presione ENTER.
    // Retorna pair<fila, columna> de la celda elegida (índices 0-based).
    //
    // Parametros:
    //   colorSeleccion  ?  color de la celda activa     (default: 2 = verde)
    //   mostrarIndicador?  muestra "[>]" / "[ ]"        (default: true)
    pair<int,int> mostrar(int colorSeleccion = 2, bool mostrarIndicador = true) {
        const int ENTER  = 13;
        const int PREFIX = 224;

        // Si no se fijo posicion, dibujar donde esté el cursor ahora
        if (posX == -1 || posY == -1) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            posX = csbi.dwCursorPosition.X;
            posY = csbi.dwCursorPosition.Y;
        }

        while (true) {
            limpiarArea();
            dibujar(colorSeleccion, mostrarIndicador);

            int tecla = getch();

            if (tecla == ENTER)
                return { filaActual, columnaActual };

            if (tecla == PREFIX) {
                int flecha = getch();
                switch (flecha) {
                    case 72: filaActual--;    break; // ? arriba
                    case 80: filaActual++;    break; // ? abajo
                    case 75: columnaActual--; break; // ? izquierda
                    case 77: columnaActual++; break; // ? derecha
                }

                // Wraparound: si sale de los limites, aparece por el lado opuesto
                if (filaActual < 0)          filaActual    = filas    - 1;
                if (filaActual >= filas)     filaActual    = 0;
                if (columnaActual < 0)       columnaActual = columnas - 1;
                if (columnaActual >= columnas) columnaActual = 0;
            }
        }
    }
};


// -------------------------------------------------- //
//  SECCION 5 — BOTONES Y CAJAS INTERACTIVOS
// -------------------------------------------------- //

void drawBox(int x, int y, int w, int h) {
    gotoxy(x, y);         cout << char(218);
    gotoxy(x + w, y);     cout << char(191);
    gotoxy(x, y + h);     cout << char(192);
    gotoxy(x + w, y + h); cout << char(217);

    for (int i = 1; i < w; i++) {
        gotoxy(x + i, y);     cout << char(196);
        gotoxy(x + i, y + h); cout << char(196);
    }

    for (int i = 1; i < h; i++) {
        gotoxy(x, y + i);     cout << char(179);
        gotoxy(x + w, y + i); cout << char(179);
    }
}

void drawButton(int x, int y, string texto, bool activo) {
    int w = (int)texto.size() + 4;
    color(activo ? 10 : 15);
    drawBox(x, y, w, 2);
    gotoxy(x + 2, y + 1);
    cout << texto;
    color(15);
}


// -------------------------------------------------- //
//  SECCION 6 — TAMAÑO DE CONSOLA
// -------------------------------------------------- //

void setConsoleSize(int cols, int rows) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { (SHORT)cols, (SHORT)rows };
    SetConsoleScreenBufferSize(hOut, bufferSize);
    SMALL_RECT windowSize = { 0, 0, (SHORT)(cols - 1), (SHORT)(rows - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
}


// -------------------------------------------------- //
//  SECCION 7 — ANIMACIONES DE TEXTO
// -------------------------------------------------- //

// Escribe el texto caracter a caracter simulando una maquina de escribir
void escribirAnimado(int x, int y, string texto, int delayMs = 40) {
    gotoxy(x, y);
    for (char c : texto) {
        cout << c;
        Sleep(delayMs);
    }
}

// Hace aparecer el texto con fade-in: de gris oscuro hasta blanco
void textoFade(int x, int y, string texto) {
    int colores[] = {8, 7, 15};
    for (int c : colores) {
        color(c);
        gotoxy(x, y);
        cout << texto;
        Sleep(80);
    }
    color(15);
}


