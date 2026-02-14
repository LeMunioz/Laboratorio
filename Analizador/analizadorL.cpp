#include <iostream>
#include <fstream>
#include <string>
#include "colores.cpp"
using namespace std;

/*
ANALIZADOR LEXICO v1 [Comentarios y espacios vacios]
Angel Eduardo Muñoz Perez
Febrero 2026
ING COMPUTACION _ CUALTOS
Compiladores Prof. Meño

FUNCIONAMIENTO
==== Analizador Lexico
	 Le pide al usuario el nombre del archivo en > origen y la compilacion la escribe en > destino
		PARTE 1 LIMPIADOR DE COMENTARIOS
			mientras haya lineas en origen, entra al archivo, por cada linea entra a un for para 
			recorrer la linea, entra a un switch para ver que tipo de texto hay, checa si se encuentra
			un comentario o si esta dentro de un string o caracter, todo utilizando banderas, cada bandera
			se activa y desactiva inviertiendo su valor cuando entra al caso y estas se usan para verificar
			en donde se encuentra el caracter y distinguir si es un comentario, si lo es fijarse si esta 
			dentro de una cadena o no, para ver si lo escribe en destino, y si no hay ninguna bandera, por 
			default escribir tambien el caracter en destino, solo se hace este proceso por cada linea que si
			tenga contenido que verifica con un for dentro de lineaTieneContenido()
*/


// ------------------------------------------------- //
//    FUNCIONES DE LIMPIADOR DE COMENTARIOS
// ------------------------------------------------- //
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

// Funcion principal para procesar una línea
string procesarLinea(string linea, bool &banderaComentarioMultilinea) {
    string resultado = "";
    
    // Si ya estamos en comentario multilinea, buscar el fin
    if (banderaComentarioMultilinea) {
        for (int i = 0; i < linea.length(); i++) {
            if (esComentarioMultilineaFin(linea, i)) {
                banderaComentarioMultilinea = false;
                i++; // Saltar el siguiente caracter (/)
                // Continuar procesando el resto de la linea
                for (int j = i + 1; j < linea.length(); j++) {
                    resultado += linea[j];
                }
                break;
            }
        }
        return resultado;
    }
    
    // Procesar linea normalmente
    bool banderaString = false;
    bool banderaCaracter = false;
    
    for (int i = 0; i < linea.length(); i++) {
        char caracterActual = linea[i];
        
        switch (caracterActual) {
            case '"':
                // Detectar strings
                if (!banderaCaracter && !esCaracterEscape(linea, i)) {
                    banderaString = !banderaString;
                }
                resultado += caracterActual;
                break;
                
            case '\'':
                // Detectar caracteres
                if (!banderaString && !esCaracterEscape(linea, i)) {
                    banderaCaracter = !banderaCaracter;
                }
                resultado += caracterActual;
                break;
                
            case '/':
                // Verificar si estamos dentro de string o caracter
                if (banderaString || banderaCaracter) {
                    resultado += caracterActual;
                } else {
                    // Verificar tipo de comentario
                    if (esComentarioMultilineaInicio(linea, i)) {
                        banderaComentarioMultilinea = true;
                        i++; // Saltar el siguiente caracter (*)
                    } else if (esComentarioLinea(linea, i)) {
                        // Terminar procesamiento de esta línea
                        return resultado;
                    } else {
                        // Es solo un caracter /
                        resultado += caracterActual;
                    }
                }
                break;
                
            default:
                // Cualquier otro carácter
                resultado += caracterActual;
                break;
        }
    }
    
    return resultado;
}// fin de procesearLinea ////////////////////////////////////////////////

int main() {
    string archivoOrigen, archivoDestino;
    
    // PEDIR ARCHIVOS AL USUARIO
    bool pidioCompilarAlgoInseguro = true;
    while(pidioCompilarAlgoInseguro){
    	color(3);
	    cout << "Ingrese el nombre del archivo origen: ";
	    color(7);
		cin >> archivoOrigen;
		if (archivoOrigen == "AnalizadorL.cpp"){
			color(12);
			cout << "ESTIMADO!!!! NO VUELVAS A COMETER EL MISMO ERROR, ESCOGE ALGO QUE NO SEA EL MISMO COMPILADOR PARA COMPILARLO >:{"<<endl;
			continue;
		}else{
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
    
    string linea;
    bool banderaComentarioMultilinea = false;
    
    // PROCESAR LAS LINEAS Y BORRAR COMENTARIOS
    while (getline(origen, linea)) {
        string lineaResultado = procesarLinea(linea, banderaComentarioMultilinea);
        // Escribir en destino si la linea esta lista
        if (lineaTieneContenido(lineaResultado)) {
            destino << lineaResultado << endl;
        }
    }
    
    // Cerrar archivos
    origen.close();
    destino.close();
    
    color(2);
    cout << "Proceso completado. Archivo generado: " << archivoDestino << endl;
    color(15);
    return 0;
}// fin de main ////////////////////////////////////
