#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include "Frontend/frontend.cpp"
using namespace std;
int main() {
	setConsoleSize(120, 30);
	system("cls");
    cout << "=== PRUEBA DE COLORES ===\n";
    color(10); cout << "Texto verde pasto\n";
    color(12); cout << "Texto rojo claro\n";
    color(11); cout << "Texto celeste\n";
    color(15);
    cout << "\nPresiona una tecla para continuar...";
    getch();
    system("cls");
    cout << "=== PRUEBA DE COORDENADAS ===\n";
    gotoxy(5, 5);
    cout << "Esto aparece en (5,5)\n";
    gotoxy(10, 7);
    cout << "Esto aparece en (10,7)\n";
    gotox(20);
    cout << "Esto aparece en la columna 20\n";
    cout << "\n\nPresiona una tecla para continuar...";
    getch();
    system("cls");
    cout << "=== PRUEBA DE VALIDACIÓN ===\n";
    cout << "Ingresa un numero del 1 al 5: ";
    int num = leerOpcion(1, 5);
    cout << "Ingresaste: " << num << "\n";
    cout << "\nIngresa un caracter (s/n): ";
    char c = leerCharOpcional({'s', 'n'});
    cout << "Elegiste: " << c << "\n";
    cout << "\nPresiona una tecla para continuar...";
    getch();
    system("cls");
    cout<< "Prueba de menus con flechas, usa las felchas y presiona enter"<<endl;
    gotoxy(2,4);
	Menu menuPrueba(
        { "Opcion 1", "Opcion 2", "Opcion 3", "Salir" },
        MenuTipo::Auto
    );
    int opcion = menuPrueba.mostrar();
    system("cls");
    cout << "Elegiste: " << opcion << "\n";
    cout << "(0 = Opcion 1, 1 = Opcion 2, ...)\n";
    getch();
	bool salir = true;
	while (salir){
	char teclaSalir;
	cin>>teclaSalir;
		if(teclaSalir == 'q'){
			salir = false;
		}else{
		drawBox(20, 5, 60, 15);
		escribirAnimado(25, 7, "Mini Framework de Consola");
		Menu menu({"Iniciar", "Opciones", "Salir"});
		menu.setPos(30, 12);
		int sel = menu.mostrar();
		gotoxy(30, 20);
		cout << "Seleccionaste: " << menu.obtenerSeleccion();
		}
	}
}
