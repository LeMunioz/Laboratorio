#include <iostream>
#include <fstream>
#include <string>
#include "colores.cpp"
using namespace std;

/*
ANALIZADOR LEXICO v1 [Comentarios y espacios vacios]
Angel Eduardo Mu?oz Perez
Febrero 2026
ING COMPUTACION _ CUALTOS
Compiladores Prof. Me?o

FUNCIONAMIENTO
==== Analizador Lexico
		Lee un archivo cpp o txt que el usuario ingrese en archivoEntrada > entrada y
		opera inmediatamente escribiendo en archivoSalida > salida. Por cada linea, 
		mete el resultado en la variable resultado, checa dentro de la funcion eliminarComentarios()
		mandamos la ubicacion del bool dentroDeComentarioBloque para verificar que no estamos en un
		comentario durante la lectura de la linea, luego si fija si entra a algun string o caracter,
		si es asi activa una bandera correspondiente. Despues entra a la condicion que pregunta a la 
		bandera para ver si puede copear el codigo o no, despues de eso checa existencia de comentarios
		ya sean con // o bloques de de comentarios /+ +/ hasta el final manda llamar la funcion 
		tieneContenido() que checa lineas vacias y se las brinca si ese es el caso.
*/

// Funci?n para procesar una l?nea y eliminar comentarios
string eliminarComentarios(string linea, bool &dentroDeComentarioBloque) {
    string resultado = "";
    bool dentroDeString = false;
    bool dentroDeChar = false;
    
    for (int i = 0; i < linea.length(); i++) {
        char actual = linea[i];
        char siguiente = (i + 1 < linea.length()) ? linea[i+1] : '\0';       
        // Si ve comillas dobles (strings)
        if (actual == '"' && !dentroDeChar && !dentroDeComentarioBloque) {
            dentroDeString = !dentroDeString;
            resultado += actual;
            continue;
        }
        // Si ve comillas simples (chars)
        if (actual == '\'' && !dentroDeString && !dentroDeComentarioBloque) {
            dentroDeChar = !dentroDeChar;
            resultado += actual;
            continue;
        }        
        // Si estamos protegidos (dentro de string o char), copiar directo
        if (dentroDeString || dentroDeChar) {
            resultado += actual;
            continue;
        }        
        // Buscar fin de comentario de bloque */
        if (dentroDeComentarioBloque && actual == '*' && siguiente == '/') {
            dentroDeComentarioBloque = false;
            i++; // saltar '/'
            continue;
        }        
        // Si estamos en bloque, ignorar todo
        if (dentroDeComentarioBloque) {
            continue;
        }        
        // Detectar inicio de comentario de l?nea //
        if (actual == '/' && siguiente == '/') {
            break; // terminar procesamiento de esta l?nea
        }        
        // Detectar inicio de comentario de bloque /*
        if (actual == '/' && siguiente == '*') {
            dentroDeComentarioBloque = true;
            i++; // saltar '*'
            continue;
        }        
        // Caracter normal, agregar al resultado
        resultado += actual;
    }
    
    return resultado;
}

// Funci?n para verificar si una l?nea tiene contenido
bool tieneContenido(string linea) {
    for (int i = 0; i < linea.length(); i++) {
        if (linea[i] != ' ' && linea[i] != '\t') {
            return true;
        }
    }
    return false;
}

// Funci?n para eliminar espacios al final
string limpiarFinal(string linea) {
    while (linea.length() > 0 && (linea.back() == ' ' || linea.back() == '\t')) {
        linea.pop_back();
    }
    return linea;
}

int main() {
    string archivoEntrada, archivoSalida;
    
    color(5); cout<<"      ANALIZADOR LEXICO V1.01"<<endl;
    color(1); cout<<"           Hecho por Muñoz"<<endl;
              cout<<"============================================"<<endl;
    
    color(3);cout << "Ingrese el nombre del archivo de entrada: ";
    color(7);cin >> archivoEntrada;
    
    color(3);cout << "Ingrese el nombre del archivo de salida: ";
    color(7);cin >> archivoSalida;
    
    ifstream entrada(archivoEntrada);
    ofstream salida(archivoSalida);
    
    if (!entrada.is_open()) {
        color(4); cout << "Error: No se pudo abrir el archivo de entrada." << endl;
        color(15);return 1;
    }
    
    if (!salida.is_open()) {
        color(4); cout << "Error: No se pudo crear el archivo de salida." << endl;
        color(15);return 1;
    }
    
    string linea;
    bool dentroDeComentarioBloque = false;
    
    while (getline(entrada, linea)) {
        linea = eliminarComentarios(linea, dentroDeComentarioBloque);
        linea = limpiarFinal(linea);
        
        if (tieneContenido(linea)) {
            salida << linea << endl;
        }
    }
    
    entrada.close();
    salida.close();
    
    color(10); cout << "Proceso completado en el archivo: " << archivoSalida << endl;
    color(15);
    return 0;
}
