/*
ANALIZADOR SINTACTICO v1 [Metodo LR con Tabla de Parsing / CST]
Angel Eduardo Munoz Perez
Abril 2026
ING COMPUTACION _ CUALTOS
Compiladores Prof. Meno

FUNCIONAMIENTO
==== Analizador Sintactico (complemento del Analizador Lexico)
     Lee el vector de tokens que produjo el analizador lexico y agrega
     un token especial "$" al final para indicar fin de entrada.
     Luego usa una pila y la tabla de parsing (precargada como una
     matriz de strings hardcoded aqui mismo) para ir procesando
     token por token.

     Por cada paso:
       - Mira el tope de la pila (estado actual) y el token actual
       - Busca en la tabla la accion a realizar
       - Si es desplazamiento (dN): mete el token y el nuevo estado a la pila
       - Si es reduccion (rN): saca elementos de la pila segun la regla
                               y luego hace goto con el estado que queda
       - Si es aceptacion (r0): el programa es correcto, termina bien
       - Si no hay accion: hay un error sintactico

     Al final muestra si el codigo paso o no el analisis.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <iomanip>
#include "colores.cpp"
using namespace std;


// ------------------------------------------------- //
//    ESTRUCTURA DE TOKENS (igual que en el lexico)
// ------------------------------------------------- //
struct Token {
    string tipo;
    string cadena;
    short linea;
};


// ------------------------------------------------- //
//    REGLAS DE LA GRAMATICA
//    Cada regla tiene: nombre del no-terminal que produce
//    y cuantos simbolos hay del lado derecho (para saber
//    cuantos elementos sacar de la pila al reducir)
// ------------------------------------------------- //
struct Regla {
    string lhs;   // Lado izquierdo (no terminal resultante)
    int longitud; // Cuantos simbolos tiene el lado derecho
};

// Estas reglas vienen de la gramatica del lenguaje que analiza la tabla
// r0 = aceptacion (la pila queda vacia y llegamos a $)
// r1 a r52 = reducciones normales
Regla reglas[] = {
    // r0: aceptacion
    {"programa",          1}, // r0  - programa -> Definiciones
    {"programa",          1}, // r1  - programa -> programa Definiciones  (en realidad es Accept)
    {"Definiciones",      0}, // r2  - Definiciones -> epsilon
    {"Definiciones",      2}, // r3  - Definiciones -> Definiciones Definicion
    {"Definicion",        1}, // r4  - Definicion -> DefVar
    {"Definicion",        1}, // r5  - Definicion -> DefFunc
    {"DefVar",            4}, // r6  - DefVar -> tipo identificador ListaVar ;
    {"ListaVar",          0}, // r7  - ListaVar -> epsilon
    {"ListaVar",          3}, // r8  - ListaVar -> ListaVar , identificador
    {"DefFunc",           6}, // r9  - DefFunc -> tipo identificador ( Parametros ) BloqFunc
    {"Parametros",        0}, // r10 - Parametros -> epsilon
    {"Parametros",        1}, // r11 - Parametros -> ListaParam
    {"ListaParam",        2}, // r12 - ListaParam -> tipo identificador
    {"ListaParam",        4}, // r13 - ListaParam -> ListaParam , tipo identificador
    {"BloqFunc",          1}, // r14 - BloqFunc -> DefLocales   [reduccion de BloqFunc]
    {"DefLocales",        0}, // r15 - DefLocales -> epsilon  (antes de llegar a sentencias)
    {"DefLocales",        1}, // r16 - DefLocales -> DefLocales }   (cierre del bloque)
    {"DefLocal",          1}, // r17 - DefLocal -> DefVar
    {"DefLocal",          1}, // r18 - DefLocal -> Sentencia
    {"Sentencias",        0}, // r19 - Sentencias -> epsilon
    {"Sentencias",        1}, // r20 - Sentencias -> Sentencias }
    {"Sentencia",         2}, // r21 - Sentencia -> LlamadaFunc ;
    {"Sentencia",         1}, // r22 - Sentencia -> SentenciaBloque
    {"SentenciaBloque",   1}, // r23 - SentenciaBloque -> Bloque
    {"SentenciaBloque",   4}, // r24 - SentenciaBloque -> if ( Expresion ) Sentencia
    {"Sentencia",         3}, // r25 - Sentencia -> identificador = Expresion ;
    {"Sentencia",         1}, // r26 - Sentencia -> SentenciaBloque   (sin else)
    {"Sentencia",         1}, // r27 - Sentencia -> SentenciaBloque   (con else)
    {"Sentencia",         1}, // r28 - Sentencia -> Bloque
    {"ValorRegresa",      0}, // r29 - ValorRegresa -> epsilon
    {"Sentencia",         2}, // r30 - Sentencia -> return ValorRegresa ;
    {"Argumentos",        0}, // r31 - Argumentos -> epsilon
    {"Argumentos",        1}, // r32 - Argumentos -> ListaArgumentos
    {"ListaArgumentos",   1}, // r33 - ListaArgumentos -> Expresion
    {"ListaArgumentos",   3}, // r34 - ListaArgumentos -> ListaArgumentos , Expresion
    {"Termino",           1}, // r35 - Termino -> LlamadaFunc
    {"Termino",           1}, // r36 - Termino -> identificador
    {"Termino",           1}, // r37 - Termino -> entero
    {"Termino",           1}, // r38 - Termino -> real
    {"Termino",           1}, // r39 - Termino -> cadena
    {"Expresion",         1}, // r40 - Expresion -> LlamadaFunc ( )
    {"Sentencias",        2}, // r41 - Sentencias -> Sentencia Sentencias  [parte 1]
    {"Sentencias",        2}, // r42 - Sentencias -> Bloque Sentencias     [parte 2]
    {"Expresion",         3}, // r43 - Expresion -> ( Expresion )
    {"Expresion",         2}, // r44 - Expresion -> opSuma Expresion  (unario)
    {"Expresion",         2}, // r45 - Expresion -> opNot Expresion
    {"Expresion",         3}, // r46 - Expresion -> Expresion opMul Expresion
    {"Expresion",         3}, // r47 - Expresion -> Expresion opSuma Expresion
    {"Expresion",         3}, // r48 - Expresion -> Expresion opRelac Expresion
    {"Expresion",         3}, // r49 - Expresion -> Expresion opOr Expresion
    {"Expresion",         3}, // r50 - Expresion -> Expresion opAnd Expresion
    {"Expresion",         3}, // r51 - Expresion -> Expresion opIgualdad Expresion
    {"Expresion",         1}, // r52 - Expresion -> Termino
};
int totalReglas = 53;


// ------------------------------------------------- //
//    TABLA DE PARSING (LR)
//    Esta es la tabla que vino del archivo Excel,
//    organizada como: tabla[estado][simbolo] = accion
//    Las acciones son strings: "dN", "rN", o numeros (goto)
// ------------------------------------------------- //
map<int, map<string, string>> tabla;

// Llena la tabla con todos los datos que tenia el Excel
void inicializarTabla() {

    // --- Estado 0 ---
    tabla[0]["tipo"]         = "d5";
    tabla[0]["$"]            = "r2";
    tabla[0]["programa"]     = "1";
    tabla[0]["Definiciones"] = "2";
    tabla[0]["Definicion"]   = "3";
    tabla[0]["DefVar"]       = "4";
    tabla[0]["DefFunc"]      = "6";

    // --- Estado 1 ---
    tabla[1]["$"] = "r0";  // ACEPTACION

    // --- Estado 2 ---
    tabla[2]["$"] = "r1";

    // --- Estado 3 ---
    tabla[3]["tipo"]         = "d5";
    tabla[3]["$"]            = "r2";
    tabla[3]["Definiciones"] = "7";
    tabla[3]["Definicion"]   = "3";
    tabla[3]["DefVar"]       = "4";
    tabla[3]["DefFunc"]      = "6";

    // --- Estado 4 ---
    tabla[4]["tipo"] = "r4";
    tabla[4]["$"]    = "r4";

    // --- Estado 5 ---
    tabla[5]["identificador"] = "d8";

    // --- Estado 6 ---
    tabla[6]["tipo"] = "r5";
    tabla[6]["$"]    = "r5";

    // --- Estado 7 ---
    tabla[7]["$"] = "r3";

    // --- Estado 8 ---
    tabla[8][";"]       = "r7";
    tabla[8][","]       = "d10";
    tabla[8]["("]       = "d11";
    tabla[8]["ListaVar"] = "9";

    // --- Estado 9 ---
    tabla[9][";"] = "d12";

    // --- Estado 10 ---
    tabla[10]["identificador"] = "d13";

    // --- Estado 11 ---
    tabla[11]["tipo"]       = "d15";
    tabla[11][")"]          = "r10";
    tabla[11]["Parametros"] = "14";

    // --- Estado 12 ---
    tabla[12]["identificador"] = "r6";
    tabla[12]["tipo"]           = "r6";
    tabla[12]["}"]              = "r6";
    tabla[12]["if"]             = "r6";
    tabla[12]["while"]          = "r6";
    tabla[12]["return"]         = "r6";
    tabla[12]["$"]              = "r6";

    // --- Estado 13 ---
    tabla[13][";"]       = "r7";
    tabla[13][","]       = "d10";
    tabla[13]["ListaVar"] = "16";

    // --- Estado 14 ---
    tabla[14][")"] = "d17";

    // --- Estado 15 ---
    tabla[15]["identificador"] = "d18";

    // --- Estado 16 ---
    tabla[16][";"] = "r8";

    // --- Estado 17 ---
    tabla[17]["{"]       = "d20";
    tabla[17]["BloqFunc"] = "19";

    // --- Estado 18 ---
    tabla[18][","]        = "d22";
    tabla[18][")"]        = "r12";
    tabla[18]["ListaParam"] = "21";

    // --- Estado 19 ---
    tabla[19]["tipo"] = "r9";
    tabla[19]["$"]    = "r9";

    // --- Estado 20 ---
    tabla[20]["identificador"] = "d27";
    tabla[20]["tipo"]          = "d5";
    tabla[20]["}"]             = "r15";
    tabla[20]["if"]            = "d28";
    tabla[20]["while"]         = "d29";
    tabla[20]["return"]        = "d30";
    tabla[20]["DefVar"]        = "25";
    tabla[20]["DefLocales"]    = "23";
    tabla[20]["DefLocal"]      = "24";
    tabla[20]["Sentencia"]     = "26";
    tabla[20]["LlamadaFunc"]   = "31";

    // --- Estado 21 ---
    tabla[21][")"] = "r11";

    // --- Estado 22 ---
    tabla[22]["tipo"] = "d32";

    // --- Estado 23 ---
    tabla[23]["}"] = "d33";

    // --- Estado 24 ---
    tabla[24]["identificador"] = "d27";
    tabla[24]["tipo"]          = "d5";
    tabla[24]["}"]             = "r15";
    tabla[24]["if"]            = "d28";
    tabla[24]["while"]         = "d29";
    tabla[24]["return"]        = "d30";
    tabla[24]["DefVar"]        = "25";
    tabla[24]["DefLocales"]    = "34";
    tabla[24]["DefLocal"]      = "24";
    tabla[24]["Sentencia"]     = "26";
    tabla[24]["LlamadaFunc"]   = "31";

    // --- Estado 25 ---
    tabla[25]["identificador"] = "r17";
    tabla[25]["tipo"]          = "r17";
    tabla[25]["}"]             = "r17";
    tabla[25]["if"]            = "r17";
    tabla[25]["while"]         = "r17";
    tabla[25]["return"]        = "r17";

    // --- Estado 26 ---
    tabla[26]["identificador"] = "r18";
    tabla[26]["tipo"]          = "r18";
    tabla[26]["}"]             = "r18";
    tabla[26]["if"]            = "r18";
    tabla[26]["while"]         = "r18";
    tabla[26]["return"]        = "r18";

    // --- Estado 27 ---
    tabla[27]["("] = "d36";
    tabla[27]["="] = "d35";

    // --- Estado 28 ---
    tabla[28]["("] = "d37";

    // --- Estado 29 ---
    tabla[29]["("] = "d38";

    // --- Estado 30 ---
    tabla[30]["identificador"]  = "d46";
    tabla[30]["entero"]         = "d47";
    tabla[30]["real"]           = "d48";
    tabla[30]["cadena"]         = "d49";
    tabla[30]["opSuma"]         = "d42";
    tabla[30]["opNot"]          = "d43";
    tabla[30][";"]              = "r29";
    tabla[30]["("]              = "d41";
    tabla[30]["ValorRegresa"]   = "39";
    tabla[30]["Termino"]        = "44";
    tabla[30]["LlamadaFunc"]    = "45";
    tabla[30]["Expresion"]      = "40";

    // --- Estado 31 ---
    tabla[31][";"] = "d50";

    // --- Estado 32 ---
    tabla[32]["identificador"] = "d51";

    // --- Estado 33 ---
    tabla[33]["tipo"] = "r14";
    tabla[33]["$"]    = "r14";

    // --- Estado 34 ---
    tabla[34]["}"] = "r16";

    // --- Estado 35 ---
    tabla[35]["identificador"] = "d46";
    tabla[35]["entero"]        = "d47";
    tabla[35]["real"]          = "d48";
    tabla[35]["cadena"]        = "d49";
    tabla[35]["opSuma"]        = "d42";
    tabla[35]["opNot"]         = "d43";
    tabla[35]["("]             = "d41";
    tabla[35]["Termino"]       = "44";
    tabla[35]["LlamadaFunc"]   = "45";
    tabla[35]["Expresion"]     = "52";

    // --- Estado 36 ---
    tabla[36]["identificador"]    = "d46";
    tabla[36]["entero"]           = "d47";
    tabla[36]["real"]             = "d48";
    tabla[36]["cadena"]           = "d49";
    tabla[36]["opSuma"]           = "d42";
    tabla[36]["opNot"]            = "d43";
    tabla[36]["("]                = "d41";
    tabla[36][")"]                = "r31";
    tabla[36]["Argumentos"]       = "53";
    tabla[36]["Termino"]          = "44";
    tabla[36]["LlamadaFunc"]      = "45";
    tabla[36]["Expresion"]        = "54";

    // --- Estado 37 ---
    tabla[37]["identificador"] = "d46";
    tabla[37]["entero"]        = "d47";
    tabla[37]["real"]          = "d48";
    tabla[37]["cadena"]        = "d49";
    tabla[37]["opSuma"]        = "d42";
    tabla[37]["opNot"]         = "d43";
    tabla[37]["("]             = "d41";
    tabla[37]["Termino"]       = "44";
    tabla[37]["LlamadaFunc"]   = "45";
    tabla[37]["Expresion"]     = "55";

    // --- Estado 38 ---
    tabla[38]["identificador"] = "d46";
    tabla[38]["entero"]        = "d47";
    tabla[38]["real"]          = "d48";
    tabla[38]["cadena"]        = "d49";
    tabla[38]["opSuma"]        = "d42";
    tabla[38]["opNot"]         = "d43";
    tabla[38]["("]             = "d41";
    tabla[38]["Termino"]       = "44";
    tabla[38]["LlamadaFunc"]   = "45";
    tabla[38]["Expresion"]     = "56";

    // --- Estado 39 ---
    tabla[39][";"] = "d57";

    // --- Estado 40 ---
    tabla[40]["opSuma"]      = "d59";
    tabla[40]["opMul"]       = "d58";
    tabla[40]["opRelac"]     = "d60";
    tabla[40]["opOr"]        = "d63";
    tabla[40]["opAnd"]       = "d62";
    tabla[40]["opIgualdad"]  = "d61";
    tabla[40][";"]           = "r30";

    // --- Estado 41 ---
    tabla[41]["identificador"] = "d46";
    tabla[41]["entero"]        = "d47";
    tabla[41]["real"]          = "d48";
    tabla[41]["cadena"]        = "d49";
    tabla[41]["opSuma"]        = "d42";
    tabla[41]["opNot"]         = "d43";
    tabla[41]["("]             = "d41";
    tabla[41]["Termino"]       = "44";
    tabla[41]["LlamadaFunc"]   = "45";
    tabla[41]["Expresion"]     = "64";

    // --- Estado 42 ---
    tabla[42]["identificador"] = "d46";
    tabla[42]["entero"]        = "d47";
    tabla[42]["real"]          = "d48";
    tabla[42]["cadena"]        = "d49";
    tabla[42]["opSuma"]        = "d42";
    tabla[42]["opNot"]         = "d43";
    tabla[42]["("]             = "d41";
    tabla[42]["Termino"]       = "44";
    tabla[42]["LlamadaFunc"]   = "45";
    tabla[42]["Expresion"]     = "65";

    // --- Estado 43 ---
    tabla[43]["identificador"] = "d46";
    tabla[43]["entero"]        = "d47";
    tabla[43]["real"]          = "d48";
    tabla[43]["cadena"]        = "d49";
    tabla[43]["opSuma"]        = "d42";
    tabla[43]["opNot"]         = "d43";
    tabla[43]["("]             = "d41";
    tabla[43]["Termino"]       = "44";
    tabla[43]["LlamadaFunc"]   = "45";
    tabla[43]["Expresion"]     = "66";

    // --- Estado 44 ---
    tabla[44]["opSuma"]     = "r52";
    tabla[44]["opMul"]      = "r52";
    tabla[44]["opRelac"]    = "r52";
    tabla[44]["opOr"]       = "r52";
    tabla[44]["opAnd"]      = "r52";
    tabla[44]["opIgualdad"] = "r52";
    tabla[44][";"]          = "r52";
    tabla[44][","]          = "r52";
    tabla[44][")"]          = "r52";

    // --- Estado 45 ---
    tabla[45]["opSuma"]     = "r35";
    tabla[45]["opMul"]      = "r35";
    tabla[45]["opRelac"]    = "r35";
    tabla[45]["opOr"]       = "r35";
    tabla[45]["opAnd"]      = "r35";
    tabla[45]["opIgualdad"] = "r35";
    tabla[45][";"]          = "r35";
    tabla[45][","]          = "r35";
    tabla[45][")"]          = "r35";

    // --- Estado 46 ---
    tabla[46]["opSuma"]     = "r36";
    tabla[46]["opMul"]      = "r36";
    tabla[46]["opRelac"]    = "r36";
    tabla[46]["opOr"]       = "r36";
    tabla[46]["opAnd"]      = "r36";
    tabla[46]["opIgualdad"] = "r36";
    tabla[46][";"]          = "r36";
    tabla[46][","]          = "r36";
    tabla[46]["("]          = "d36";
    tabla[46][")"]          = "r36";

    // --- Estado 47 ---
    tabla[47]["opSuma"]     = "r37";
    tabla[47]["opMul"]      = "r37";
    tabla[47]["opRelac"]    = "r37";
    tabla[47]["opOr"]       = "r37";
    tabla[47]["opAnd"]      = "r37";
    tabla[47]["opIgualdad"] = "r37";
    tabla[47][";"]          = "r37";
    tabla[47][","]          = "r37";
    tabla[47][")"]          = "r37";

    // --- Estado 48 ---
    tabla[48]["opSuma"]     = "r38";
    tabla[48]["opMul"]      = "r38";
    tabla[48]["opRelac"]    = "r38";
    tabla[48]["opOr"]       = "r38";
    tabla[48]["opAnd"]      = "r38";
    tabla[48]["opIgualdad"] = "r38";
    tabla[48][";"]          = "r38";
    tabla[48][","]          = "r38";
    tabla[48][")"]          = "r38";

    // --- Estado 49 ---
    tabla[49]["opSuma"]     = "r39";
    tabla[49]["opMul"]      = "r39";
    tabla[49]["opRelac"]    = "r39";
    tabla[49]["opOr"]       = "r39";
    tabla[49]["opAnd"]      = "r39";
    tabla[49]["opIgualdad"] = "r39";
    tabla[49][";"]          = "r39";
    tabla[49][","]          = "r39";
    tabla[49][")"]          = "r39";

    // --- Estado 50 ---
    tabla[50]["identificador"] = "r25";
    tabla[50]["tipo"]          = "r25";
    tabla[50]["}"]             = "r25";
    tabla[50]["if"]            = "r25";
    tabla[50]["while"]         = "r25";
    tabla[50]["return"]        = "r25";
    tabla[50]["else"]          = "r25";

    // --- Estado 51 ---
    tabla[51][","]        = "d22";
    tabla[51][")"]        = "r12";
    tabla[51]["ListaParam"] = "67";

    // --- Estado 52 ---
    tabla[52]["opSuma"]     = "d59";
    tabla[52]["opMul"]      = "d58";
    tabla[52]["opRelac"]    = "d60";
    tabla[52]["opOr"]       = "d63";
    tabla[52]["opAnd"]      = "d62";
    tabla[52]["opIgualdad"] = "d61";
    tabla[52][";"]          = "d68";

    // --- Estado 53 ---
    tabla[53][")"] = "d69";

    // --- Estado 54 ---
    tabla[54]["opSuma"]          = "d59";
    tabla[54]["opMul"]           = "d58";
    tabla[54]["opRelac"]         = "d60";
    tabla[54]["opOr"]            = "d63";
    tabla[54]["opAnd"]           = "d62";
    tabla[54]["opIgualdad"]      = "d61";
    tabla[54][","]               = "d71";
    tabla[54][")"]               = "r33";
    tabla[54]["ListaArgumentos"] = "70";

    // --- Estado 55 ---
    tabla[55]["opSuma"]     = "d59";
    tabla[55]["opMul"]      = "d58";
    tabla[55]["opRelac"]    = "d60";
    tabla[55]["opOr"]       = "d63";
    tabla[55]["opAnd"]      = "d62";
    tabla[55]["opIgualdad"] = "d61";
    tabla[55][")"]          = "d72";

    // --- Estado 56 ---
    tabla[56]["opSuma"]     = "d59";
    tabla[56]["opMul"]      = "d58";
    tabla[56]["opRelac"]    = "d60";
    tabla[56]["opOr"]       = "d63";
    tabla[56]["opAnd"]      = "d62";
    tabla[56]["opIgualdad"] = "d61";
    tabla[56][")"]          = "d73";

    // --- Estado 57 ---
    tabla[57]["identificador"] = "r24";
    tabla[57]["tipo"]          = "r24";
    tabla[57]["}"]             = "r24";
    tabla[57]["if"]            = "r24";
    tabla[57]["while"]         = "r24";
    tabla[57]["return"]        = "r24";
    tabla[57]["else"]          = "r24";

    // --- Estado 58 ---
    tabla[58]["identificador"] = "d46";
    tabla[58]["entero"]        = "d47";
    tabla[58]["real"]          = "d48";
    tabla[58]["cadena"]        = "d49";
    tabla[58]["opSuma"]        = "d42";
    tabla[58]["opNot"]         = "d43";
    tabla[58]["("]             = "d41";
    tabla[58]["Termino"]       = "44";
    tabla[58]["LlamadaFunc"]   = "45";
    tabla[58]["Expresion"]     = "74";

    // --- Estado 59 ---
    tabla[59]["identificador"] = "d46";
    tabla[59]["entero"]        = "d47";
    tabla[59]["real"]          = "d48";
    tabla[59]["cadena"]        = "d49";
    tabla[59]["opSuma"]        = "d42";
    tabla[59]["opNot"]         = "d43";
    tabla[59]["("]             = "d41";
    tabla[59]["Termino"]       = "44";
    tabla[59]["LlamadaFunc"]   = "45";
    tabla[59]["Expresion"]     = "75";

    // --- Estado 60 ---
    tabla[60]["identificador"] = "d46";
    tabla[60]["entero"]        = "d47";
    tabla[60]["real"]          = "d48";
    tabla[60]["cadena"]        = "d49";
    tabla[60]["opSuma"]        = "d42";
    tabla[60]["opNot"]         = "d43";
    tabla[60]["("]             = "d41";
    tabla[60]["Termino"]       = "44";
    tabla[60]["LlamadaFunc"]   = "45";
    tabla[60]["Expresion"]     = "76";

    // --- Estado 61 ---
    tabla[61]["identificador"] = "d46";
    tabla[61]["entero"]        = "d47";
    tabla[61]["real"]          = "d48";
    tabla[61]["cadena"]        = "d49";
    tabla[61]["opSuma"]        = "d42";
    tabla[61]["opNot"]         = "d43";
    tabla[61]["("]             = "d41";
    tabla[61]["Termino"]       = "44";
    tabla[61]["LlamadaFunc"]   = "45";
    tabla[61]["Expresion"]     = "77";

    // --- Estado 62 ---
    tabla[62]["identificador"] = "d46";
    tabla[62]["entero"]        = "d47";
    tabla[62]["real"]          = "d48";
    tabla[62]["cadena"]        = "d49";
    tabla[62]["opSuma"]        = "d42";
    tabla[62]["opNot"]         = "d43";
    tabla[62]["("]             = "d41";
    tabla[62]["Termino"]       = "44";
    tabla[62]["LlamadaFunc"]   = "45";
    tabla[62]["Expresion"]     = "78";

    // --- Estado 63 ---
    tabla[63]["identificador"] = "d46";
    tabla[63]["entero"]        = "d47";
    tabla[63]["real"]          = "d48";
    tabla[63]["cadena"]        = "d49";
    tabla[63]["opSuma"]        = "d42";
    tabla[63]["opNot"]         = "d43";
    tabla[63]["("]             = "d41";
    tabla[63]["Termino"]       = "44";
    tabla[63]["LlamadaFunc"]   = "45";
    tabla[63]["Expresion"]     = "79";

    // --- Estado 64 ---
    tabla[64]["opSuma"]     = "d59";
    tabla[64]["opMul"]      = "d58";
    tabla[64]["opRelac"]    = "d60";
    tabla[64]["opOr"]       = "d63";
    tabla[64]["opAnd"]      = "d62";
    tabla[64]["opIgualdad"] = "d61";
    tabla[64][")"]          = "d80";

    // --- Estado 65 ---
    tabla[65]["opSuma"]     = "r44";
    tabla[65]["opMul"]      = "r44";
    tabla[65]["opRelac"]    = "r44";
    tabla[65]["opOr"]       = "r44";
    tabla[65]["opAnd"]      = "r44";
    tabla[65]["opIgualdad"] = "r44";
    tabla[65][";"]          = "r44";
    tabla[65][","]          = "r44";
    tabla[65][")"]          = "r44";

    // --- Estado 66 ---
    tabla[66]["opSuma"]     = "r45";
    tabla[66]["opMul"]      = "r45";
    tabla[66]["opRelac"]    = "r45";
    tabla[66]["opOr"]       = "r45";
    tabla[66]["opAnd"]      = "r45";
    tabla[66]["opIgualdad"] = "r45";
    tabla[66][";"]          = "r45";
    tabla[66][","]          = "r45";
    tabla[66][")"]          = "r45";

    // --- Estado 67 ---
    tabla[67][")"] = "r13";

    // --- Estado 68 ---
    tabla[68]["identificador"] = "r21";
    tabla[68]["tipo"]          = "r21";
    tabla[68]["}"]             = "r21";
    tabla[68]["if"]            = "r21";
    tabla[68]["while"]         = "r21";
    tabla[68]["return"]        = "r21";
    tabla[68]["else"]          = "r21";

    // --- Estado 69 ---
    tabla[69]["opSuma"]     = "r40";
    tabla[69]["opMul"]      = "r40";
    tabla[69]["opRelac"]    = "r40";
    tabla[69]["opOr"]       = "r40";
    tabla[69]["opAnd"]      = "r40";
    tabla[69]["opIgualdad"] = "r40";
    tabla[69][";"]          = "r40";
    tabla[69][","]          = "r40";
    tabla[69][")"]          = "r40";

    // --- Estado 70 ---
    tabla[70][")"] = "r32";

    // --- Estado 71 ---
    tabla[71]["identificador"] = "d46";
    tabla[71]["entero"]        = "d47";
    tabla[71]["real"]          = "d48";
    tabla[71]["cadena"]        = "d49";
    tabla[71]["opSuma"]        = "d42";
    tabla[71]["opNot"]         = "d43";
    tabla[71]["("]             = "d41";
    tabla[71]["Termino"]       = "44";
    tabla[71]["LlamadaFunc"]   = "45";
    tabla[71]["Expresion"]     = "81";

    // --- Estado 72 ---
    tabla[72]["identificador"] = "d27";
    tabla[72]["{"]             = "d85";
    tabla[72]["if"]            = "d28";
    tabla[72]["while"]         = "d29";
    tabla[72]["return"]        = "d30";
    tabla[72]["Sentencia"]     = "83";
    tabla[72]["Bloque"]        = "84";
    tabla[72]["LlamadaFunc"]   = "31";
    tabla[72]["SentenciaBloque"] = "82";

    // --- Estado 73 ---
    tabla[73]["{"]     = "d85";
    tabla[73]["Bloque"] = "86";

    // --- Estado 74 ---
    tabla[74]["opSuma"]     = "r46";
    tabla[74]["opMul"]      = "r46";
    tabla[74]["opRelac"]    = "r46";
    tabla[74]["opOr"]       = "r46";
    tabla[74]["opAnd"]      = "r46";
    tabla[74]["opIgualdad"] = "r46";
    tabla[74][";"]          = "r46";
    tabla[74][","]          = "r46";
    tabla[74][")"]          = "r46";

    // --- Estado 75 ---
    tabla[75]["opSuma"]     = "r47";
    tabla[75]["opMul"]      = "d58"; // opMul tiene mayor precedencia
    tabla[75]["opRelac"]    = "r47";
    tabla[75]["opOr"]       = "r47";
    tabla[75]["opAnd"]      = "r47";
    tabla[75]["opIgualdad"] = "r47";
    tabla[75][";"]          = "r47";
    tabla[75][","]          = "r47";
    tabla[75][")"]          = "r47";

    // --- Estado 76 ---
    tabla[76]["opSuma"]     = "d59";
    tabla[76]["opMul"]      = "d58";
    tabla[76]["opRelac"]    = "r48";
    tabla[76]["opOr"]       = "r48";
    tabla[76]["opAnd"]      = "r48";
    tabla[76]["opIgualdad"] = "r48";
    tabla[76][";"]          = "r48";
    tabla[76][","]          = "r48";
    tabla[76][")"]          = "r48";

    // --- Estado 77 ---
    tabla[77]["opSuma"]     = "d59";
    tabla[77]["opMul"]      = "d58";
    tabla[77]["opRelac"]    = "d60";
    tabla[77]["opOr"]       = "r49";
    tabla[77]["opAnd"]      = "r49";
    tabla[77]["opIgualdad"] = "r49";
    tabla[77][";"]          = "r49";
    tabla[77][","]          = "r49";
    tabla[77][")"]          = "r49";

    // --- Estado 78 ---
    tabla[78]["opSuma"]     = "d59";
    tabla[78]["opMul"]      = "d58";
    tabla[78]["opRelac"]    = "d60";
    tabla[78]["opOr"]       = "r50";
    tabla[78]["opAnd"]      = "r50";
    tabla[78]["opIgualdad"] = "d61";
    tabla[78][";"]          = "r50";
    tabla[78][","]          = "r50";
    tabla[78][")"]          = "r50";

    // --- Estado 79 ---
    tabla[79]["opSuma"]     = "d59";
    tabla[79]["opMul"]      = "d58";
    tabla[79]["opRelac"]    = "d60";
    tabla[79]["opOr"]       = "r51";
    tabla[79]["opAnd"]      = "d62";
    tabla[79]["opIgualdad"] = "d61";
    tabla[79][";"]          = "r51";
    tabla[79][","]          = "r51";
    tabla[79][")"]          = "r51";

    // --- Estado 80 ---
    tabla[80]["opSuma"]     = "r43";
    tabla[80]["opMul"]      = "r43";
    tabla[80]["opRelac"]    = "r43";
    tabla[80]["opOr"]       = "r43";
    tabla[80]["opAnd"]      = "r43";
    tabla[80]["opIgualdad"] = "r43";
    tabla[80][";"]          = "r43";
    tabla[80][","]          = "r43";
    tabla[80][")"]          = "r43";

    // --- Estado 81 ---
    tabla[81]["opSuma"]          = "d59";
    tabla[81]["opMul"]           = "d58";
    tabla[81]["opRelac"]         = "d60";
    tabla[81]["opOr"]            = "d63";
    tabla[81]["opAnd"]           = "d62";
    tabla[81]["opIgualdad"]      = "d61";
    tabla[81][","]               = "d71";
    tabla[81][")"]               = "r33";
    tabla[81]["ListaArgumentos"] = "87";

    // --- Estado 82 ---
    tabla[82]["identificador"] = "r26";
    tabla[82]["tipo"]          = "r26";
    tabla[82]["}"]             = "r26";
    tabla[82]["if"]            = "r26";
    tabla[82]["while"]         = "r26";
    tabla[82]["return"]        = "r26";
    tabla[82]["else"]          = "d89";
    tabla[82]["Otro"]          = "88";

    // --- Estado 83 ---
    tabla[83]["identificador"] = "r41";
    tabla[83]["tipo"]          = "r41";
    tabla[83]["}"]             = "r41";
    tabla[83]["if"]            = "r41";
    tabla[83]["while"]         = "r41";
    tabla[83]["return"]        = "r41";
    tabla[83]["else"]          = "r41";

    // --- Estado 84 ---
    tabla[84]["identificador"] = "r42";
    tabla[84]["tipo"]          = "r42";
    tabla[84]["}"]             = "r42";
    tabla[84]["if"]            = "r42";
    tabla[84]["while"]         = "r42";
    tabla[84]["return"]        = "r42";
    tabla[84]["else"]          = "r42";

    // --- Estado 85 ---
    tabla[85]["identificador"] = "d27";
    tabla[85]["}"]             = "r19";
    tabla[85]["if"]            = "d28";
    tabla[85]["while"]         = "d29";
    tabla[85]["return"]        = "d30";
    tabla[85]["Sentencias"]    = "90";
    tabla[85]["Sentencia"]     = "91";
    tabla[85]["LlamadaFunc"]   = "31";

    // --- Estado 86 ---
    tabla[86]["identificador"] = "r23";
    tabla[86]["tipo"]          = "r23";
    tabla[86]["}"]             = "r23";
    tabla[86]["if"]            = "r23";
    tabla[86]["while"]         = "r23";
    tabla[86]["return"]        = "r23";
    tabla[86]["else"]          = "r23";

    // --- Estado 87 ---
    tabla[87][")"] = "r34";

    // --- Estado 88 ---
    tabla[88]["identificador"] = "r22";
    tabla[88]["tipo"]          = "r22";
    tabla[88]["}"]             = "r22";
    tabla[88]["if"]            = "r22";
    tabla[88]["while"]         = "r22";
    tabla[88]["return"]        = "r22";
    tabla[88]["else"]          = "r22";

    // --- Estado 89 ---
    tabla[89]["identificador"] = "d27";
    tabla[89]["{"]             = "d85";
    tabla[89]["if"]            = "d28";
    tabla[89]["while"]         = "d29";
    tabla[89]["return"]        = "d30";
    tabla[89]["Sentencia"]     = "83";
    tabla[89]["Bloque"]        = "84";
    tabla[89]["LlamadaFunc"]   = "31";
    tabla[89]["SentenciaBloque"] = "92";

    // --- Estado 90 ---
    tabla[90]["}"] = "d93";

    // --- Estado 91 ---
    tabla[91]["identificador"] = "d27";
    tabla[91]["}"]             = "r19";
    tabla[91]["if"]            = "d28";
    tabla[91]["while"]         = "d29";
    tabla[91]["return"]        = "d30";
    tabla[91]["Sentencias"]    = "94";
    tabla[91]["Sentencia"]     = "91";
    tabla[91]["LlamadaFunc"]   = "31";

    // --- Estado 92 ---
    tabla[92]["identificador"] = "r27";
    tabla[92]["tipo"]          = "r27";
    tabla[92]["}"]             = "r27";
    tabla[92]["if"]            = "r27";
    tabla[92]["while"]         = "r27";
    tabla[92]["return"]        = "r27";

    // --- Estado 93 ---
    tabla[93]["identificador"] = "r28";
    tabla[93]["tipo"]          = "r28";
    tabla[93]["}"]             = "r28";
    tabla[93]["if"]            = "r28";
    tabla[93]["while"]         = "r28";
    tabla[93]["return"]        = "r28";
    tabla[93]["else"]          = "r28";

    // --- Estado 94 ---
    tabla[94]["}"] = "r20";

}// fin de inicializarTabla


// ------------------------------------------------- //
//    MAPEO DE TOKENS DEL LEXICO A SIMBOLOS DE LA GRAMATICA
//    El analizador lexico usa sus propios nombres de tipo,
//    entonces aqui los convertimos a los simbolos que entiende
//    la tabla de parsing
// ------------------------------------------------- //
string convertirToken(Token t) {
    // Primero revisar si es una palabra reservada especifica
    if (t.tipo == "PalabraRes") {
        // Palabras reservadas que la gramatica conoce directamente
        if (t.cadena == "if")     return "if";
        if (t.cadena == "else")   return "else";
        if (t.cadena == "while")  return "while";
        if (t.cadena == "return") return "return";

        // Tipos de dato -> simbolo "tipo" en la gramatica
        if (t.cadena == "int"    || t.cadena == "float" ||
            t.cadena == "char"   || t.cadena == "bool"  ||
            t.cadena == "string" || t.cadena == "void") {
            return "tipo";
        }

        // Otras palabras reservadas no contempladas en la gramatica
        return t.cadena;
    }

    // Segun el tipo del token
    switch (t.tipo[0]) {
        case 'I':
            // "ID" -> identificador
            return "identificador";

        case 'E':
            // "Entero" -> entero
            return "entero";

        case 'D':
            // "Decimal" y "Delimitador" ambos empiezan con 'D',
            // entonces hay que revisar el tipo completo aqui
            if (t.tipo == "Decimal")     return "real";
            if (t.tipo == "Delimitador") return t.cadena;
            break;

        case 'C':
            if (t.tipo == "Cadena") return "cadena";
            break;

        case 'O':
            // "Operador" y "Asignacion" - revisar cual es
            if (t.tipo == "Asignacion") return "=";

            // Operadores: clasificar segun el simbolo
            if (t.cadena == "+"  || t.cadena == "-")  return "opSuma";
            if (t.cadena == "*"  || t.cadena == "/")  return "opMul";
            if (t.cadena == "<"  || t.cadena == ">"  ||
                t.cadena == "<=" || t.cadena == ">=") return "opRelac";
            if (t.cadena == "||")                     return "opOr";
            if (t.cadena == "&&")                     return "opAnd";
            if (t.cadena == "!")                      return "opNot";
            if (t.cadena == "==")                     return "opIgualdad";

            // Operador no clasificado
            return t.cadena;
    }

    // Si no se reconocio, devolver la cadena tal cual
    return t.cadena;

}// fin de convertirToken


// ------------------------------------------------- //
//    FUNCIONES PARA MOSTRAR ESTADO DE LA PILA
// ------------------------------------------------- //

// Convierte el contenido de la pila a un string legible
// La pila tiene pares (simbolo, estado), los mostramos como:
//   [simbolo:estado] [simbolo:estado] ...
string pilaAString(stack<pair<string,int>> pila) {
    // Como stack no se puede iterar directo, pasamos a vector temporal
    vector<pair<string,int>> temp;
    while (!pila.empty()) {
        temp.push_back(pila.top());
        pila.pop();
    }

    // Lo construimos de abajo hacia arriba
    string resultado = "";
    for (int i = (int)temp.size() - 1; i >= 0; i--) {
        resultado += "[" + temp[i].first + ":" + to_string(temp[i].second) + "] ";
    }
    return resultado;
}

// Muestra una fila del trace del analisis
void mostrarPaso(int paso, string pilaStr, string tokenActual, string accion) {
    color(7); // Blanco normal
    cout << left;
    cout << setw(5)  << paso
         << "| ";

    // Truncar la pila si es muy larga para que quepa en pantalla
    string pilaVisible = pilaStr;
    if ((int)pilaVisible.length() > 45) {
        pilaVisible = "..." + pilaVisible.substr((int)pilaVisible.length() - 42);
    }
    color(11); // Cyan para la pila
    cout << setw(47) << pilaVisible;

    color(14); // Amarillo para el token
    cout << "| " << setw(18) << tokenActual;

    // Color segun la accion
    if (accion[0] == 'd') {
        color(10); // Verde = desplazamiento
    } else if (accion[0] == 'r') {
        color(13); // Magenta = reduccion
    } else if (accion == "ACEPTO") {
        color(10); // Verde brillante
    } else {
        color(12); // Rojo = error
    }
    cout << "| " << accion << endl;
    color(15);
}

// Muestra el encabezado de la tabla de trace
void mostrarEncabezado() {
    color(14); // Amarillo
    cout << left;
    cout << setw(5)  << "PASO"
         << "| "
         << setw(47) << "PILA (simbolo:estado)"
         << "| "
         << setw(18) << "TOKEN ACTUAL"
         << "| "
         << "ACCION" << endl;
    color(8);
    cout << string(5, '-')  << "+"
         << string(48, '-') << "+"
         << string(19, '-') << "+"
         << string(20, '-') << endl;
    color(15);
}


// ------------------------------------------------- //
//    FUNCION PRINCIPAL DEL ANALISIS SINTACTICO
// ------------------------------------------------- //

bool analizarSintaxis(vector<Token> &tokens) {

    // Agregar el token de fin de entrada "$"
    Token tokenFin;
    tokenFin.tipo   = "EOF";
    tokenFin.cadena = "$";
    tokenFin.linea  = -1;
    tokens.push_back(tokenFin);

    // La pila guarda pares de (simbolo, numero de estado)
    // Empieza con el estado inicial 0 y simbolo especial de fondo
    stack<pair<string,int>> pila;
    pila.push({"$inicio", 0});

    int indiceToken = 0; // Apunta al token actual en la lista
    int paso = 1;        // Contador de pasos para mostrar

    bool hayError  = false;
    bool acepto    = false;

    mostrarEncabezado();

    // Loop principal del algoritmo LR
    while (true) {
        // El estado actual es el del tope de la pila
        int estadoActual = pila.top().second;

        // El token actual a revisar
        Token tokenActual = tokens[indiceToken];
        string simboloActual = convertirToken(tokenActual);

        // Buscar la accion en la tabla
        string accion = "";
        if (tabla.count(estadoActual) && tabla[estadoActual].count(simboloActual)) {
            accion = tabla[estadoActual][simboloActual];
        }

        // Construir string de la pila para mostrar
        string pilaStr = pilaAString(pila);

        // Mostrar el estado actual antes de ejecutar la accion
        mostrarPaso(paso, pilaStr, simboloActual + " [" + tokenActual.cadena + "]", accion.empty() ? "ERROR" : accion);
        paso++;

        // Si no hay accion es un error
        if (accion.empty()) {
            color(12);
            cout << "\n  ERROR en linea " << tokenActual.linea
                 << ": token inesperado '" << tokenActual.cadena
                 << "' (tipo: " << simboloActual << ")"
                 << " en estado " << estadoActual << endl;
            color(15);
            hayError = true;
            break;
        }

        // Decidir que hacer segun la accion
        switch (accion[0]) {
            // ---- DESPLAZAMIENTO: meter token a la pila ----
            case 'd': {
                // El numero despues de 'd' es el nuevo estado
                int nuevoEstado = stoi(accion.substr(1));
                pila.push({simboloActual, nuevoEstado});
                indiceToken++; // Avanzar al siguiente token
                break;
            }

            // ---- REDUCCION o ACEPTACION ----
            case 'r': {
                int numRegla = stoi(accion.substr(1));

                // r0 = ACEPTACION
                if (numRegla == 0) {
                    mostrarPaso(paso, pilaAString(pila), simboloActual + " [" + tokenActual.cadena + "]", "ACEPTO");
                    acepto = true;
                    goto terminar; // Salir del while de forma limpia
                }

                // Verificar que la regla existe
                if (numRegla >= totalReglas) {
                    color(12);
                    cout << "\n  ERROR: regla " << numRegla << " no existe en la gramatica." << endl;
                    color(15);
                    hayError = true;
                    goto terminar;
                }

                {
                    Regla regla = reglas[numRegla];

                    // Sacar de la pila tantos elementos como simbolos tiene el lado derecho
                    for (int i = 0; i < regla.longitud; i++) {
                        if (pila.empty()) {
                            color(12);
                            cout << "\n  ERROR: la pila quedo vacia durante reduccion r" << numRegla << endl;
                            color(15);
                            hayError = true;
                            goto terminar;
                        }
                        pila.pop();
                    }

                    // El estado que queda expuesto despues de sacar es el "estado anterior"
                    int estadoDespuesDeReducir = pila.top().second;

                    // Buscar en la tabla el goto con el no-terminal que produce la regla
                    string gotoAccion = "";
                    if (tabla.count(estadoDespuesDeReducir) &&
                        tabla[estadoDespuesDeReducir].count(regla.lhs)) {
                        gotoAccion = tabla[estadoDespuesDeReducir][regla.lhs];
                    }

                    if (gotoAccion.empty()) {
                        color(12);
                        cout << "\n  ERROR: no hay goto para '"
                             << regla.lhs << "' desde estado " << estadoDespuesDeReducir << endl;
                        color(15);
                        hayError = true;
                        goto terminar;
                    }

                    // Meter el no-terminal con su nuevo estado goto
                    int estadoGoto = stoi(gotoAccion);
                    pila.push({regla.lhs, estadoGoto});
                }
                break;
            }

            // ---- Cualquier otra cosa es error ----
            default: {
                // Puede ser un numero de goto que llego aqui por error de logica
                color(12);
                cout << "\n  ERROR: accion inesperada '" << accion << "'" << endl;
                color(15);
                hayError = true;
                goto terminar;
            }
        }// fin switch

    }// fin while

    terminar:

    return acepto && !hayError;

}// fin de analizarSintaxis


// ------------------------------------------------- //
//    FUNCIONES DEL ANALIZADOR LEXICO (Parte 2): TOKENS
//    (Copiadas del analizadorL.cpp para poder usarlo aqui)
// ------------------------------------------------- //

string palabrasReservadas[] = {
    "if", "else", "for", "while", "do", "switch", "case",
    "return", "break", "continue", "int", "float", "char",
    "bool", "cin", "cout", "string", "void"
};
int totalPalabrasReservadas = 18;

bool esPalabraReservada(string palabra) {
    for (int i = 0; i < totalPalabrasReservadas; i++) {
        if (palabrasReservadas[i] == palabra) return true;
    }
    return false;
}

bool esComentarioLinea(string linea, int posicion) {
    return (posicion + 1 < (int)linea.length() &&
            linea[posicion] == '/' &&
            linea[posicion + 1] == '/');
}

bool esComentarioMultilineaInicio(string linea, int posicion) {
    return (posicion + 1 < (int)linea.length() &&
            linea[posicion] == '/' &&
            linea[posicion + 1] == '*');
}

bool esComentarioMultilineaFin(string linea, int posicion) {
    return (posicion + 1 < (int)linea.length() &&
            linea[posicion] == '*' &&
            linea[posicion + 1] == '/');
}

bool esCaracterEscape(string linea, int posicion) {
    return (posicion > 0 && linea[posicion - 1] == '\\');
}

bool lineaTieneContenido(string linea) {
    for (int i = 0; i < (int)linea.length(); i++) {
        if (linea[i] != ' ' && linea[i] != '\t') return true;
    }
    return false;
}

string procesarLinea(string linea, bool &banderaComentarioMultilinea) {
    string resultado = "";
    if (banderaComentarioMultilinea) {
        for (int i = 0; i < (int)linea.length(); i++) {
            if (esComentarioMultilineaFin(linea, i)) {
                banderaComentarioMultilinea = false;
                i++;
                for (int j = i + 1; j < (int)linea.length(); j++) resultado += linea[j];
                break;
            }
        }
        return resultado;
    }
    bool banderaString    = false;
    bool banderaCaracter  = false;
    for (int i = 0; i < (int)linea.length(); i++) {
        char caracterActual = linea[i];
        switch (caracterActual) {
            case '"':
                if (!banderaCaracter && !esCaracterEscape(linea, i)) banderaString = !banderaString;
                resultado += caracterActual;
                break;
            case '\'':
                if (!banderaString && !esCaracterEscape(linea, i)) banderaCaracter = !banderaCaracter;
                resultado += caracterActual;
                break;
            case '/':
                if (banderaString || banderaCaracter) {
                    resultado += caracterActual;
                } else {
                    if (esComentarioMultilineaInicio(linea, i)) { banderaComentarioMultilinea = true; i++; }
                    else if (esComentarioLinea(linea, i))       { return resultado; }
                    else                                        { resultado += caracterActual; }
                }
                break;
            default:
                resultado += caracterActual;
                break;
        }
    }
    return resultado;
}

string leerIdentificador(string linea, int &pos) {
    string res = "";
    while (pos < (int)linea.length() && (isalnum(linea[pos]) || linea[pos] == '_')) {
        res += linea[pos]; pos++;
    }
    pos--;
    return res;
}

string leerNumero(string linea, int &pos, bool &esDecimal) {
    string res = ""; esDecimal = false;
    while (pos < (int)linea.length() && (isdigit(linea[pos]) || linea[pos] == '.')) {
        if (linea[pos] == '.') esDecimal = true;
        res += linea[pos]; pos++;
    }
    pos--;
    return res;
}

string leerCadena(string linea, int &pos) {
    char delimitador = linea[pos];
    string res = "";
    res += linea[pos]; pos++;
    while (pos < (int)linea.length()) {
        res += linea[pos];
        if (linea[pos] == delimitador && linea[pos - 1] != '\\') break;
        pos++;
    }
    return res;
}

void tokenizarLinea(string linea, short numLinea, vector<Token> &tokens) {
    if (!linea.empty() && linea[0] == '#') {
        // Las librerias no participan en el analisis sintactico de este lenguaje
        return;
    }
    for (int i = 0; i < (int)linea.length(); i++) {
        char c = linea[i];
        if (c == ' ' || c == '\t') continue;
        Token t;
        t.linea = numLinea;
        if (c == '"' || c == '\'') {
            t.cadena = leerCadena(linea, i);
            t.tipo   = "Cadena";
        }
        else if (isdigit(c)) {
            bool esDecimal = false;
            t.cadena = leerNumero(linea, i, esDecimal);
            t.tipo   = esDecimal ? "Decimal" : "Entero";
        }
        else if (isalpha(c) || c == '_') {
            t.cadena = leerIdentificador(linea, i);
            t.tipo   = esPalabraReservada(t.cadena) ? "PalabraRes" : "ID";
        }
        else if (c == '=' && i + 1 < (int)linea.length() && linea[i+1] == '=') { t.cadena = "=="; t.tipo = "Operador"; i++; }
        else if (c == '|' && i + 1 < (int)linea.length() && linea[i+1] == '|') { t.cadena = "||"; t.tipo = "Operador"; i++; }
        else if (c == '&' && i + 1 < (int)linea.length() && linea[i+1] == '&') { t.cadena = "&&"; t.tipo = "Operador"; i++; }
        else if (c == '<' && i + 1 < (int)linea.length() && linea[i+1] == '<') { t.cadena = "<<"; t.tipo = "Operador"; i++; }
        else if (c == '>' && i + 1 < (int)linea.length() && linea[i+1] == '>') { t.cadena = ">>"; t.tipo = "Operador"; i++; }
        else {
            switch (c) {
                case '+': case '-': case '*': case '/':
                case '%': case '>': case '<': case '!':
                    t.cadena = string(1, c); t.tipo = "Operador"; break;
                case '=':
                    t.cadena = "="; t.tipo = "Asignacion"; break;
                case ';': case '{': case '}': case '(': case ')': case '[': case ']':
                    t.cadena = string(1, c); t.tipo = "Delimitador"; break;
                default:
                    t.cadena = string(1, c); t.tipo = "Desconocido"; break;
            }
        }
        tokens.push_back(t);
    }
}

// Muestra la tabla de tokens con colores (igual que en el lexico)
void mostrarTablaTokens(vector<Token> &tokens) {
    color(14);
    cout << left;
    cout << setw(12) << "TOKEN"           << "|"
         << setw(22) << "Token Encontrado" << "|"
         << "LINEA" << endl;
    cout << string(12, '_') << "|"
         << string(22, '_') << "|"
         << string(10, '_') << endl;
    color(15);

    for (int i = 0; i < (int)tokens.size(); i++) {
        if      (tokens[i].tipo == "PalabraRes") color(11);
        else if (tokens[i].tipo == "ID")          color(15);
        else if (tokens[i].tipo == "Entero")      color(10);
        else if (tokens[i].tipo == "Decimal")     color(10);
        else if (tokens[i].tipo == "Operador")    color(12);
        else if (tokens[i].tipo == "Asignacion")  color(12);
        else if (tokens[i].tipo == "Delimitador") color(14);
        else if (tokens[i].tipo == "Cadena")      color(13);
        else                                       color(8);

        string cadenaVisible = tokens[i].cadena;
        if ((int)cadenaVisible.length() > 21) cadenaVisible = cadenaVisible.substr(0, 18) + "...";

        cout << left
             << setw(12) << tokens[i].tipo    << "|"
             << setw(22) << cadenaVisible      << "|"
             << tokens[i].linea << endl;
    }
    color(15);
}


// ------------------------------------------------- //
//    MAIN
// ------------------------------------------------- //
int main() {

    string archivoOrigen, archivoDestino;

    // PEDIR ARCHIVOS AL USUARIO
    bool pidioCompilarAlgoInseguro = true;
    while (pidioCompilarAlgoInseguro) {
        color(3);
        cout << "Ingrese el nombre del archivo origen: ";
        color(7);
        cin >> archivoOrigen;
        if (archivoOrigen == "analizadorS.cpp" || archivoOrigen == "analizadorL.cpp") {
            color(12);
            cout << "ESTIMADO!!!! NO VUELVAS A COMETER EL MISMO ERROR, ESCOGE ALGO QUE NO SEA EL MISMO COMPILADOR PARA COMPILARLO >:{" << endl;
            continue;
        } else {
            color(3);
            cout << "Ingrese el nombre del archivo destino (limpio): ";
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

    // ==================================================
    // PARTE 1: LIMPIAR COMENTARIOS Y LINEAS VACIAS
    // ==================================================
    color(3);
    cout << "\n[PARTE 1] Limpiando comentarios y lineas vacias..." << endl;
    color(15);

    string linea;
    bool banderaComentarioMultilinea = false;
    vector<string> lineasLimpias;

    while (getline(origen, linea)) {
        string lineaResultado = procesarLinea(linea, banderaComentarioMultilinea);
        if (lineaTieneContenido(lineaResultado)) {
            destino << lineaResultado << endl;
            lineasLimpias.push_back(lineaResultado);
        }
    }

    origen.close();
    destino.close();

    color(2);
    cout << "Limpieza completada. Archivo generado: " << archivoDestino << endl;
    color(15);

    // ==================================================
    // PARTE 2: TOKENIZAR EL CODIGO LIMPIO
    // ==================================================
    color(3);
    cout << "\n[PARTE 2] Analizando tokens..." << endl;
    color(15);

    vector<Token> tokens;

    for (int i = 0; i < (int)lineasLimpias.size(); i++) {
        short numLinea = (short)(i + 1);
        tokenizarLinea(lineasLimpias[i], numLinea, tokens);
    }

    color(3);
    cout << "\n===== TABLA DE TOKENS =====" << endl;
    color(15);
    mostrarTablaTokens(tokens);

    color(2);
    cout << "\nTotal de tokens encontrados: " << tokens.size() << endl;
    color(15);

    // ==================================================
    // PARTE 3: ANALISIS SINTACTICO LR CON PILA
    // ==================================================
    color(3);
    cout << "\n[PARTE 3] Iniciando analisis sintactico..." << endl;
    color(15);

    // Cargar la tabla de parsing
    inicializarTabla();

    color(3);
    cout << "\n===== TRACE DEL ANALISIS SINTACTICO =====" << endl;
    color(15);

    bool resultado = analizarSintaxis(tokens);

    // Mostrar el veredicto final
    cout << "\n" << string(60, '=') << endl;
    if (resultado) {
        color(10); // Verde brillante
        cout << "  *** ANALISIS CORRECTO: El codigo paso el test sintactico ***" << endl;
    } else {
        color(12); // Rojo
        cout << "  *** ANALISIS FALLIDO: El codigo contiene errores sintacticos ***" << endl;
    }
    color(15);
    cout << string(60, '=') << endl;

    return 0;
}// fin de main
