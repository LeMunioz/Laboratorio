#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// frontend.cpp PRIMERO: define color(), gotoxy(), Menu, explorador, etc.
#include "frontend.cpp"

// Analizador Lexico: define Token, procesarLinea, tokenizarLinea, mostrarTablaTokens, etc.
#include "analizadorLexico.cpp"

// Analizador Sintactico: define Regla, tabla, inicializarTabla, analizarSintaxis, etc.
#include "analizadorSintactico.cpp"

// Analizador Semantico: define NodoCST, tablaSimbolos, construirArbol, analizarSemantica, etc.
#include "AnalizadorSemantico.cpp"

using namespace std;

// -------------------------------------------------------
//  Imprime un separador de seccion con color y titulo
// -------------------------------------------------------
void seccion(const string &titulo, int colorNum = 3) {
    color(colorNum);
    string linea(60, '=');
    cout << "\n" << linea << "\n";
    cout << "  " << titulo << "\n";
    cout << linea << "\n";
    color(15);
}


// -------------------------------------------------------
//  MAIN
// -------------------------------------------------------
int main() {

    // --------------------------------------------------
    //  Pantalla de bienvenida
    // --------------------------------------------------
    system("cls");
    color(3);
    cout << R"(
  +=================================================+
  |         COMPILADOR DE MUNOZ                    |
  |  Lexico  ->  Sintactico  ->  Semantico         |
  +=================================================+
)" << endl;
    color(15);

    // --------------------------------------------------
    //  PASO 1: Cargar archivo fuente con el explorador
    // --------------------------------------------------
    seccion("PASO 1 - CARGAR ARCHIVO FUENTE");

    ifstream archivoEntrada;
    abrirArchivoEntrada(archivoEntrada);

    if (!archivoEntrada.is_open()) {
        color(12);
        cout << "\n  No se cargo ningun archivo. Cerrando compilador.\n";
        color(15);
        return 1;
    }

    // --------------------------------------------------
    //  PASO 2: Cargar archivo de salida (limpio)
    // --------------------------------------------------
    seccion("PASO 2 - ELEGIR ARCHIVO DE SALIDA (codigo limpio)");

    ofstream archivoSalida;
    abrirArchivoSalida(archivoSalida);

    bool guardarSalida = archivoSalida.is_open();
    if (!guardarSalida) {
        color(14);
        cout << "\n  Continuando sin guardar archivo limpio...\n";
        color(15);
    }

    // --------------------------------------------------
    //  PASO 3: ANALISIS LEXICO - Limpieza y tokenizacion
    // --------------------------------------------------
    seccion("PASO 3 - ANALISIS LEXICO");

    color(3);
    cout << "\n[3.1] Limpiando comentarios y lineas vacias...\n";
    color(15);

    string linea;
    bool banderaComentarioMultilinea = false;
    vector<string> lineasLimpias;

    while (getline(archivoEntrada, linea)) {
        string lineaResultado = procesarLinea(linea, banderaComentarioMultilinea);
        if (lineaTieneContenido(lineaResultado)) {
            if (guardarSalida) {
                archivoSalida << lineaResultado << "\n";
            }
            lineasLimpias.push_back(lineaResultado);
        }
    }

    archivoEntrada.close();
    if (guardarSalida) archivoSalida.close();

    color(2);
    cout << "  Limpieza completada. Lineas validas: " << lineasLimpias.size() << "\n";
    color(15);

    // Tokenizar
    color(3);
    cout << "\n[3.2] Tokenizando codigo...\n";
    color(15);

    vector<Token> tokens;
    for (int i = 0; i < (int)lineasLimpias.size(); i++) {
        short numLinea = (short)(i + 1);
        tokenizarLinea(lineasLimpias[i], numLinea, tokens);
    }

    color(3);
    cout << "\n===== TABLA DE TOKENS =====\n";
    color(15);
    mostrarTablaTokens(tokens);

    color(2);
    cout << "\n  Total de tokens encontrados: " << tokens.size() << "\n";
    color(15);

    if (tokens.empty()) {
        color(12);
        cout << "\n  No se encontraron tokens. Verificar el archivo de entrada.\n";
        color(15);
        return 1;
    }

    // Pausar entre fases
    color(14);
    cout << "\n  [Presione una tecla para continuar con el analisis sintactico...]\n";
    color(15);
    cin.get();

    // --------------------------------------------------
    //  PASO 4: ANALISIS SINTACTICO
    // --------------------------------------------------
    seccion("PASO 4 - ANALISIS SINTACTICO (LR)");

    // El analizador sintactico agrega "$" al final del vector,
    // por eso trabajamos sobre una copia local para el sintactico
    // (el semantico necesita los tokens originales sin el "$")
    vector<Token> tokensSintactico = tokens;

    inicializarTabla();

    color(3);
    cout << "\n===== TRACE DEL ANALISIS SINTACTICO =====\n";
    color(15);

    bool resultadoSintactico = analizarSintaxis(tokensSintactico);

    cout << "\n" << string(60, '=') << "\n";
    if (resultadoSintactico) {
        color(10);
        cout << "  *** SINTACTICO CORRECTO: El codigo paso el test sintactico ***\n";
    } else {
        color(12);
        cout << "  *** SINTACTICO FALLIDO: El codigo contiene errores sintacticos ***\n";
    }
    color(15);
    cout << string(60, '=') << "\n";

    // Pausar entre fases
    color(14);
    cout << "\n  [Presione una tecla para continuar con el analisis semantico...]\n";
    color(15);
    cin.get();

    // --------------------------------------------------
    //  PASO 5: ANALISIS SEMANTICO
    // --------------------------------------------------
    seccion("PASO 5 - ANALISIS SEMANTICO (CST + Verificacion de Tipos)");

    color(3);
    cout << "\n[5.1] Construyendo arbol de sintaxis concreto (CST)...\n";
    color(15);

    NodoCST* arbol = construirArbol(tokens);

    color(3);
    cout << "\n===== ARBOL DE SINTAXIS CONCRETO =====\n";
    color(15);
    imprimirArbol(arbol);

    color(3);
    cout << "\n[5.2] Verificando tipos y redeclaraciones...\n";
    color(15);

    color(3);
    cout << "\n===== RESULTADO DEL ANALISIS SEMANTICO =====\n";
    color(15);

    analizarSemantica(arbol);

    cout << "\n" << string(60, '=') << "\n";
    if (!huboError) {
        color(10);
        cout << "  *** SEMANTICO CORRECTO: El codigo paso todas las pruebas ***\n";
    } else {
        color(12);
        cout << "  *** SEMANTICO FALLIDO: El codigo contiene errores semanticos ***\n";
    }
    color(15);
    cout << string(60, '=') << "\n";

    // --------------------------------------------------
    //  RESUMEN FINAL
    // --------------------------------------------------
    seccion("RESUMEN FINAL", 14);

    color(14);
    cout << "  Analisis Lexico    : ";
    color(10); cout << "OK (" << tokens.size() << " tokens)\n";

    color(14);
    cout << "  Analisis Sintactico: ";
    if (resultadoSintactico) { color(10); cout << "CORRECTO\n"; }
    else                     { color(12); cout << "CON ERRORES\n"; }

    color(14);
    cout << "  Analisis Semantico : ";
    if (!huboError) { color(10); cout << "CORRECTO\n"; }
    else            { color(12); cout << "CON ERRORES\n"; }

    color(15);
    cout << "\n";

    // Liberar memoria del arbol
    delete arbol;

    color(14);
    cout << "  [Presione una tecla para salir...]\n";
    color(15);
    cin.get();

    return 0;
}
