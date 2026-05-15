#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

/*
ANALIZADOR LEXICO v2 [Comentarios, espacios vacios y Tokenizacion]
Angel Eduardo Munoz Perez
Febrero 2026
ING COMPUTACION _ CUALTOS
Compiladores Prof. Meno
*/


// ------------------------------------------------- //
//    STRUCTURA DE TOKENS (compartida con sintactico y semantico)
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

    if (banderaComentarioMultilinea) {
        for (int i = 0; i < (int)linea.length(); i++) {
            if (esComentarioMultilineaFin(linea, i)) {
                banderaComentarioMultilinea = false;
                i++;
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
}


// ------------------------------------------------- //
//    FUNCIONES DEL ANALIZADOR LEXICO (Parte 2): TOKENS
// ------------------------------------------------- //

// Lista de palabras reservadas reconocidas
// "void" incluido para que el analizador sintactico lo reconozca como tipo
string palabrasReservadas[] = {
    "if", "else", "for", "while", "do", "switch", "case",
    "return", "break", "continue", "int", "float", "char",
    "bool", "cin", "cout", "string", "void"
};
int totalPalabrasReservadas = 18;

bool esPalabraReservada(string palabra) {
    for (int i = 0; i < totalPalabrasReservadas; i++) {
        if (palabrasReservadas[i] == palabra) return true;
    }
    return false;
}

string leerIdentificador(string linea, int &pos) {
    string res = "";
    while (pos < (int)linea.length() && (isalnum(linea[pos]) || linea[pos] == '_')) {
        res += linea[pos];
        pos++;
    }
    pos--;
    return res;
}

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

string leerCadena(string linea, int &pos) {
    char delimitador = linea[pos];
    string res = "";
    res += linea[pos];
    pos++;
    while (pos < (int)linea.length()) {
        res += linea[pos];
        if (linea[pos] == delimitador && linea[pos - 1] != '\\') break;
        pos++;
    }
    return res;
}

string leerLibreria(string linea) {
    return linea;
}

void tokenizarLinea(string linea, short numLinea, vector<Token> &tokens) {

    if (!linea.empty() && linea[0] == '#') {
        tokens.push_back({"Libreria", leerLibreria(linea), numLinea});
        return;
    }

    for (int i = 0; i < (int)linea.length(); i++) {
        char c = linea[i];

        if (c == ' ' || c == '\t') continue;

        Token t;
        t.linea = numLinea;

        if (c == '"' || c == '\'') {
            t.cadena = leerCadena(linea, i);
            t.tipo = "Cadena";
        }
        else if (isdigit(c)) {
            bool esDecimal = false;
            t.cadena = leerNumero(linea, i, esDecimal);
            t.tipo = esDecimal ? "Decimal" : "Entero";
        }
        else if (isalpha(c) || c == '_') {
            t.cadena = leerIdentificador(linea, i);
            t.tipo = esPalabraReservada(t.cadena) ? "PalabraRes" : "ID";
        }
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
        else {
            switch (c) {
                case '+': case '-': case '*': case '/':
                case '%': case '>': case '<': case '!':
                    t.cadena = string(1, c);
                    t.tipo = "Operador";
                    break;
                case '=':
                    t.cadena = "=";
                    t.tipo = "Asignacion";
                    break;
                case ';': case '{': case '}': case '(': case ')':
                case '[': case ']': case ',':
                    t.cadena = string(1, c);
                    t.tipo = "Delimitador";
                    break;
                default:
                    t.cadena = string(1, c);
                    t.tipo = "Desconocido";
                    break;
            }
        }

        tokens.push_back(t);
    }
}

void mostrarTablaTokens(vector<Token> &tokens) {
    color(14);
    cout << left;
    cout << setw(12) << "TOKEN"            << "|"
         << setw(22) << "Token Encontrado" << "|"
         << "LINEA" << endl;
    cout << string(12, '_') << "|"
         << string(22, '_') << "|"
         << string(10, '_') << endl;
    color(15);

    for (int i = 0; i < (int)tokens.size(); i++) {
        if      (tokens[i].tipo == "PalabraRes")  color(11);
        else if (tokens[i].tipo == "ID")           color(15);
        else if (tokens[i].tipo == "Entero")       color(10);
        else if (tokens[i].tipo == "Decimal")      color(10);
        else if (tokens[i].tipo == "Operador")     color(12);
        else if (tokens[i].tipo == "Asignacion")   color(12);
        else if (tokens[i].tipo == "Delimitador")  color(14);
        else if (tokens[i].tipo == "Cadena")       color(13);
        else if (tokens[i].tipo == "Libreria")     color(9);
        else                                       color(8);

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
}
