#include <Windows.h>

/*
COMO USAR
COLORES
	color(numero);  //todo lo que este despues se pintara del color correspondiente 
CORDENADAS
	gotoxy(x,y);	//el cursos se ubicara en es columna y renglon
TAMAÑO DE VENTANA   
	ajustarConsola(x, y); //la consola se pondra del tamaño en columnas y filas	

COLORES
[1]  AZUL REY
[2]  VERDE
[3]  AZUL CYAN
[4]  ROJO
[5]  VIOLETA
[6]  AMARILLO OCRE
[7]  GRIS MUY CLARO
[8]  GRIS
[9]  AZUL
[10] VERDE PASTO
[11] CELESTE
[12] ROJO CLARO
[13] MORADO
[14] CREMA
[15] BLANCO
[16]++ FONDO  
*/
void gotoxy(int x,int y){
	HANDLE hcon;
	hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD dwPos;
	
	dwPos.X = x;
	dwPos.Y = y;
	
	SetConsoleCursorPosition(hcon,dwPos);
}
void gotox(int x){
	HANDLE hcon;
	hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD dwPos;
	
	dwPos.X = x;
	
	SetConsoleCursorPosition(hcon,dwPos);
}

void color(int color){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleTextAttribute(hConsole, color);
}


// Función para ajustar la ventana de la consola
void ajustarConsola(int ancho, int alto) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD bufferSize;
    bufferSize.X = static_cast<SHORT>(ancho);
    bufferSize.Y = static_cast<SHORT>(alto);
    SetConsoleScreenBufferSize(hOut, bufferSize);

    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = static_cast<SHORT>(ancho - 1);
    windowSize.Bottom = static_cast<SHORT>(alto - 1);
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);

}
