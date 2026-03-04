#include <iostream>
#include <cstdlib>
#include <string>
#include "Frontend/frontend.cpp"
using namespace std;

/*
JUEGO CONECTA 4
Angel Eduardo Muñoz Perez
*/

//////////////////////////////
//  VARIABLES
//////////////////////////////

char matriz [8][6];
int turno = 0;

//////////////////////////////
//  FUNCIONES
//////////////////////////////

void iniciarMatriz(char m[8][6]){
	for(int f=0; f<8; f++){
		for(int c=0; c<6; c++){
			m[f][c] = ' ';
		}
	}	
}

void dibujarMatriz(){
	//DIBUJO LA MATRIZ DEL CONECTA 4 UN POQUITO A LA DERECHA
	gotoxy(10,4); color(6);
	cout<<"[ ][ ][ ][ ][ ][ ][ ][ ]";
	gotoxy(10,5);
	cout<<"[ ][ ][ ][ ][ ][ ][ ][ ]";
	gotoxy(10,6);
	cout<<"[ ][ ][ ][ ][ ][ ][ ][ ]";
	gotoxy(10,7);	
	cout<<"[ ][ ][ ][ ][ ][ ][ ][ ]";
	gotoxy(10,8);
	cout<<"[ ][ ][ ][ ][ ][ ][ ][ ]";
	gotoxy(10,9);
	cout<<"[ ][ ][ ][ ][ ][ ][ ][ ]";
	gotoxy(10,10); color(1);
	cout<<"========================";
	color(15);
	
	/*Dibujo en el interior de cada casilla "[ ]" los valores de su matriz correspondiente
		cada casilla albarca e 3 espacios "[ ]" y empieza desde [10,4]
		el espacio intermedio de la primera casilla esta en 10+1=11 y de ahi se recorre de 3
		en 3. asi que uso la formula x = 11 + c * 3 para sacar la pocision de cada columna
	*/
	for(int f = 0; f < 6; f++){
        for(int c = 0; c < 8; c++){
            int x = 11 + c * 3;   //para que dinuje el contenido dentro de las casillas "[]"
            int y = 4  + f;
            gotoxy(x, y);
            cout << matriz[c][f]; 
        }
    }			
}

bool turnoPar(short turno){
	return turno % 2 == 0 ? true : false;
}

void ponerFichaAhi(string jugador, int columna){
	bool sigueCayendo = true;
	short filaActual =0;
	//poner la ficha arriba en esa casilla
	switch (jugador){
		case "jugador1":
			color(1);
			matriz [0][columna] = "X";
			color(15);
			//si cabe mas abajo, dibujar como va cayendo
			while(sigueCayendo){
				if(matriz[filaActual+1][columna] == " " && filaActual < 7){
					matriz[filaActual][columna] = " ";
					filaActual += 1;
					matriz[filaActual][columna] = "X";
				}else{
					break;
				}
			}
			break;	
		case "jugador2":
			color(4);
			matriz [0][columna] = "O";
			color(15);
			//si cabe mas abajo, dibujar como va cayendo
			while(sigueCayendo){
				if(matriz[filaActual+1][columna] == " " && filaActual < 7){
					matriz[filaActual][columna] = " ";
					filaActual += 1;
					matriz[filaActual][columna] = "O";
				}else{
					break;
				}
			break;
		default:
			cout<<"HUBO UN ERROR la columna escogida no tiene jugador asignado";
			break;
			
	}
	
}

void jugada(string jugador){
	cout<<"escoge en que lugar poner la ficha";
	gotoxy(10,3);
	Menu escogerColumna({" v "," v "," v "," v "," v "," v "," v "," v"},MenuTipo::Horizontal);
	int columnaEscogida = escogerColumna.mostrar();
	bool NoSePuedePonerAhi = true;
	
	while (NoSePuedePonerAhi){
		if(matriz[0][columnaEscogida] != " "){
			cout<<"ahi ya esta ocupado, escoge otro lado"<<endl;
		}else{
			switch(columnaEscogida){
				case 0:
					ponerFichaAhi (jugador, 0);		
					break:
				case 1:
					ponerFichaAhi (jugador, 1);
					break:
				case 2:
					ponerFichaAhi (jugador, 2);
					break:
				case 3:	
					ponerFichaAhi (jugador, 3);
					break:
				case 4:
					ponerFichaAhi (jugador, 4);
					break:
				case 5:
					ponerFichaAhi (jugador, 5);
					break:
				case 6:
					ponerFichaAhi (jugador, 6);
					break:
				case 7:
					ponerFichaAhi (jugador, 7);
					break:
				case 8:
					ponerFichaAhi (jugador, 8);
					break:
				default:
				
					break:	
			}
		}
		NosePuedePonerAhi = false;		
	}	
	turno ++;
}

bool checar4enLinea(){

}

//////////////////////////////////
//JUEGO CONTRA OTRO JUGADOR
//////////////////////////////////

void jugarJ(){
	bool noHayVictoria=true;
	system("cls");
	int jugadorGanador = 0;
	
	cout<<"entro a J";
	iniciarMatriz(matriz);
	
	while(noHayVictoria){
		dibujarMatriz();
		if(checar4enLinea()){
			switch(jugadorGanador){
				case 1: 
					Victoria("jugador 1");
					break;
				case 2:
					Victoria("jugador 2");
					break;
				default:
					cout<<"NO PUES NO SE COMO NO GANO 1 o 2";
					return 0;
					break;	
			}
		}else{
			if(turno == 48){
				color(2);
				cout<<"EMPATE"<<endl;
				return 0;
			}else{
				if (turnoPar()){
					jugada("jugador2");
				}else{
					jugada("jugador1");
				}
			}
		}
			
	}//fin de while 
}//fin de jugarJ()

//////////////////////////////////
//JUEGO CONTRA EL CPU
//////////////////////////////////

void JugarC(){
	cout<<"entro a C";
}

///////////////////////////
//   MAIN
///////////////////////////

int main(){
	color(3);
	gotoxy(6,2);
	cout<<"CONECTA 4";
	gotoxy(0,3);
	cout<<"=====================";
	color(8);
	gotoxy(1,4);
	cout<<"Hecho por Muñoz";
	
	drawBox(0,6,24,4);
	gotoxy(1,7);
	Menu mimenu({"Jugar contra otro","Jugar contra cpu ","salir"},MenuTipo::Vertical);
	int opcion = mimenu.mostrar();
	
	switch(opcion){
		case 0:
			jugarJ();
			break;
		case 1:
			JugarC();
			break;
		case 2:
			return 0;
			break;
		default:
			color(12);
			cout<<"COMO ÑONGAS LE HICISTE!! ESCOGE BIEN MENSO!!"<<endl;
			return 0;		
	}
	gotoxy(1,20);
	return 0;
}
