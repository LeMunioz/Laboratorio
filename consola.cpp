
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
drawBox(0,0,50,45);
//mi nombre
color(12);
gotoxy(1,3); cout<<"    __   __   _   _    ====    __    ______";
gotoxy(1,4); cout<<"   |  \\ /  | | | | |  _    _  /   \\ |      |";
gotoxy(1,5); cout<<"   |   v   | | | | | |  \\ | | | | | --7   /";
gotoxy(1,6); cout<<"   |  \\ /  | |  U  | |   v  | | | |  /   /_";
gotoxy(1,7); cout<<"   |_|   |_| |_____| |_|\\___| \\___/  |_____|";

gotoxy(1,9); cout<<"      ====================================";
color(3);
gotoxy(1,14);cout<<"       Escoge el";
gotoxy(1,15);cout<<"   sistema donde";
gotoxy(1,16);cout<<" gustes arrancar";

// -- DIBUJO DE LA YUI ------------
gotoxy(21,11);
color(6); cout<<"     -= ``` == _";
gotoxy(21,12);
          cout<<"   /             \\";
gotoxy(21,13);
          cout<<"  |   /|  |  \\     \\ ";
gotoxy(26,13);
color(14);cout<<"==";
color(6);
gotoxy(21,14);
          cout<<"  /    -\\ /  = \\|   |";
gotoxy(26,14);
color(14);cout<<"==";
color(6);
gotoxy(21,15);
          cout<<" |   |          | | \\";
gotoxy(29,15);
color(15);cout<<"0     0";
color(6);
gotoxy(21,16);
          cout<<" |   |          | \\_|";
gotoxy(28,16);
color(12);cout<<"##     ##";
color(6);
gotoxy(21,17);
          cout<<"  \\/| \\         \\ /\\/";
gotoxy(21,18);
          cout<<"     \\/";
gotoxy(29,18);
color(14);cout<<"`--__--``";
color(15);

///////////////////////////////////////////////
//    	CAJA DERECHA	  /////////////////////
///////////////////////////////////////////////
color(15);
drawBox(51,0,104,45);

// Relleno fondo (SOLO rows 1-26 ahora - acortado 3 filas)
for(int c=52; c<155; c++){
    for(int f=1; f<27; f++){
        color(3); gotoxy(c,f); cout<<"#";
    }
}

// =============================================================
//   CIELO (rows 1-18)
// =============================================================
color(9);
for(int r = 1; r <= 18; r++){
    gotoxy(52, r);
    for(int c = 0; c < 103; c++) cout << "#";
}

// =============================================================
//   OCÉANO (rows 19-21)
// =============================================================
color(1);
for(int r = 19; r <= 21; r++){
    gotoxy(52, r);
    for(int c = 0; c < 103; c++) cout << "~";
}

// =============================================================
//   PLATAFORMA BASE (rows 22-26 - acortado desde 29)
// =============================================================
color(3);
for(int r = 22; r <= 26; r++){
    gotoxy(52, r);
    for(int c = 0; c < 103; c++) cout << "+";
}

// =============================================================
//   PLANETA (esquina sup-izq)
// =============================================================
color(5);
for(int r = 2; r <= 6; r++){
    gotoxy(53, r);
    for(int c = 0; c < 19; c++) cout << "#";
}
gotoxy(56, 1); for(int c=0;c<12;c++) cout<<"#";
gotoxy(54, 7); for(int c=0;c<16;c++) cout<<"#";
color(13);
gotoxy(57,1); cout << "NNNNNNNN";
gotoxy(53,2); cout << "N.NN.N.NN.N.NNN.NNN";
gotoxy(53,3); cout << "NN.N.NN.N.NNN.N.NNN";
gotoxy(53,4); cout << "N.NNN.N.NN.N.NNN.NN";
gotoxy(53,5); cout << "NN.N.NNN.N.NN.N.NNN";
gotoxy(53,6); cout << "N.NN.N.NN.NNN.N.NNN";
gotoxy(55,7); cout << "NNNNNNNNNNNN";
color(15);
gotoxy(55,1); cout << "."; gotoxy(68,1); cout << ".";
gotoxy(53,2); cout << "("; gotoxy(71,2); cout << ")";
gotoxy(52,4); cout << "("; gotoxy(72,4); cout << ")";
gotoxy(53,6); cout << "("; gotoxy(71,6); cout << ")";
gotoxy(54,7); cout << "`"; gotoxy(70,7); cout << "'";
color(5);
gotoxy(52,7); cout << ".~~~~~~~~~~~~~~~~~~~~~~~.";

// =============================================================
//   NUBES MEJORADAS (formas redondeadas con ( ) . ` ' ~)
// =============================================================

// Nube 1 - bajo planeta (rows 10-12)
color(7);
gotoxy(54,10); cout << ".~~~~~~~.";
gotoxy(53,11); cout << "(~NNNNNNN~)";
gotoxy(54,12); cout << "`~~~~~~~'";

// Nube 2 - centro-izquierda (rows 8-10)
color(15);
gotoxy(65,8);  cout << ".~~~~~~~~~.";
gotoxy(64,9);  cout << "(~NNNNNNNNN~)";
gotoxy(65,10); cout << "`~~~~~~~~~'";

// Nube 3 - pequeña centro (rows 6-8)
color(7);
gotoxy(84,6); cout << ".~~~~~~.";
gotoxy(83,7); cout << "(~NNNN~)";
gotoxy(84,8); cout << "`~~~~~~'";

// Nube 4 - grande derecha (rows 3-7)
color(15);
gotoxy(117,3); cout << ".~~.~~~.~~.";
gotoxy(114,4); cout << ".(~NNNNNNNNN~).";
gotoxy(113,5); cout << "(~~NNNNNNNNNNN~~)";
gotoxy(114,6); cout << "`(~NNNNNNNNN~)'";
gotoxy(117,7); cout << "`~~~.~~~'";

// Nube 5 - pequeña derecha (rows 5-7)
color(7);
gotoxy(133,5); cout << ".~~~~~~.";
gotoxy(132,6); cout << "(~NNNN~)";
gotoxy(133,7); cout << "`~~~~~~'";

// =============================================================
//   MONTAÑAS IZQUIERDA (cols 52-79, rows 12-18)
// =============================================================
color(8);
for(int r = 12; r <= 17; r++){
    int half = r - 12;
    int left = 61 - half;
    int right = 61 + half;
    gotoxy(left, r);
    for(int c = left; c <= right; c++) cout << "#";
}
color(15);
for(int r = 13; r <= 17; r++){
    int half = r - 12;
    gotoxy(61 - half, r); cout << "/";
    gotoxy(61 + half, r); cout << "\\";
}
gotoxy(61, 12); cout << "^";

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

// Base verde montañas izq
color(10);
gotoxy(52, 18);
for(int c = 0; c < 28; c++) cout << "~";

// =============================================================
//   MONTAÑAS DERECHA (cols 128-154, rows 12-18)
// =============================================================
color(8);
for(int r = 12; r <= 17; r++){
    int half = r - 12;
    int left = 136 - half;
    int right = min(136 + half, 154);
    if(left <= right){ gotoxy(left, r); for(int c = left; c <= right; c++) cout << "#"; }
}
color(15);
for(int r = 13; r <= 17; r++){
    int half = r - 12;
    gotoxy(136 - half, r); cout << "/";
    int rp = min(136 + half, 154);
    gotoxy(rp, r); cout << "\\";
}
gotoxy(136, 12); cout << "^";

color(8);
for(int r = 13; r <= 17; r++){
    int half = r - 13;
    int left = 147 - half;
    int right = min(147 + half, 154);
    if(left <= right){ gotoxy(left, r); for(int c = left; c <= right; c++) cout << "#"; }
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

// =============================================================
//   CIUDAD (cols 82-126, rows 9-17) - sin cambios
// =============================================================
// Torre central (pico col 105, rows 9-17)
color(1);
for(int r = 11; r <= 17; r++){
    gotoxy(102, r);
    for(int c = 0; c < 8; c++) cout << "#";
}
color(11);
gotoxy(104, 9);  cout << "/\\";
gotoxy(103, 10); cout << "/##\\";
gotoxy(102, 11); cout << "/####\\";
color(15);
gotoxy(102,11); cout << "|"; gotoxy(109,11); cout << "|";
for(int r = 12; r <= 16; r++){
    color(11); gotoxy(103, r); cout << "[]";
    gotoxy(106, r); cout << "[]";
    color(1); gotoxy(105, r); cout << "#";
}
color(15); gotoxy(102,17); cout << "|------|";

// Edificio izquierdo
color(3);
for(int r = 12; r <= 17; r++){ gotoxy(91, r); for(int c = 0; c < 6; c++) cout << "#"; }
color(15);
gotoxy(93,11); cout << "/\\";
gotoxy(92,12); cout << "/##\\";
gotoxy(91,13); cout << "/####\\";
color(11);
for(int r = 13; r <= 16; r++){ gotoxy(92, r); cout << "[]"; gotoxy(94, r); cout << "[]"; }
color(15); gotoxy(91,17); cout << "|____|";

// Edificio derecho
color(3);
for(int r = 12; r <= 17; r++){ gotoxy(110, r); for(int c = 0; c < 5; c++) cout << "#"; }
color(15);
gotoxy(111,11); cout << "/\\";
gotoxy(110,12); cout << "/##\\";
gotoxy(110,13); cout << "/###\\";
color(11);
for(int r = 13; r <= 16; r++){ gotoxy(111, r); cout << "[]"; gotoxy(113, r); cout << "[]"; }
color(15); gotoxy(110,17); cout << "|___|";

// Torre delgada alta
color(9);
for(int r = 11; r <= 17; r++){ gotoxy(117, r); cout << "###"; }
color(15);
gotoxy(118,9);  cout << "|";
gotoxy(118,10); cout << "|";
gotoxy(117,11); cout << "[|]";
color(11);
for(int r = 12; r <= 16; r++){ gotoxy(117, r); cout << "[.]"; }
color(9); gotoxy(117,17); cout << "###";

// Edificios bajos izquierda
color(1);
for(int r = 14; r <= 17; r++){ gotoxy(83, r); cout << "####"; }
color(11);
gotoxy(83,14); cout << "[]##";
gotoxy(83,15); cout << "[]##";
gotoxy(83,16); cout << "[][]";
color(15); gotoxy(83,17); cout << "|__|";
color(1);
for(int r = 15; r <= 17; r++){ gotoxy(88, r); cout << "###"; }
color(11);
gotoxy(88,15); cout << "###";
gotoxy(88,16); cout << "[|]";
color(15); gotoxy(88,17); cout << "|_|";

// Edificios bajos derecha
color(1);
for(int r = 14; r <= 17; r++){ gotoxy(120, r); cout << "####"; }
color(11);
gotoxy(120,14); cout << "##[]";
gotoxy(120,15); cout << "##[]";
gotoxy(120,16); cout << "[][]";
color(15); gotoxy(120,17); cout << "|__|";
color(1);
for(int r = 15; r <= 17; r++){ gotoxy(125, r); cout << "###"; }
color(11);
gotoxy(125,15); cout << "###";
gotoxy(125,16); cout << "[|]";
color(15); gotoxy(125,17); cout << "|_|";

// Base isla blanca
color(15);
gotoxy(82, 18);
for(int c = 0; c < 44; c++) cout << "=";

// =============================================================
//   ÁRBOLES - IZQUIERDA (entre montañas y ciudad)
// =============================================================
// Árbol 1 - pino grande (col 75)
color(10);
gotoxy(76,13); cout << "*";
gotoxy(75,14); cout << "***";
gotoxy(74,15); cout << "*****";
gotoxy(75,16); cout << "***";
color(6);
gotoxy(76,17); cout << "||";

// Árbol 2 - pino pequeño (col 80)
color(2);
gotoxy(80,15); cout << "*";
gotoxy(79,16); cout << "***";
color(6);
gotoxy(80,17); cout << "|";

// =============================================================
//   ÁRBOLES - DERECHA (entre ciudad y montañas)
// =============================================================
// Árbol 3 - pino grande (col 128)
color(10);
gotoxy(129,13); cout << "*";
gotoxy(128,14); cout << "***";
gotoxy(127,15); cout << "*****";
gotoxy(128,16); cout << "***";
color(6);
gotoxy(129,17); cout << "||";

// Árbol 4 - pino pequeño (col 130)
color(2);
gotoxy(130,15); cout << "*";
gotoxy(129,16); cout << "***";
color(6);
gotoxy(130,17); cout << "|";

// =============================================================
//   OCÉANO DETALLADO (rows 19-21)
// =============================================================
color(11);
gotoxy(52, 19);
for(int c = 0; c < 103; c++) cout << (c % 5 < 2 ? '~' : '#');
color(9);
gotoxy(52, 20);
for(int c = 0; c < 103; c++) cout << (c % 4 < 2 ? '~' : '#');
color(1);
gotoxy(52, 21);
for(int c = 0; c < 103; c++) cout << (c % 5 < 3 ? '~' : '+');
color(11);
gotoxy(103, 19); cout << "||||";
gotoxy(103, 20); cout << "||||";

// =============================================================
//   PLATAFORMA/PUENTE (rows 22-26, acortado)
// =============================================================
color(15);
gotoxy(52, 22);
for(int c = 0; c < 103; c++) cout << "=";
color(3);
gotoxy(52, 23);
for(int c = 0; c < 103; c++) cout << (c % 3 == 0 ? '+' : '#');
// Pilares (3 rows: head, body, foot)
color(11);
int pilares[] = {60, 80, 100, 120, 140};
for(int i = 0; i < 5; i++){
    int px = pilares[i];
    gotoxy(px, 24); cout << "+--+";
    gotoxy(px, 25); cout << "|##|";
    gotoxy(px, 26); cout << "+--+";
}
// Relleno entre pilares (rows 24-26)
color(3);
for(int r = 24; r <= 26; r++){
    for(int gap_start = 64; gap_start <= 136; gap_start += 20){
        gotoxy(gap_start, r);
        for(int c = 0; c < 16; c++)
            cout << (c % 5 == 0 ? '+' : '#');
    }
}

// Nave volando
color(8);
gotoxy(96, 10); cout << ">-<";
color(15);


/////////////////////////////////////////////////
//    3 SUBCAJAS INFERIORES (rows 27-41)
//      ZONA TIPO HACKER
/////////////////////////////////////////////////

// =============================================================
//  SUBCAJA 1: Monitor de procesos estilo htop (cols 52-91)
//  Interior: cols 53-90 = 38 chars, rows 28-40
// =============================================================
color(15);
drawBox(52, 27, 40, 17);

color(10);
gotoxy(53,28); cout << "[ PROCESS MONITOR ]";
color(8);
gotoxy(53,29); cout << "--------------------------------------";
color(10);
gotoxy(53,30); cout << " PID   USER   CPU%  MEM%  COMMAND";
color(8);
gotoxy(53,31); cout << "--------------------------------------";

color(15); gotoxy(53,32); cout << "1337  root    ";
color(12); cout << "98.2 ";
color(11); cout << "12.4  ";
color(10); cout << "[miner]";

color(15); gotoxy(53,33); cout << "4096  daemon  ";
color(10); cout << "44.7 ";
color(11); cout << " 8.1  ";
color(15); cout << "netwatch -all";

color(15); gotoxy(53,34); cout << "0x1A  root    ";
color(6);  cout << "31.0 ";
color(11); cout << " 5.3  ";
color(15); cout << "/bin/sshd -D";

color(15); gotoxy(53,35); cout << "2048  shadow  ";
color(10); cout << "22.6 ";
color(11); cout << " 3.9  ";
color(15); cout << "exploit.pl -r";

color(15); gotoxy(53,36); cout << "7331  root    ";
color(10); cout << "17.4 ";
color(11); cout << " 2.2  ";
color(15); cout << "tcpdump eth0";

color(15); gotoxy(53,37); cout << "9999  anon    ";
color(6);  cout << "11.3 ";
color(11); cout << " 1.7  ";
color(15); cout << "tor --daemon";

color(8);
gotoxy(53,38); cout << "--------------------------------------";

color(6);
gotoxy(53,39); cout << "CPU[";
color(12); cout << "|||||||||||||||";
color(8);  cout << ".....";
color(6);  cout << "] ";
color(10); cout << "73%";

color(6);
gotoxy(53,40); cout << "MEM[";
color(11); cout << "|||||||";
color(8);  cout << ".............";
color(6);  cout << "] ";
color(10); cout << "31%";

color(6);
gotoxy(53,41); cout <<"SWP[";
color(5);  cout<<"|||||";
color(10); cout<<"           ";
color(6);  cout << "] ";
color(10); cout<<"3.8/10GB           ";

gotoxy(53,42);
color(11); cout<<"Task:";color(10);cout<<" 19";
color(11); cout<<", ";color(10);cout<<" 3 thr";
color(11); cout<<";"; color(10);cout<<"   4 ";
color(11); cout<<"running";

gotoxy(53,43);
color(1); cout<<"ldAvg: ";color(15);cout<<"5.5 ";color(7);cout<<"3.24";color(8);cout<<" 2.79 ";
color(1); cout<<" Uptime"; color(11); cout<<" 94:21:45";



// =============================================================
//  SUBCAJA 2: Log del sistema (cols 92-121)
//  Interior: cols 93-120 = 28 chars, rows 28-40
// =============================================================
color(15);
drawBox(92, 27, 30, 14);

color(3);
gotoxy(93,28); cout << "[ SYSTEM LOG ]";
color(8);
gotoxy(93,29); cout << "----------------------------";

color(8);   gotoxy(93,30); cout << "01:33:07 ";
color(12);  cout << "ALERT ";
color(15);  cout << "scan 443";

color(8);   gotoxy(93,31); cout << "01:33:09 ";
color(10);  cout << "INFO  ";
color(15);  cout << "root login";

color(8);   gotoxy(93,32); cout << "01:33:11 ";
color(12);  cout << "WARN  ";
color(15);  cout << "bruteforce";

color(8);   gotoxy(93,33); cout << "01:33:14 ";
color(10);  cout << "INFO  ";
color(15);  cout << "tun:6881";

color(8);   gotoxy(93,34); cout << "01:33:17 ";
color(6);   cout << "DEBUG ";
color(15);  cout << "DNS spoof OK";

color(8);   gotoxy(93,35); cout << "01:33:20 ";
color(12);  cout << "CRIT  ";
color(15);  cout << "FW bypassed!";

color(8);   gotoxy(93,36); cout << "01:33:22 ";
color(10);  cout << "INFO  ";
color(15);  cout << "payload OK";

color(8);   gotoxy(93,37); cout << "01:33:25 ";
color(12);  cout << "ALERT ";
color(15);  cout << "new session";

color(8);   gotoxy(93,38); cout << "01:33:28 ";
color(10);  cout << "INFO  ";
color(15);  cout << "rootkit set";

color(8);
gotoxy(93,39); cout << "----------------------------";
color(10);
gotoxy(93,40); cout << "root@SHADOW:~# ";
color(15);
cout << "_";


// =============================================================
//  SUBCAJA 3: Metasploit / Exploit Shell (cols 122-153)
//  Interior: cols 123-152 = 30 chars, rows 28-40
//  NUEVO - Contenido hacker estilo Hollywood
// =============================================================
color(15);
drawBox(122, 27, 32, 14);

color(12);
gotoxy(123,28); cout << "[ METASPLOIT SHELL ]";
color(8);
gotoxy(123,29); cout << "------------------------------";

color(6);  gotoxy(123,30); cout << "msf6";
color(15); cout << "> use exploit/smb";
color(6);  gotoxy(123,31); cout << "msf6";
color(15); cout << "> set RHOST 10.0.0.5";
color(6);  gotoxy(123,32); cout << "msf6";
color(15); cout << "> set PAYLOAD shell";
color(6);  gotoxy(123,33); cout << "msf6";
color(15); cout << "> run";

color(8);
gotoxy(123,34); cout << "------------------------------";

color(11); gotoxy(123,35); cout << "[*]";
color(15); cout << " Connecting to host...";
color(11); gotoxy(123,36); cout << "[*]";
color(15); cout << " Auth bypass OK";
color(10); gotoxy(123,37); cout << "[+]";
color(15); cout << " Root shell obtained!";

color(8);
gotoxy(123,38); cout << "------------------------------";

color(15); gotoxy(123,39); cout << "C:\\> whoami";
color(10); gotoxy(123,40); cout << "nt authority\\system";


// =============================================================
//  BARRAS DE ACTIVIDAD DE RED  (cols 93-152, rows 41-43)
//  Espectro estilo Hollywood - barra verde que simula trafico
// =============================================================

// --- Fila 41: NET I/O ---
color(11);  gotoxy(93, 42); cout << "NET I/O";
color(8);   gotoxy(100,42); cout << "|";
// patron de barras: verde bajo -> amarillo medio -> rojo pico
color(10);  gotoxy(101,42); cout << "|||";
color(2);   cout << "||||";
color(10);  cout << "|||||";
color(2);   cout << "|||";
color(10);  cout << "||||";
color(6);   cout << "||||||";
color(12);  cout << "|||";
color(6);   cout << "||||";
color(10);  cout << "||||||";
color(2);   cout << "|||";
color(10);  cout << "|||||";
color(8);   cout << "...";
color(15);  gotoxy(149,42); cout << "8.4K";

// --- Fila 42: PACKETS ---
color(3);   gotoxy(93, 43); cout << "PACKETS";
color(8);   gotoxy(100,43); cout << "|";
color(10);  gotoxy(101,43); cout << "|||||";
color(2);   cout << "||||||";
color(10);  cout << "||||";
color(6);   cout << "|||||||";
color(12);  cout << "||";
color(6);   cout << "||||||";
color(10);  cout << "|||||";
color(2);   cout << "||||";
color(10);  cout << "||||||";
color(2);   cout << "|||";
color(8);   cout << ".....";
color(15);  gotoxy(149,43); cout << " 512";

// --- Fila 43: CONNECTIONS ---
color(5);   gotoxy(93, 44); cout << "CONNS  ";
color(8);   gotoxy(100,44); cout << "|";
color(10);  gotoxy(101,44); cout << "||||";
color(6);   cout << "|||||||";
color(12);  cout << "||||";
color(6);   cout << "|||||";
color(10);  cout << "|||||||";
color(2);   cout << "|||||";
color(10);  cout << "||||";
color(8);   cout << ".......";
color(12);  gotoxy(143,44); cout << "!CRIT";
color(15);  gotoxy(149,44); cout << "  37";



color(0);
cin>>o;
}
