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

int main() {
    string archivoOrigen, archivoDestino;
    
    // Pedir archivo origen y destino
    cout << "Ingrese el nombre del archivo origen: ";
    cin >> archivoOrigen;
    cout << "Ingrese el nombre del archivo destino: ";
    cin >> archivoDestino;
    
    // Abrir archivos
    ifstream origen(archivoOrigen);
    ofstream destino(archivoDestino);
    
    if (!origen.is_open()) {
        cout << "Error: No se pudo abrir el archivo origen." << endl;
        return 1;
    }
    
    if (!destino.is_open()) {
        cout << "Error: No se pudo crear el archivo destino." << endl;
        return 1;
    }
    
    string linea;
    bool banderaComentarioMultilinea = false;
    
    // Mientras origen tenga líneas
    while (getline(origen, linea)) {
        bool banderaString = false;
        bool banderaCaracter = false;
        string lineaResultado = "";
        
        // Si estamos en comentario multilinea desde antes, solo buscamos el fin
        if (banderaComentarioMultilinea) {
            for (int i = 0; i < linea.length(); i++) {
                if (linea[i] == '*' && i + 1 < linea.length() && linea[i+1] == '/') {
                    banderaComentarioMultilinea = false;
                    i++; // Saltar el /
                    // Continuar procesando el resto de la línea
                    for (int j = i + 1; j < linea.length(); j++) {
                        lineaResultado += linea[j];
                    }
                    break;
                }
            }
        } else {
            // Procesar línea normalmente
            for (int i = 0; i < linea.length(); i++) {
                char caracterActual = linea[i];
                
                // Detectar strings ""
                if (caracterActual == '"' && !banderaCaracter) {
                    if (i == 0 || linea[i-1] != '\\') {
                        banderaString = !banderaString;
                    }
                    lineaResultado += caracterActual;
                    continue;
                }
                
                // Detectar caracteres ''
                if (caracterActual == '\'' && !banderaString) {
                    if (i == 0 || linea[i-1] != '\\') {
                        banderaCaracter = !banderaCaracter;
                    }
                    lineaResultado += caracterActual;
                    continue;
                }
                
                // Si estamos dentro de un string o carácter, escribir todo
                if (banderaString || banderaCaracter) {
                    lineaResultado += caracterActual;
                    continue;
                }
                
                // Detectar inicio de comentario multilinea /*
                if (caracterActual == '/' && i + 1 < linea.length() && linea[i+1] == '*') {
                    banderaComentarioMultilinea = true;
                    i++; // Saltar el *
                    continue;
                }
                
                // Detectar comentario de línea //
                if (caracterActual == '/' && i + 1 < linea.length() && linea[i+1] == '/') {
                    // Ignorar el resto de la línea
                    break;
                }
                
                // Si llegamos aquí, es un carácter normal
                lineaResultado += caracterActual;
            }
        }
        
        // Verificar si la línea tiene contenido (no solo espacios)
        bool lineaTieneContenido = false;
        for (int i = 0; i < lineaResultado.length(); i++) {
            if (lineaResultado[i] != ' ' && lineaResultado[i] != '\t') {
                lineaTieneContenido = true;
                break;
            }
        }
        
        // Escribir en destino si la línea tiene contenido
        if (lineaTieneContenido) {
            destino << lineaResultado << endl;
        }
    }
    
    // Cerrar archivos
    origen.close();
    destino.close();
    
    cout << "Proceso completado. Archivo generado: " << archivoDestino << endl;
    
    return 0;
}
