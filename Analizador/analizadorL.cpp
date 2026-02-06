#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    string archivoOrigen;
    string archivoDestino;

    cout << "Nombre del archivo de entrada (.txt o .cpp): ";
    getline(cin, archivoOrigen);

    ifstream entrada(archivoOrigen);

    if (!entrada.is_open()) {
        cout << "No se pudo abrir el archivo de origen.\n";
        return 1;
    }

    cout << "Nombre del archivo de salida: ";
    getline(cin, archivoDestino);

    ofstream salida(archivoDestino);

    if (!salida.is_open()) {
        cout << "No se pudo crear el archivo de salida.\n";
        return 1;
    }

    string linea;

    // Copiar todo el contenido
    while (getline(entrada, linea)) {
        salida << linea << endl;
    }

    entrada.close();
    salida.close();

    cout << "Archivo copiado correctamente.\n";

    return 0;
}

