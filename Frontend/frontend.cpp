#include <iostream>
#include <vector>
#include <cctype>
#include <string>
#include <conio.h>
#include <windows.h>
using namespace std;

/*
LIBRERIA DE FRONT-END PARA C++
Hecho por: Angel Eduardo Mu?oz Perez & Javier Garnica Escamilla 

imprimir en colores en consola - - - - 1
imprimir con cordenadas en consola - - 2
validacion de entrada de datos - - - - 3
menus interactivos - - - - - - - - - - 4
botones y cajas interactivos - - - - - 5
tama?o de consolas - - - - - - - - - - 6
animacion de textos- - - - - - - - - - 7


1) IMPRIMIR EN COLORES EN CONSOLA
    Todo lo que se imprima despu?s cambiar? de color seg?n el valor asignado.
    COLORES DISPONIBLES:
        [1]   AZUL REY
        [2]   VERDE
        [3]   AZUL CYAN
        [4]   ROJO
        [5]   VIOLETA
        [6]   AMARILLO OCRE
        [7]   GRIS MUY CLARO
        [8]   GRIS
        [9]   AZUL
        [10]  VERDE PASTO
        [11]  CELESTE
        [12]  ROJO CLARO
        [13]  MORADO
        [14]  CREMA
        [15]  BLANCO
        [16]++ FONDO DE COLOR

    Ejemplo de sintaxis:
        color(12);   // Pinta en rojo claro
        cout << "Hola mundo en rojo";

2) IMPRIMIR CON CORDENADAS EN CONSOLA
    Puedes mover el cursor de escritura a una posici?n exacta antes de imprimir.
    - gotoxy(x, y)  ? mueve en columnas (x) y renglones (y)
    - gotox(x)      ? mueve solo a la columna x, respetando el rengl?n actual

    Ejemplo de sintaxis:
        gotoxy(10, 5);
        cout << "Texto empieza en 10,5";

3) VALIDACION DE ENTRADA DE DATOS
    Estas funciones permiten validar lo que el usuario escribe evitando errores comunes.

    PARA VALIDAR ENTEROS:
        leerOpcion(minimo, maximo);
    La funci?n solo acepta n?meros dentro del rango indicado.
    
    Ejemplo:
        int edad = leerOpcion(1, 120);

    PARA VALIDAR CARACTERES:
        leerCharOpcional({ 's', 'n', 'a' });
    La funci?n convierte a min?sculas y solo acepta uno de los caracteres permitidos en formato de vector.

    Ejemplo:
        char c = leerCharOpcional({'s','n'});
        // Solo acepta s o n (may?sculas tambi?n funcionan)

4) MENUS INTERACTIVOS
    Sistema de men?s navegables usando flechas < > ^ v & ENTER.
    Soporta estos tipos de menu:
        - Men? vertical      
        - Men? horizontal 		
        - Modo autom?tico (elige el mejor seg?n ancho)

    Sintaxis b?sica:
        Menu miMenu({"Opcion 1", "Opcion 2", ... "OpcionN"}MenuTipo:"tipo");
        int elegido = miMenu.mostrar();

5) BOTONES INTERACTIVOS


6) CONSOLA DE TAMA?O   


7) ANIMACION DE TEXTOS

     
*/


//	==========  //
//  CORDENADAS  //
//	==========  // 
void gotoxy(int x,int y){
	HANDLE hcon;
	hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD dwPos;
	
	dwPos.X = x;
	dwPos.Y = y;
	
	SetConsoleCursorPosition(hcon,dwPos);
}
void gotox(int x){
	HANDLE hcon;
	hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD dwPos;
	
	dwPos.X = x;
	
	SetConsoleCursorPosition(hcon,dwPos);
}

//	=======  //
//  COLORES  //
//	======== //
void color(int color){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleTextAttribute(hConsole, color);
}

//	======================  //
//  VALIDACION DE ENTRADAS  
//	======================  //
int leerOpcion(int min, int max) {
    int opcion;
    while (true) {
        cin >> opcion;
        if (!cin.fail() && opcion >= min && opcion <= max) break;
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Entrada inv?lida. Intenta nuevamente: ";
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
        cout << "Entrada inv?lida. Intenta nuevamente: ";
    }
}

//  ==================  //
//  MENUS INTERACTIVOS  //
//	==================  //
enum class MenuTipo {
    Vertical,
    Horizontal,
    Auto
};

class Menu {
private:
    vector<string> opciones;
    int seleccionado;
    MenuTipo tipo;

    // Posici?n donde se dibuja el men?
    int posX = -1, posY = -1;

    // Limpia las l?neas previas del men? sin borrar toda la pantalla
    void limpiarArea(int lineas) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        for (int i = 0; i < lineas; i++) {
            gotoxy(posX, posY + i);
            cout << string(120, ' '); // borra la l?nea
        }
    }

public:
    Menu(const vector<string>& opciones, MenuTipo tipo = MenuTipo::Auto)
        : opciones(opciones), seleccionado(0), tipo(tipo) {}

    // Permite elegir manualmente d?nde dibujar el men?
    void setPos(int x, int y) {
        posX = x;
        posY = y;
    }

    // Regresa el texto de la opci?n seleccionada
    string obtenerSeleccion() {
        return opciones[seleccionado];
    }

    bool cabeEnLinea() {
        int ancho = 0;
        for (auto& op : opciones)
            ancho += op.size() + 6;
        return ancho < 110;
    }

    void dibujarVertical() {
        for (int i = 0; i < opciones.size(); i++) {
            gotoxy(posX, posY + i);

            if (i == seleccionado) {
                color(2);
                cout << "[>] " << opciones[i];
            } else {
                color(15);
                cout << "[ ] " << opciones[i];
            }
        }
    }

    void dibujarHorizontal() {
        gotoxy(posX, posY);

        for (int i = 0; i < opciones.size(); i++) {
            if (i == seleccionado) {
                color(2);
                cout << "[>] " << opciones[i] << "   ";
            } else {
                color(15);
                cout << "[ ] " << opciones[i] << "   ";
            }
        }
    }

    // ----------- AQUI OCURRE LAS COSAS -----------
    int mostrar() {
        const int ENTER = 13;
        const int PREFIX = 224;

        // Si no se defini? posici?n: se dibuja donde est? el cursor
        if (posX == -1 || posY == -1) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            posX = csbi.dwCursorPosition.X;
            posY = csbi.dwCursorPosition.Y;
        }

        MenuTipo tipoFinal = tipo;
        if (tipo == MenuTipo::Auto) {
            tipoFinal = cabeEnLinea() ? MenuTipo::Horizontal : MenuTipo::Vertical;
        }

        while (true) {

            // Limpia SOLO el ?rea ocupada por el men?, sin borrar pantalla
            limpiarArea(tipoFinal == MenuTipo::Vertical ? opciones.size() : 1);

            // Lo vuelve a dibujar en su misma posici?n
            if (tipoFinal == MenuTipo::Vertical)
                dibujarVertical();
            else
                dibujarHorizontal();

            // Leer tecla
            int tecla = getch();

            if (tecla == ENTER)
                return seleccionado;

            if (tecla == PREFIX) {
                int flecha = getch();

                switch (flecha) {
                case 72: // arriba
                case 75: // izquierda
                    seleccionado--;
                    break;

                case 80: // abajo
                case 77: // derecha
                    seleccionado++;
                    break;
                }

                if (seleccionado < 0) seleccionado = opciones.size() - 1;
                if (seleccionado >= opciones.size()) seleccionado = 0;
            }
        }
    }
};

//  ============================ //
//  BOTONES Y CAJAS INTERACTIVOS //
//  ============================ //
void drawBox(int x, int y, int w, int h) {
    // Esquinas
    gotoxy(x, y);         cout << char(218);
    gotoxy(x + w, y);     cout << char(191);
    gotoxy(x, y + h);     cout << char(192);
    gotoxy(x + w, y + h); cout << char(217);

    // Horizontales
    for (int i = 1; i < w; i++) {
        gotoxy(x + i, y);     cout << char(196);
        gotoxy(x + i, y + h); cout << char(196);
    }

    // Verticales
    for (int i = 1; i < h; i++) {
        gotoxy(x, y + i);     cout << char(179);
        gotoxy(x + w, y + i); cout << char(179);
    }
}

// -- BOTONES -- //
void drawButton(int x, int y, string texto, bool activo) {
    int w = texto.size() + 4;

    color(activo ? 10 : 15);
    drawBox(x, y, w, 2);

    gotoxy(x + 2, y + 1);
    cout << texto;

    color(15);
}


//  ================= //
//  TAMA?O DE CONSOLA //
//  ================= //
void setConsoleSize(int cols, int rows) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Ajustar buffer
    COORD bufferSize = { (SHORT)cols, (SHORT)rows };
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // Ajustar ventana
    SMALL_RECT windowSize = { 0, 0, (SHORT)(cols - 1), (SHORT)(rows - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
}

//  ==================== //
//  ANIMACIONES DE TEXTO //
//  ==================== //
// Escribir tipo maquina de texto
void escribirAnimado(int x, int y, string texto, int delayMs = 30) {
    gotoxy(x, y);
    for (char c : texto) {
        cout << c;
        Sleep(delayMs);
    }
}

//fade in usando colores (de negro a blanco)
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


