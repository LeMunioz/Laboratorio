#include <iostream>
#include <cstdlib>
#include "Frontend/frontend.cpp"
using namespace std;

int main(){
char o;
cin>>o;
int d =92;
////////////////////////////
//		CAJA IZQUIERDA    //
////////////////////////////
drawBox(0,0,50,43);
//mi nombre
color(12);
gotoxy(1,3); cout<<"    __   __   _   _    ====    __    ______";
gotoxy(1,4); cout<<"   |  \\ /  | | | | |  _    _  /   \\ |      |";
gotoxy(1,5); cout<<"   |   v   | | | | | |  \\ | | | | | --7   /";
gotoxy(1,6); cout<<"   |  \\ /  | |  U  | |   v  | | | |  /   /_";
gotoxy(1,7); cout<<"   |_|   |_| |_____| |_|\\___| \\___/  |_____|";
color(15);
////////////////////////////
//    	CAJA DERECHA	  //
////////////////////////////
drawBox(51,0,104,43);

//drawBox(52,1,102,28);
//DIBUJO  estara entre [52,1    154,1 ]
//                     [52,29   154,29]
for(int c=52; c<155; c++){
	for(int f=1; f<30; f++){
		color(18); gotoxy(c,f); cout<<".";
	} 
} 
color(15);
/////////////////////////////////////////////////

//subacaa de caja derecha [arriba]

//subcaja de caja derecha  [abajo izquierda]
drawBox(52,30,57,12);

//subcaja de caja derecha  [abajo derecha]
drawBox(110,30,44,12);

color(0);
cin>>o;
}
