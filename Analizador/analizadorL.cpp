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

// Funciones para detectar tipos de comentarios
bool esComentarioLinea(string linea, int posicion) {
    return (posicion + 1 < linea.length() && 
            linea[posicion] == '/' && 
            linea[posicion + 1] == '/');
}
bool esComentarioMultilineaInicio(string linea, int posicion) {
    return (posicion + 1 < linea.length() && 
            linea[posicion] == '/' && 
            linea[posicion + 1] == '*');
}
bool esComentarioMultilineaFin(string linea, int posicion) {
    return (posicion + 1 < linea.length() && 
            linea[posicion] == '*' && 
            linea[posicion + 1] == '/');
}

// Funcion para verificar si un caracter es escape
bool esCaracterEscape(string linea, int posicion) {
    return (posicion > 0 && linea[posicion - 1] == '\\');
}

// Funcion para verificar si una linea tiene contenido real
bool lineaTieneContenido(string linea) {
    for (int i = 0; i < linea.length(); i++) {
        if (linea[i] != ' ' && linea[i] != '\t') {
            return true;
        }
    }
    return false;
}

// Funcion para procesar linea cuando hay comentario multilinea activo
string procesarLineaConComentarioMultilinea(string linea, bool &banderaComentarioMultilinea) {
    string resultado = "";
    
    for (int i = 0; i < linea.length(); i++) {
        if (esComentarioMultilineaFin(linea, i)) {
            banderaComentarioMultilinea = false;
            i++; // Saltar el siguiente carácter (/)
            // Continuar procesando el resto de la linea
            for (int j = i + 1; j < linea.length(); j++) {
                resultado += linea[j];
            }
            break;
        }
    }
    
    return resultado;
}

// Funcion principal para procesar una línea
string procesarLinea(string linea, bool &banderaComentarioMultilinea) {
    // Si ya estamos en comentario multilinea, usar funcion especial
    if (banderaComentarioMultilinea) {
        return procesarLineaConComentarioMultilinea(linea, banderaComentarioMultilinea);
    }
    
    bool banderaString = false;
    bool banderaCaracter = false;
    string lineaResultado = "";
    
    for (int i = 0; i < linea.length(); i++) {
        char caracterActual = linea[i];
        
        switch (caracterActual) {
            case '"':
                // Detectar strings
                if (!banderaCaracter && !esCaracterEscape(linea, i)) {
                    banderaString = !banderaString;
                }
                lineaResultado += caracterActual;
                break;
                
            case '\'':
                // Detectar caracteres
                if (!banderaString && !esCaracterEscape(linea, i)) {
                    banderaCaracter = !banderaCaracter;
                }
                lineaResultado += caracterActual;
                break;
                
            case '/':
                // Verificar si estamos dentro de string o carácter
                if (banderaString || banderaCaracter) {
                    lineaResultado += caracterActual;
                } else {
                    // Verificar tipo de comentario
                    if (esComentarioMultilineaInicio(linea, i)) {
                        banderaComentarioMultilinea = true;
                        i++; // Saltar el siguiente carácter (*)
                    } else if (esComentarioLinea(linea, i)) {
                        // Terminar procesamiento de esta línea
                        return lineaResultado;
                    } else {
                        // Es solo un caracter /
                        lineaResultado += caracterActual;
                    }
                }
                break;
                
            default:
                // Cualquier otro carácter
                if (banderaString || banderaCaracter) {
                    lineaResultado += caracterActual;
                } else {
                    lineaResultado += caracterActual;
                }
                break;
        }
    }
    
    return lineaResultado;
}

// FUNCIONES PARA ARBIR Y CERRAR ARCHIVO
bool abrirArchivoOrigen(ifstream &archivo, string nombreArchivo) {
    archivo.open(nombreArchivo);
    return archivo.is_open();
}
bool abrirArchivoDestino(ofstream &archivo, string nombreArchivo) {
    archivo.open(nombreArchivo);
    return archivo.is_open();
}

void solicitarNombresArchivos(string &archivoOrigen, string &archivoDestino) {
    cout << "Ingrese el nombre del archivo origen: ";
    cin >> archivoOrigen;
    cout << "Ingrese el nombre del archivo destino: ";
    cin >> archivoDestino;
}

void cerrarArchivos(ifstream &origen, ofstream &destino) {
    origen.close();
    destino.close();
}

int main() {
    string archivoOrigen, archivoDestino;
    ifstream origen;
    ofstream destino;
    
    // Solicitar nombres de archivos
    solicitarNombresArchivos(archivoOrigen, archivoDestino);

    if (!abrirArchivoOrigen(origen, archivoOrigen)) {
        cout << "Error: No se pudo abrir el archivo origen." << endl;
        return 1;
    }
    
    if (!abrirArchivoDestino(destino, archivoDestino)) {
        cout << "Error: No se pudo crear el archivo destino." << endl;
        return 1;
    }
    
    string linea;
    bool banderaComentarioMultilinea = false;
    
    // Procesar cada linea del archivo
    while (getline(origen, linea)) {
        string lineaResultado = procesarLinea(linea, banderaComentarioMultilinea);
        
        // Escribir en destino si la linea tiene contenido
        if (lineaTieneContenido(lineaResultado)) {
            destino << lineaResultado << endl;
        }
    }
    // Cerrar archivos
    cerrarArchivos(origen, destino);
    
    cout << "Proceso completado. Archivo generado: " << archivoDestino << endl;
    
    return 0;
}
