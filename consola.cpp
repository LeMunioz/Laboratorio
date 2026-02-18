#include <iostream>
#include <cstdlib>
#include "Frontend/frontend.cpp"
using namespace std;

int main(){
char o;
cin>>o;
int d =92;
///////////////////////////////////////////////
//		CAJA IZQUIERDA    /////////////////////
///////////////////////////////////////////////
drawBox(0,0,50,43);
//mi nombre
color(12);
gotoxy(1,3); cout<<"    __   __   _   _    ====    __    ______";
gotoxy(1,4); cout<<"   |  \\ /  | | | | |  _    _  /   \\ |      |";
gotoxy(1,5); cout<<"   |   v   | | | | | |  \\ | | | | | --7   /";
gotoxy(1,6); cout<<"   |  \\ /  | |  U  | |   v  | | | |  /   /_";
gotoxy(1,7); cout<<"   |_|   |_| |_____| |_|\\___| \\___/  |_____|";


gotoxy(1,9); cout<<"      ====================================";
color(1);
gotoxy(1,15);cout<<" Escoge el sistema";
drawBox(20,11,29,11);
// -- ROW 12: Cabello superior (castaño, redondeado) ------------
color(6); // AMARILLO OCRE = pelo castaño
gotoxy(26,12); cout << ".-----------.";

// -- ROW 13: Clips amarillos + volumen lateral del pelo --------

color(6);
gotoxy(25,13); cout << "/ /";
gotoxy(28,13); cout << "__/_/  ";  // flequillo/pelo
gotoxy(35,13); cout << " \\ \\ \\";
gotoxy(28,14); cout << "/";

// -- ROW 14: Ojos chibi simples --------------------------------
color(6);
gotoxy(25,14); cout << "| |";
color(14);  // piel cara
gotoxy(28,14); cout << "  ";
// ojo izquierdo
color(7);   // GRIS OSCURO
gotoxy(30,14); cout << "(";
color(15);  // BLANCO - brillo
gotoxy(31,14); cout << "0";
color(7);
gotoxy(32,14); cout << ")";
color(14);
gotoxy(33,14); cout << "  ";
// ojo derecho
color(7);
gotoxy(35,14); cout << "(";
color(15);
gotoxy(36,14); cout << "0";
color(7);
gotoxy(37,14); cout << ")";
color(14);
gotoxy(38,14); cout << "  ";
color(6);
gotoxy(39,14); cout << "| |";

// -- ROW 15: Mejillas rosadas + nariz (minimalista) ------------
color(6);
gotoxy(25,15); cout << "| |";
color(12);  // ROJO CLARO - rubor mejilla izq
gotoxy(28,15); cout << ",,";
color(14);  // nariz - solo un punto
gotoxy(31,15); cout << "  .  ";
color(12);  // rubor mejilla der
gotoxy(36,15); cout << ",,";
color(14);
gotoxy(38,15); cout << "  ";
color(6);
gotoxy(39,15); cout << "| |";

// -- ROW 16: Boquita tierna ------------------------------------
color(6);
gotoxy(25,16); cout << "| |";
color(14);
gotoxy(28,16); cout << "    ";
color(15);  // BLANCO - boca
//gotoxy(32,16); cout << "(";
color(12);  // tono labio
gotoxy(33,16); cout << "V";
color(15);
//gotoxy(34,16); cout << ")";
color(14);
gotoxy(35,16); cout << "    ";
color(6);
gotoxy(39,16); cout << "| |";

// -- ROW 17: Menton redondo ------------------------------------
color(6);
gotoxy(25,17); cout << "\\ \\";
color(14);
gotoxy(28,17); cout << " \\";
gotoxy(30,17); cout << "_______";
gotoxy(37,17); cout << "/ ";
color(6);
gotoxy(39,17); cout << "/ /";

// -- ROW 18: Cuello + collar blanco ---------------------------
color(15);  // BLANCO - collar
gotoxy(30,18); cout << "/|";
gotoxy(32,18); cout << "===";
gotoxy(35,18); cout << "|\\";
color(14);  // piel cuello
gotoxy(32,18); cout << " = ";

// -- ROW 19: Chaqueta oscura (azul marino) ---------------------
color(1);   // AZUL REY - chaqueta
gotoxy(26,19); cout << "_/";
gotoxy(28,19); cout << "/ ";
color(15);  // BLANCO - camisa interior
gotoxy(30,19); cout << "|";
gotoxy(31,19); cout << "   ";
gotoxy(34,19); cout << "|";
color(1);   // chaqueta der
gotoxy(35,19); cout << " \\";
gotoxy(37,19); cout << "\\_";

// -- ROW 20: Cuerpo chaqueta -----------------------------------
color(1);   // AZUL REY
gotoxy(24,20); cout << "___/ ";
gotoxy(29,20); cout << "/";
color(15);  // camisa blanca centro
gotoxy(30,20); cout << "     ";
color(1);
gotoxy(35,20); cout << "\\";
gotoxy(36,20); cout << " \\___";
color(14); // CREMA - clips (pasadores)
gotoxy(25,13); cout << "cc";





///////////////////////////////////////////////
//    	CAJA DERECHA	  /////////////////////
///////////////////////////////////////////////
color(15);
drawBox(51,0,104,43);


//drawBox(52,1,102,28) ZONA DE DIBUJO 
/////////////////////////////////////////////
//DIBUJO  estara entre [52,1    154,1 ]
//                     [52,29   154,29]
/////////////////////////////////////////////
gotoxy(54,2);

for(int c=52; c<155; c++){// ciclo para rellenar el espacio de dibujo con un fondo azul, solo sirve para delimitar la caja y tener un fondo azul de cajon donde se requiera
	for(int f=1; f<30; f++){
		color(3); gotoxy(c,f); cout<<"#";
	} 
} 
// =============================================================
//   PAISAJE ASCII  -  zona: cols 52-154, rows 1-29
//   Estrategia: rellenar zona completa por capas, luego dibujar
//   encima sin usar espacios (gotoxy preciso por elemento)
// =============================================================
// ---------------------------------------------
// CAPA 1: FONDOS BASE POR ZONA
// ---------------------------------------------
// CIELO (rows 1-18): azul con tramas de '#'
color(9); // AZUL
for(int r = 1; r <= 18; r++){
    gotoxy(52, r);
    for(int c = 0; c < 103; c++) cout << "#";
}
// OCÉANO (rows 19-21): azul rey con '~'
color(1); // AZUL REY
for(int r = 19; r <= 21; r++){
    gotoxy(52, r);
    for(int c = 0; c < 103; c++) cout << "~";
}
// PLATAFORMA/PUENTE (rows 22-29): cyan con '+'
color(3); // CYAN
for(int r = 22; r <= 29; r++){
    gotoxy(52, r);
    for(int c = 0; c < 103; c++) cout << "+";
}
// ---------------------------------------------
// CAPA 2: PLANETA (esquina sup-izq, rows 1-8)
// ---------------------------------------------
// Relleno interior violeta
color(5); // VIOLETA
for(int r = 2; r <= 6; r++){
    int ini = 53 + (r < 4 ? 0 : 0);
    int ancho = 19;
    gotoxy(ini, r);
    for(int c = 0; c < ancho; c++) cout << "#";
}
gotoxy(56, 1); for(int c=0;c<12;c++) cout<<"#";
gotoxy(54, 7); for(int c=0;c<16;c++) cout<<"#";
// Textura del planeta (puntos de detalle sin espacios)
color(13); // MORADO
gotoxy(57,1); cout << "NNNNNNNN";
gotoxy(53,2); cout << "N.NN.N.NN.N.NNN.NNN";
gotoxy(53,3); cout << "NN.N.NN.N.NNN.N.NNN";
gotoxy(53,4); cout << "N.NNN.N.NN.N.NNN.NN";
gotoxy(53,5); cout << "NN.N.NNN.N.NN.N.NNN";
gotoxy(53,6); cout << "N.NN.N.NN.NNN.N.NNN";
gotoxy(55,7); cout << "NNNNNNNNNNNN";
// Contorno del planeta
color(15); // BLANCO
gotoxy(55,1); cout << "."; gotoxy(68,1); cout << ".";
gotoxy(53,2); cout << "("; gotoxy(71,2); cout << ")";
gotoxy(52,4); cout << "("; gotoxy(72,4); cout << ")";
gotoxy(53,6); cout << "("; gotoxy(71,6); cout << ")";
gotoxy(54,7); cout << "`"; gotoxy(70,7); cout << "'";
// Anillo del planeta (diagonal)
color(5); // VIOLETA
gotoxy(52,7); cout << ".~~~~~~~~~~~~~~~~~~~~~~~.";
// ---------------------------------------------
// CAPA 3: NUBES
// (dibujadas como bloques de 'N' sin espacios)
// ---------------------------------------------
// Nube izquierda (bajo el planeta)
color(7); // GRIS MUY CLARO
gotoxy(55,10); cout << "NNNNNNN";
gotoxy(54,11); cout << "NNNNNNNNN";
gotoxy(55,12); cout << "NNNNNNN";
// Nube centro-izquierda
color(15); // BLANCO
gotoxy(67,8);  cout << "NNNNNNNN";
gotoxy(66,9);  cout << "NNNNNNNNNN";
gotoxy(67,10); cout << "NNNNNNNN";
// Nube pequeña centro
color(7);
gotoxy(85,6); cout << "NNNNN";
gotoxy(84,7); cout << "NNNNNNN";
gotoxy(85,8); cout << "NNNNN";
// Nube grande derecha
color(15);
gotoxy(117,3); cout << "NNNNNNNNNNN";
gotoxy(115,4); cout << "NNNNNNNNNNNNNNN";
gotoxy(114,5); cout << "NNNNNNNNNNNNNNNNN";
gotoxy(115,6); cout << "NNNNNNNNNNNNNNN";
gotoxy(117,7); cout << "NNNNNNNNN";
// Nube pequeña derecha
color(7);
gotoxy(134,5); cout << "NNNNNN";
gotoxy(133,6); cout << "NNNNNNNN";
gotoxy(134,7); cout << "NNNNNN";
// ---------------------------------------------
// CAPA 4: MONTAÑAS IZQUIERDA (cols 52-76, rows 12-18)
// Triángulos rellenos con '#' gris, contorno blanco
// ---------------------------------------------
// -- Montaña izq: pico col 61 --
color(8); // GRIS
for(int r = 12; r <= 17; r++){
    int half = r - 12;
    int left = 61 - half;
    int right = 61 + half;
    gotoxy(left, r);
    for(int c = left; c <= right; c++) cout << "#";
}
// Contorno blanco montaña izq
color(15);
for(int r = 13; r <= 17; r++){
    int half = r - 12;
    gotoxy(61 - half, r); cout << "/";
    gotoxy(61 + half, r); cout << "\\";
}
gotoxy(61, 12); cout << "^"; // pico
// -- Montaña der (del grupo izq): pico col 70 --
color(8);
for(int r = 13; r <= 17; r++){
    int half = r - 13;
    int left = 70 - half;
    int right = 70 + half;
    gotoxy(left, r);
    for(int c = left; c <= right; c++) cout << "#";
}
color(15);
for(int r = 14; r <= 17; r++){
    int half = r - 13;
    gotoxy(70 - half, r); cout << "/";
    gotoxy(70 + half, r); cout << "\\";
}
gotoxy(70, 13); cout << "^";
// Base verde bajo montañas izq
color(10); // VERDE
gotoxy(52, 18);
for(int c = 0; c < 28; c++) cout << "~";
// ---------------------------------------------
// CAPA 4b: MONTAÑAS DERECHA (cols 128-154, rows 12-18)
// ---------------------------------------------
// -- Montaña izq del grupo der: pico col 136 --
color(8);
for(int r = 12; r <= 17; r++){
    int half = r - 12;
    int left = 136 - half;
    int right = min(136 + half, 154);
    if(left <= right){
        gotoxy(left, r);
        for(int c = left; c <= right; c++) cout << "#";
    }
}
color(15);
for(int r = 13; r <= 17; r++){
    int half = r - 12;
    gotoxy(136 - half, r); cout << "/";
    int rp = min(136 + half, 154);
    gotoxy(rp, r); cout << "\\";
}
gotoxy(136, 12); cout << "^";

// -- Montaña der del grupo der: pico col 147 --
color(8);
for(int r = 13; r <= 17; r++){
    int half = r - 13;
    int left = 147 - half;
    int right = min(147 + half, 154);
    if(left <= right){
        gotoxy(left, r);
        for(int c = left; c <= right; c++) cout << "#";
    }
}
color(15);
for(int r = 14; r <= 17; r++){
    int half = r - 13;
    gotoxy(147 - half, r); cout << "/";
    int rp = min(147 + half, 154);
    gotoxy(rp, r); cout << "\\";
}
gotoxy(147, 13); cout << "^";
// Base verde montañas der
color(10);
gotoxy(127, 18);
for(int c = 0; c < 28; c++) cout << "~";
// ---------------------------------------------
// CAPA 5: CIUDAD (cols 82-124, rows 9-17)
// Edificios rellenos, ventanas, detalles
// ---------------------------------------------
// ------ TORRE CENTRAL (pico col 105, rows 9-17) ------
// Relleno cuerpo
color(1); // AZUL REY
for(int r = 11; r <= 17; r++){
    gotoxy(102, r);
    for(int c = 0; c < 8; c++) cout << "#";
}
// Pico en V
color(11); // CELESTE
gotoxy(104, 9);  cout << "/\\";
gotoxy(103, 10); cout << "/##\\";
gotoxy(102, 11); cout << "/####\\";
// Ventanas
color(15);
gotoxy(102,11); cout << "|"; gotoxy(109,11); cout << "|";
for(int r = 12; r <= 16; r++){
    color(11);
    gotoxy(103, r); cout << "[]";
    gotoxy(106, r); cout << "[]";
    color(1);
    gotoxy(105, r); cout << "#";
}
color(15);
gotoxy(102,17); cout << "|------|";
// ------ EDIFICIO IZQUIERDO con pico (cols 91-96, rows 11-17) ------
color(3); // CYAN - relleno
for(int r = 12; r <= 17; r++){
    gotoxy(91, r);
    for(int c = 0; c < 6; c++) cout << "#";
}
// Pico
color(15);
gotoxy(93,11); cout << "/\\";
gotoxy(92,12); cout << "/##\\";
gotoxy(91,13); cout << "/####\\";
// Ventanas
color(11);
for(int r = 13; r <= 16; r++){
    gotoxy(92, r); cout << "[]";
    gotoxy(94, r); cout << "[]";
}
color(15);
gotoxy(91,17); cout << "|____|";
// ------ EDIFICIO DERECHO con pico (cols 110-115, rows 11-17) ------
color(3);
for(int r = 12; r <= 17; r++){
    gotoxy(110, r);
    for(int c = 0; c < 5; c++) cout << "#";
}
color(15);
gotoxy(111,11); cout << "/\\";
gotoxy(110,12); cout << "/##\\";
gotoxy(110,13); cout << "/###\\";
color(11);
for(int r = 13; r <= 16; r++){
    gotoxy(111, r); cout << "[]";
    gotoxy(113, r); cout << "[]";
}
color(15);
gotoxy(110,17); cout << "|___|";

// ------ TORRE DELGADA alta (cols 117-119, rows 9-17) ------
color(9); // AZUL
for(int r = 11; r <= 17; r++){
    gotoxy(117, r); cout << "###";
}
// Antena
color(15);
gotoxy(118,9);  cout << "|";
gotoxy(118,10); cout << "|";
gotoxy(117,11); cout << "[|]";
// Ventanas pequeñas
color(11);
gotoxy(117,12); cout << "[.]";
gotoxy(117,13); cout << "[.]";
gotoxy(117,14); cout << "[.]";
gotoxy(117,15); cout << "[.]";
gotoxy(117,16); cout << "[.]";
color(9);
gotoxy(117,17); cout << "###";
// ------ EDIFICIOS BAJOS IZQUIERDA (cols 83-89) ------
color(1); // AZUL REY
for(int r = 14; r <= 17; r++){
    gotoxy(83, r); cout << "####";
}
color(11);
gotoxy(83,14); cout << "[]##";
gotoxy(83,15); cout << "[]##";
gotoxy(83,16); cout << "[][]";
color(15);
gotoxy(83,17); cout << "|__|";
color(1);
for(int r = 15; r <= 17; r++){
    gotoxy(88, r); cout << "###";
}
color(11);
gotoxy(88,15); cout << "###";
gotoxy(88,16); cout << "[|]";
color(15);
gotoxy(88,17); cout << "|_|";
// ------ EDIFICIOS BAJOS DERECHA (cols 120-126) ------
color(1);
for(int r = 14; r <= 17; r++){
    gotoxy(120, r); cout << "####";
}
color(11);
gotoxy(120,14); cout << "##[]";
gotoxy(120,15); cout << "##[]";
gotoxy(120,16); cout << "[][]";
color(15);
gotoxy(120,17); cout << "|__|";

color(1);
for(int r = 15; r <= 17; r++){
    gotoxy(125, r); cout << "###";
}
color(11);
gotoxy(125,15); cout << "###";
gotoxy(125,16); cout << "[|]";
color(15);
gotoxy(125,17); cout << "|_|";
// ------ BASE ISLA blanca ------
color(15);
gotoxy(82, 18);
for(int c = 0; c < 44; c++) cout << "=";
// ---------------------------------------------
// CAPA 6: OCÉANO (rows 19-21) con tramas de ondas
// ---------------------------------------------
// Row 19: olas azul claro
color(11); // CELESTE
gotoxy(52, 19);
for(int c = 0; c < 103; c++) cout << (c % 5 < 2 ? '~' : '#');
// Row 20: trama mixta
color(9); // AZUL
gotoxy(52, 20);
for(int c = 0; c < 103; c++) cout << (c % 4 < 2 ? '~' : '#');
// Row 21: oscuro, más profundo
color(1); // AZUL REY
gotoxy(52, 21);
for(int c = 0; c < 103; c++) cout << (c % 5 < 3 ? '~' : '+');
// Reflejo torre en agua
color(11);
gotoxy(103, 19); cout << "||||";
gotoxy(103, 20); cout << "||||";
// ---------------------------------------------
// CAPA 7: PLATAFORMA Y PUENTE (rows 22-29)
// ---------------------------------------------
// Línea superior de plataforma (borde del agua)
color(15);
gotoxy(52, 22);
for(int c = 0; c < 103; c++) cout << "=";
// Superficie de la plataforma: trama cyan/verde
color(3); // CYAN
gotoxy(52, 23);
for(int c = 0; c < 103; c++) cout << (c % 3 == 0 ? '+' : '#');
// Interior de la plataforma entre pilares (rows 24-29)
// Fondo de color ya puesto, solo necesitamos los pilares
// PILARES DEL PUENTE (5 pilares, rellenos)
color(11); // CELESTE
int pilares[] = {60, 80, 100, 120, 140};
for(int i = 0; i < 5; i++){
    int px = pilares[i];
    // Cabeza del pilar
    gotoxy(px, 24); cout << "+--+";
    // Cuerpo relleno
    for(int r = 25; r <= 28; r++){
        gotoxy(px, r); cout << "|##|";
    }
    // Pie del pilar
    gotoxy(px, 29); cout << "+--+";
}
// SEPARADORES verticales de techo entre pilares
color(3);
for(int r = 24; r <= 29; r++){
    // zonas entre pilares se ven como techo interior
    for(int gap_start = 64; gap_start <= 143; gap_start += 20){
        gotoxy(gap_start, r);
        for(int c = 0; c < 16; c++)
            cout << (c % 5 == 0 ? '+' : '#');
    }
}
// ---------------------------------------------
// NAVE VOLANDO (detalle sobre ciudad)
// ---------------------------------------------
color(8); // GRIS
gotoxy(96, 10); cout << ">-<";

color(15);


/////////////////////////////////////////////////
//    SUBCAJAS INFERIORES 
/////////////////////////////////////////////////



// -------------------------------------------------------------
//  SUBCAJA IZQUIERDA: Monitor de procesos estilo htop
// -------------------------------------------------------------
//subcaja de caja derecha  [abajo izquierda]
color(15);
drawBox(52,30,57,12);
// -- Encabezado --
color(10); // VERDE PASTO
gotoxy(53,31); cout << "PID   USER    CPU% MEM%  COMMAND";

// -- Barra separadora --
color(8); // GRIS
gotoxy(53,32); cout << "--------------------------------------";

// -- Procesos (alternando colores para simular actividad) --
color(15); // BLANCO
gotoxy(53,33); cout << "1337  root    ";
color(12); // ROJO CLARO (proceso alto CPU)
cout << "98.2 ";
color(11); // CELESTE
cout << "12.4  ";
color(10);
cout << "[crypto_miner]";

color(15);
gotoxy(53,34); cout << "4096  daemon  ";
color(10); // VERDE
cout << "44.7 ";
color(11);
cout << " 8.1  ";
color(15);
cout << "netwatch -s 0.0.0.0";

color(15);
gotoxy(53,35); cout << "0x1A  root    ";
color(6); // AMARILLO OCRE
cout << "31.0 ";
color(11);
cout << " 5.3  ";
color(15);
cout << "/bin/sshd -D";

color(15);
gotoxy(53,36); cout << "2048  shadow  ";
color(10);
cout << "22.6 ";
color(11);
cout << " 3.9  ";
color(15);
cout << "perl exploit.pl -r";

color(15);
gotoxy(53,37); cout << "7331  root    ";
color(10);
cout << "17.4 ";
color(11);
cout << " 2.2  ";
color(15);
cout << "tcpdump -i eth0";

color(15);
gotoxy(53,38); cout << "9999  anon    ";
color(6);
cout << "11.3 ";
color(11);
cout << " 1.7  ";
color(15);
cout << "tor --RunAsDaemon 1";

color(15);
gotoxy(53,39); cout << "3141  www     ";
color(10);
cout << " 5.9 ";
color(11);
cout << " 0.8  ";
color(15);
cout << "python3 server.py";

// -- Barra de estado del sistema --
color(8);
gotoxy(53,40); cout << "--------------------------------------";
color(6); // AMARILLO OCRE
gotoxy(53,41); cout << "CPU[";
color(12);
cout << "|||||||||||||||";
color(8);
cout << ".....";
color(6);
cout << "] ";
color(10);
cout << "73%  MEM[";
color(11);
cout << "||||||";
color(8);
cout << ".........";
color(6);
cout << "]";
color(10);
cout << "31%";




// -------------------------------------------------------------
//  SUBCAJA DERECHA: Log del sistema / actividad de red
// -------------------------------------------------------------
color(15);
drawBox(110,30,44,12);

// -- Encabezado --
color(3); // CYAN
gotoxy(111,31); cout << "[ SYSTEM LOG - LIVE FEED ]";

// -- Barra separadora --
color(8);
gotoxy(111,32); cout << "-----------------------------";

// -- Líneas de log (estilo syslog con timestamps) --
color(8);   gotoxy(111,33); cout << "01:33:07 ";
color(12);  cout << "ALERT  ";
color(15);  cout << "port scan 443";

color(8);   gotoxy(111,34); cout << "01:33:09 ";
color(10);  cout << "INFO   ";
color(15);  cout << "ssh root@10.0.0.1";

color(8);   gotoxy(111,35); cout << "01:33:11 ";
color(12);  cout << "WARN   ";
color(15);  cout << "brute-force /dev/sda";

color(8);   gotoxy(111,36); cout << "01:33:14 ";
color(10);  cout << "INFO   ";
color(15);  cout << "tunnel 6881 <-> 443";

color(8);   gotoxy(111,37); cout << "01:33:17 ";
color(6);   cout << "DEBUG  ";
color(15);  cout << "DNS spoof resolved";

color(8);   gotoxy(111,38); cout << "01:33:20 ";
color(12);  cout << "CRIT   ";
color(15);  cout << "firewall bypassed!";

color(8);   gotoxy(111,39); cout << "01:33:22 ";
color(10);  cout << "INFO   ";
color(15);  cout << "payload injected OK";

// -- Prompt parpadeante al final --
color(8);
gotoxy(111,40); cout << "-----------------------------";
color(10); // VERDE
gotoxy(111,41); cout << "root@SHADOW:~# ";
color(15);
cout << "_ "; // cursor


color(0);
cin>>o;
}
