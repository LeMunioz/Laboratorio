#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "colores.cpp"
using namespace std;

/*
ANALIZADOR LEXICO v2 [Comentarios, espacios vacios y Tokenizacion]
Angel Eduardo Munoz Perez
Febrero 2026
ING COMPUTACION _ CUALTOS
Compiladores Prof. Meno

FUNCIONAMIENTO
==== Analizador Lexico
     PARTE 1 LIMPIADOR DE COMENTARIOS
        Le pide al usuario el nombre del archivo en > origen y la compilacion la escribe en > destino
        mientras haya lineas en origen, entra al archivo, por cada linea entra a un for para
        recorrer la linea, entra a un switch para ver que tipo de texto hay, checa si se encuentra
        un comentario o si esta dentro de un string o caracter, todo utilizando banderas, cada bandera
        se activa y desactiva invirtiendo su valor cuando entra al caso y estas se usan para verificar
        en donde se encuentra el caracter y distinguir si es un comentario, si lo es fijarse si esta
        dentro de una cadena o no, para ver si lo escribe en destino, y si no hay ninguna bandera, por
        default escribir tambien el caracter en destino, solo se hace este proceso por cada linea que si
        tenga contenido que verifica con un for dentro de lineaTieneContenido()

     PARTE 2 ANALIZADOR LEXICO
        Lee el archivo destino ya limpio linea por linea, por cada linea llama a tokenizarLinea()
        que recorre la linea caracter por caracter. Segun el caracter actual decide que tipo de token
        empieza: si es '#' lee toda la linea como libreria, si es '"' o '\'' lee hasta el cierre como
        cadena, si es digito lee el numero completo (entero o decimal), si es letra o '_' lee el
        identificador y verifica si es palabra reservada. Para operadores de dos caracteres como ==,
        ||, &&, << y >> revisa el siguiente caracter antes de decidir. Delimitadores y caracteres
        desconocidos se manejan con un switch. Todos los tokens se guardan en un vector de structs
        y al final se imprime la tabla en pantalla con colores.
*/


// ------------------------------------------------- //
//    STRUCTURA DE TOKENS
// ------------------------------------------------- //
struct Token {
    string tipo;
    string cadena;
    short linea;
};


// ------------------------------------------------- //
//    FUNCIONES DE LIMPIADOR DE COMENTARIOS (Parte 1)
// ------------------------------------------------- //

bool esComentarioLinea(string linea, int posicion) {
    return (posicion + 1 < (int)linea.length() &&
            linea[posicion] == '/' &&
            linea[posicion + 1] == '/');
}

bool esComentarioMultilineaInicio(string linea, int posicion) {
    return (posicion + 1 < (int)linea.length() &&
            linea[posicion] == '/' &&
            linea[posicion + 1] == '*');
}

bool esComentarioMultilineaFin(string linea, int posicion) {
    return (posicion + 1 < (int)linea.length() &&
            linea[posicion] == '*' &&
            linea[posicion + 1] == '/');
}

bool esCaracterEscape(string linea, int posicion) {
    return (posicion > 0 && linea[posicion - 1] == '\\');
}

bool lineaTieneContenido(string linea) {
    for (int i = 0; i < (int)linea.length(); i++) {
        if (linea[i] != ' ' && linea[i] != '\t') {
            return true;
        }
    }
    return false;
}

string procesarLinea(string linea, bool &banderaComentarioMultilinea) {
    string resultado = "";

    // Si ya estamos en comentario multilinea, buscar el fin
    if (banderaComentarioMultilinea) {
        for (int i = 0; i < (int)linea.length(); i++) {
            if (esComentarioMultilineaFin(linea, i)) {
                banderaComentarioMultilinea = false;
                i++; // Saltar el '/'
                for (int j = i + 1; j < (int)linea.length(); j++) {
                    resultado += linea[j];
                }
                break;
            }
        }
        return resultado;
    }

    bool banderaString = false;
    bool banderaCaracter = false;

    for (int i = 0; i < (int)linea.length(); i++) {
        char caracterActual = linea[i];

        switch (caracterActual) {
            case '"':
                if (!banderaCaracter && !esCaracterEscape(linea, i)) {
                    banderaString = !banderaString;
                }
                resultado += caracterActual;
                break;

            case '\'':
                if (!banderaString && !esCaracterEscape(linea, i)) {
                    banderaCaracter = !banderaCaracter;
                }
                resultado += caracterActual;
                break;

            case '/':
                if (banderaString || banderaCaracter) {
                    resultado += caracterActual;
                } else {
                    if (esComentarioMultilineaInicio(linea, i)) {
                        banderaComentarioMultilinea = true;
                        i++;
                    } else if (esComentarioLinea(linea, i)) {
                        return resultado;
                    } else {
                        resultado += caracterActual;
                    }
                }
                break;

            default:
                resultado += caracterActual;
                break;
        }
    }

    return resultado;
}// fin de procesarLinea


// ------------------------------------------------- //
//    FUNCIONES DEL ANALIZADOR LEXICO (Parte 2): TOKENS
// ------------------------------------------------- //

// Lista de palabras reservadas reconocidas
string palabrasReservadas[] = {
    "if", "else", "for", "while", "do", "switch", "case",
    "return", "break", "continue", "int", "float", "char",
    "bool", "cin", "cout", "string"
};
int totalPalabrasReservadas = 17;

bool esPalabraReservada(string palabra) {
    for (int i = 0; i < totalPalabrasReservadas; i++) {
        if (palabrasReservadas[i] == palabra) return true;
    }
    return false;
}

// Lee un identificador completo desde la posicion actual
// (letras, digitos y guion bajo - debe iniciar con letra o '_')
string leerIdentificador(string linea, int &pos) {
    string res = "";
    while (pos < (int)linea.length() && (isalnum(linea[pos]) || linea[pos] == '_')) {
        res += linea[pos];
        pos++;
    }
    pos--; // El for principal hara pos++ al terminar
    return res;
}

// Lee un numero completo, detecta si tiene punto decimal
string leerNumero(string linea, int &pos, bool &esDecimal) {
    string res = "";
    esDecimal = false;
    while (pos < (int)linea.length() && (isdigit(linea[pos]) || linea[pos] == '.')) {
        if (linea[pos] == '.') esDecimal = true;
        res += linea[pos];
        pos++;
    }
    pos--;
    return res;
}

// Lee una cadena o caracter incluyendo los delimitadores "" o ''
string leerCadena(string linea, int &pos) {
    char delimitador = linea[pos];
    string res = "";
    res += linea[pos];
    pos++;
    while (pos < (int)linea.length()) {
        res += linea[pos];
        // Cierra cuando encuentra el mismo delimitador sin escape
        if (linea[pos] == delimitador && linea[pos - 1] != '\\') break;
        pos++;
    }
    return res;
}

// Lee una directiva de libreria: toda la linea empieza con '#'
string leerLibreria(string linea) {
    return linea;
}

// Tokeniza una linea completa y agrega los tokens al vector
void tokenizarLinea(string linea, short numLinea, vector<Token> &tokens) {

    // Toda linea que empieza con '#' es una libreria
    if (!linea.empty() && linea[0] == '#') {
        tokens.push_back({"Libreria", leerLibreria(linea), numLinea});
        return;
    }

    for (int i = 0; i < (int)linea.length(); i++) {
        char c = linea[i];

        // Saltar espacios y tabulaciones
        if (c == ' ' || c == '\t') continue;

        Token t;
        t.linea = numLinea;

        // ---- Cadenas y caracteres: inician con " o ' ----
        if (c == '"' || c == '\'') {
            t.cadena = leerCadena(linea, i);
            t.tipo = "Cadena";
        }
        // ---- Numeros: inician con digito ----
        else if (isdigit(c)) {
            bool esDecimal = false;
            t.cadena = leerNumero(linea, i, esDecimal);
            t.tipo = esDecimal ? "Decimal" : "Entero";
        }
        // ---- Identificadores y palabras reservadas ----
        else if (isalpha(c) || c == '_') {
            t.cadena = leerIdentificador(linea, i);
            t.tipo = esPalabraReservada(t.cadena) ? "PalabraRes" : "ID";
        }
        // ---- Operadores de dos caracteres (revisar siguiente antes de decidir) ----
        else if (c == '=' && i + 1 < (int)linea.length() && linea[i + 1] == '=') {
            t.cadena = "=="; t.tipo = "Operador"; i++;
        }
        else if (c == '|' && i + 1 < (int)linea.length() && linea[i + 1] == '|') {
            t.cadena = "||"; t.tipo = "Operador"; i++;
        }
        else if (c == '&' && i + 1 < (int)linea.length() && linea[i + 1] == '&') {
            t.cadena = "&&"; t.tipo = "Operador"; i++;
        }
        else if (c == '<' && i + 1 < (int)linea.length() && linea[i + 1] == '<') {
            t.cadena = "<<"; t.tipo = "Operador"; i++;
        }
        else if (c == '>' && i + 1 < (int)linea.length() && linea[i + 1] == '>') {
            t.cadena = ">>"; t.tipo = "Operador"; i++;
        }
        // ---- Operadores y delimitadores de un caracter ----
        else {
            switch (c) {
                // Operadores aritmeticos y logicos de un caracter
                case '+': case '-': case '*': case '/':
                case '%': case '>': case '<': case '!':
                    t.cadena = string(1, c);
                    t.tipo = "Operador";
                    break;
                // Asignacion simple
                case '=':
                    t.cadena = "=";
                    t.tipo = "Asignacion";
                    break;
                // Delimitadores de bloques e instrucciones
                case ';': case '{': case '}': case '(': case ')': case '[': case ']':
                    t.cadena = string(1, c);
                    t.tipo = "Delimitador";
                    break;
                // Cualquier otro caracter no reconocido
                default:
                    t.cadena = string(1, c);
                    t.tipo = "Desconocido";
                    break;
            }
        }

        tokens.push_back(t);
    }
}// fin de tokenizarLinea

// Muestra la tabla de tokens en pantalla con colores por tipo
void mostrarTablaTokens(vector<Token> &tokens) {
    color(14); // Amarillo para el encabezado
    cout << left;
    cout << setw(12) << "TOKEN"      << "|"
         << setw(22) << "Token Encontrado" << "|"
         << "LINEA" << endl;
    cout << string(12, '_') << "|"
         << string(22, '_') << "|"
         << string(10, '_') << endl;
    color(15);

    for (int i = 0; i < (int)tokens.size(); i++) {
        // Color segun el tipo de token
        if      (tokens[i].tipo == "PalabraRes")  color(11); // Cyan
        else if (tokens[i].tipo == "ID")           color(15); // Blanco
        else if (tokens[i].tipo == "Entero")       color(10); // Verde claro
        else if (tokens[i].tipo == "Decimal")      color(10); // Verde claro
        else if (tokens[i].tipo == "Operador")     color(12); // Rojo
        else if (tokens[i].tipo == "Asignacion")   color(12); // Rojo
        else if (tokens[i].tipo == "Delimitador")  color(14); // Amarillo
        else if (tokens[i].tipo == "Cadena")       color(13); // Magenta
        else if (tokens[i].tipo == "Libreria")     color(9);  // Azul claro
        else                                       color(8);  // Gris para Desconocido

        // Truncar cadena si es muy larga para que no rompa la tabla
        string cadenaVisible = tokens[i].cadena;
        if ((int)cadenaVisible.length() > 21) {
            cadenaVisible = cadenaVisible.substr(0, 18) + "...";
        }

        cout << left
             << setw(12) << tokens[i].tipo    << "|"
             << setw(22) << cadenaVisible      << "|"
             << tokens[i].linea << endl;
    }
    color(15);
}// fin de mostrarTablaTokens


// ------------------------------------------------- //
//    MAIN
// ------------------------------------------------- //
int main() {
    string archivoOrigen, archivoDestino;

    // PEDIR ARCHIVOS AL USUARIO
    bool pidioCompilarAlgoInseguro = true;
    while (pidioCompilarAlgoInseguro) {
        color(3);
        cout << "Ingrese el nombre del archivo origen: ";
        color(7);
        cin >> archivoOrigen;

        if (archivoOrigen == "AnalizadorL.cpp") {
            color(12);
            cout << "ESTIMADO!!!! NO VUELVAS A COMETER EL MISMO ERROR, ESCOGE ALGO QUE NO SEA EL MISMO COMPILADOR PARA COMPILARLO >:{" << endl;
            continue;
        } else {
            color(3);
            cout << "Ingrese el nombre del archivo destino: ";
            color(7);
            cin >> archivoDestino;
            color(15);
            pidioCompilarAlgoInseguro = false;
        }
    }

    // Abrir archivos
    ifstream origen(archivoOrigen);
    ofstream destino(archivoDestino);

    if (!origen.is_open()) {
        color(12);
        cout << "Error: No se pudo abrir el archivo origen." << endl;
        color(15);
        return 1;
    }
    if (!destino.is_open()) {
        color(12);
        cout << "Error: No se pudo crear el archivo destino." << endl;
        color(15);
        return 1;
    }

    // ==================================================
    // PARTE 1: LIMPIAR COMENTARIOS Y LINEAS VACIAS
    // ==================================================
    color(3);
    cout << "\n[PARTE 1] Limpiando comentarios y lineas vacias..." << endl;
    color(15);

    string linea;
    bool banderaComentarioMultilinea = false;
    vector<string> lineasLimpias; // Guardamos las lineas limpias en memoria para la parte 2

    while (getline(origen, linea)) {
        string lineaResultado = procesarLinea(linea, banderaComentarioMultilinea);
        if (lineaTieneContenido(lineaResultado)) {
            destino << lineaResultado << endl;
            lineasLimpias.push_back(lineaResultado); // Guardar para tokenizar
        }
    }

    origen.close();
    destino.close();

    color(2);
    cout << "Limpieza completada. Archivo generado: " << archivoDestino << endl;
    color(15);

    // ==================================================
    // PARTE 2: TOKENIZAR EL CODIGO LIMPIO
    // ==================================================
    color(3);
    cout << "\n[PARTE 2] Analizando tokens..." << endl;
    color(15);

    vector<Token> tokens;

    for (int i = 0; i < (int)lineasLimpias.size(); i++) {
        short numLinea = (short)(i + 1);
        tokenizarLinea(lineasLimpias[i], numLinea, tokens);
    }

    // Mostrar resultado
    color(3);
    cout << "\n===== TABLA DE TOKENS =====" << endl;
    color(15);
    mostrarTablaTokens(tokens);

    color(2);
    cout << "\nTotal de tokens encontrados: " << tokens.size() << endl;
    color(15);

    return 0;
}// fin de main
