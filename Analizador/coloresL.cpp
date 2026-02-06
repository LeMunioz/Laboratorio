#include <Windows.h>
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
