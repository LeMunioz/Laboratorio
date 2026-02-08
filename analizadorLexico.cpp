#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class AnalizadorLexico {
private:
    string contenido;
    int posicion;
    
    // Verifica si estamos dentro de un string
    bool dentroDeString(const string& linea, int pos) {
        bool enString = false;
        bool escapeActivo = false;
        
        for (int i = 0; i < pos; i++) {
            if (escapeActivo) {
                escapeActivo = false;
                continue;
            }
            
            if (linea[i] == '\\') {
                escapeActivo = true;
                continue;
            }
            
            if (linea[i] == '"') {
                enString = !enString;
            }
        }
        return enString;
    }
    
    // Verifica si estamos dentro de un caracter
    bool dentroDeChar(const string& linea, int pos) {
        bool enChar = false;
        bool escapeActivo = false;
        
        for (int i = 0; i < pos; i++) {
            if (escapeActivo) {
                escapeActivo = false;
                continue;
            }
            
            if (linea[i] == '\\') {
                escapeActivo = true;
                continue;
            }
            
            if (linea[i] == '\'') {
                enChar = !enChar;
            }
        }
        return enChar;
    }
    
public:
    // Constructor
    AnalizadorLexico() {
        posicion = 0;
    }
    
    // Lee el archivo de entrada
    bool leerArchivo(const string& nombreArchivo) {
        ifstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            cout << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
            return false;
        }
        
        string linea;
        contenido = "";
        
        while (getline(archivo, linea)) {
            contenido += linea + "\n";
        }
        
        archivo.close();
        return true;
    }
    
    // Procesa el contenido y quita los comentarios
    string quitarComentarios() {
        string resultado = "";
        bool enComentarioBloque = false;
        
        for (int i = 0; i < contenido.length(); i++) {
            // Si estamos en comentario de bloque
            if (enComentarioBloque) {
                // Buscar el cierre */
                if (i < contenido.length() - 1 && 
                    contenido[i] == '*' && contenido[i + 1] == '/') {
                    enComentarioBloque = false;
                    i++; // Saltar el '/'
                }
                continue;
            }
            
            // Verificar si estamos en string o char
            string lineaActual = "";
            int inicioLinea = i;
            while (inicioLinea > 0 && contenido[inicioLinea - 1] != '\n') {
                inicioLinea--;
            }
            for (int j = inicioLinea; j <= i; j++) {
                lineaActual += contenido[j];
            }
            
            int posEnLinea = lineaActual.length() - 1;
            
            // Verificar comentario de línea //
            if (i < contenido.length() - 1 && 
                contenido[i] == '/' && contenido[i + 1] == '/') {
                
                if (!dentroDeString(lineaActual, posEnLinea) && 
                    !dentroDeChar(lineaActual, posEnLinea)) {
                    // Saltar hasta el final de la línea
                    while (i < contenido.length() && contenido[i] != '\n') {
                        i++;
                    }
                    i--; // Retroceder uno para que el \n se procese normalmente
                    continue;
                }
            }
            
            // Verificar comentario de bloque /*
            if (i < contenido.length() - 1 && 
                contenido[i] == '/' && contenido[i + 1] == '*') {
                
                if (!dentroDeString(lineaActual, posEnLinea) && 
                    !dentroDeChar(lineaActual, posEnLinea)) {
                    enComentarioBloque = true;
                    i++; // Saltar el '*'
                    continue;
                }
            }
            
            // Agregar el caracter al resultado
            resultado += contenido[i];
        }
        
        return resultado;
    }
    
    // Elimina líneas vacías del resultado
    string eliminarLineasVacias(const string& texto) {
        string resultado = "";
        string linea = "";
        
        for (int i = 0; i < texto.length(); i++) {
            if (texto[i] == '\n') {
                // Verificar si la línea tiene contenido (no solo espacios)
                bool tieneContenido = false;
                for (char c : linea) {
                    if (c != ' ' && c != '\t') {
                        tieneContenido = true;
                        break;
                    }
                }
                
                if (tieneContenido) {
                    resultado += linea + "\n";
                }
                linea = "";
            } else {
                linea += texto[i];
            }
        }
        
        // Agregar la última línea si tiene contenido
        if (!linea.empty()) {
            bool tieneContenido = false;
            for (char c : linea) {
                if (c != ' ' && c != '\t') {
                    tieneContenido = true;
                    break;
                }
            }
            if (tieneContenido) {
                resultado += linea;
            }
        }
        
        return resultado;
    }
    
    // Guarda el resultado en un archivo
    bool guardarArchivo(const string& nombreArchivo, const string& contenido) {
        ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            cout << "Error: No se pudo crear el archivo " << nombreArchivo << endl;
            return false;
        }
        
        archivo << contenido;
        archivo.close();
        return true;
    }
};

int main() {
    AnalizadorLexico analizador;
    
    string archivoEntrada, archivoSalida;
    
    cout << "=== Analizador Lexico - Eliminador de Comentarios ===" << endl;
    cout << "Ingrese el nombre del archivo de entrada (.txt o .cpp): ";
    cin >> archivoEntrada;
    
    // Leer el archivo
    if (!analizador.leerArchivo(archivoEntrada)) {
        return 1;
    }
    
    cout << "Ingrese el nombre del archivo de salida: ";
    cin >> archivoSalida;
    
    // Procesar: quitar comentarios
    string sinComentarios = analizador.quitarComentarios();
    
    // Eliminar líneas vacías
    string resultado = analizador.eliminarLineasVacias(sinComentarios);
    
    // Guardar el resultado
    if (analizador.guardarArchivo(archivoSalida, resultado)) {
        cout << "Proceso completado exitosamente!" << endl;
        cout << "Archivo generado: " << archivoSalida << endl;
    }
    
    return 0;
}
