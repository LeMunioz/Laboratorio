#pragma once
// -----------------------------------------------------------------------
// ANALIZADOR SINTACTICO v1 [Metodo LR con Tabla de Parsing]
// Angel Eduardo Munoz Perez | Abril 2026
//
// NOTA: Token y funciones lexicas vienen de analizadorLexico.cpp
//       Este archivo solo define lo exclusivo del analisis sintactico.
// -----------------------------------------------------------------------

#include <stack>
#include <map>
#include <iomanip>

// ------------------------------------------------- //
//    REGLAS DE LA GRAMATICA
// ------------------------------------------------- //
struct Regla {
    string lhs;
    int longitud;
};

Regla reglas[] = {
    {"programa",          1}, // r0  - Accept
    {"programa",          1}, // r1
    {"Definiciones",      0}, // r2
    {"Definiciones",      2}, // r3
    {"Definicion",        1}, // r4
    {"Definicion",        1}, // r5
    {"DefVar",            4}, // r6
    {"ListaVar",          0}, // r7
    {"ListaVar",          3}, // r8
    {"DefFunc",           6}, // r9
    {"Parametros",        0}, // r10
    {"Parametros",        1}, // r11
    {"ListaParam",        2}, // r12
    {"ListaParam",        4}, // r13
    {"BloqFunc",          1}, // r14
    {"DefLocales",        0}, // r15
    {"DefLocales",        1}, // r16
    {"DefLocal",          1}, // r17
    {"DefLocal",          1}, // r18
    {"Sentencias",        0}, // r19
    {"Sentencias",        1}, // r20
    {"Sentencia",         2}, // r21
    {"Sentencia",         1}, // r22
    {"SentenciaBloque",   1}, // r23
    {"SentenciaBloque",   4}, // r24
    {"Sentencia",         3}, // r25
    {"Sentencia",         1}, // r26
    {"Sentencia",         1}, // r27
    {"Sentencia",         1}, // r28
    {"ValorRegresa",      0}, // r29
    {"Sentencia",         2}, // r30
    {"Argumentos",        0}, // r31
    {"Argumentos",        1}, // r32
    {"ListaArgumentos",   1}, // r33
    {"ListaArgumentos",   3}, // r34
    {"Termino",           1}, // r35
    {"Termino",           1}, // r36
    {"Termino",           1}, // r37
    {"Termino",           1}, // r38
    {"Termino",           1}, // r39
    {"Expresion",         1}, // r40
    {"Sentencias",        2}, // r41
    {"Sentencias",        2}, // r42
    {"Expresion",         3}, // r43
    {"Expresion",         2}, // r44
    {"Expresion",         2}, // r45
    {"Expresion",         3}, // r46
    {"Expresion",         3}, // r47
    {"Expresion",         3}, // r48
    {"Expresion",         3}, // r49
    {"Expresion",         3}, // r50
    {"Expresion",         3}, // r51
    {"Expresion",         1}, // r52
};
int totalReglas = 53;


// ------------------------------------------------- //
//    TABLA DE PARSING (LR)
// ------------------------------------------------- //
map<int, map<string, string>> tabla;

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
    tabla[1]["$"] = "r0";

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
    tabla[8][";"]        = "r7";
    tabla[8][","]        = "d10";
    tabla[8]["("]        = "d11";
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
    tabla[13][";"]        = "r7";
    tabla[13][","]        = "d10";
    tabla[13]["ListaVar"] = "16";

    // --- Estado 14 ---
    tabla[14][")"] = "d17";

    // --- Estado 15 ---
    tabla[15]["identificador"] = "d18";

    // --- Estado 16 ---
    tabla[16][";"] = "r8";

    // --- Estado 17 ---
    tabla[17]["{"]        = "d20";
    tabla[17]["BloqFunc"] = "19";

    // --- Estado 18 ---
    tabla[18][","]          = "d22";
    tabla[18][")"]          = "r12";
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
    tabla[30]["identificador"] = "d46";
    tabla[30]["entero"]        = "d47";
    tabla[30]["real"]          = "d48";
    tabla[30]["cadena"]        = "d49";
    tabla[30]["opSuma"]        = "d42";
    tabla[30]["opNot"]         = "d43";
    tabla[30][";"]             = "r29";
    tabla[30]["("]             = "d41";
    tabla[30]["ValorRegresa"]  = "39";
    tabla[30]["Termino"]       = "44";
    tabla[30]["LlamadaFunc"]   = "45";
    tabla[30]["Expresion"]     = "40";

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
    tabla[36]["identificador"] = "d46";
    tabla[36]["entero"]        = "d47";
    tabla[36]["real"]          = "d48";
    tabla[36]["cadena"]        = "d49";
    tabla[36]["opSuma"]        = "d42";
    tabla[36]["opNot"]         = "d43";
    tabla[36]["("]             = "d41";
    tabla[36][")"]             = "r31";
    tabla[36]["Argumentos"]    = "53";
    tabla[36]["Termino"]       = "44";
    tabla[36]["LlamadaFunc"]   = "45";
    tabla[36]["Expresion"]     = "54";

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
    tabla[40]["opSuma"]     = "d59";
    tabla[40]["opMul"]      = "d58";
    tabla[40]["opRelac"]    = "d60";
    tabla[40]["opOr"]       = "d63";
    tabla[40]["opAnd"]      = "d62";
    tabla[40]["opIgualdad"] = "d61";
    tabla[40][";"]          = "r30";

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
    tabla[51][","]          = "d22";
    tabla[51][")"]          = "r12";
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
    tabla[72]["identificador"]   = "d27";
    tabla[72]["{"]               = "d85";
    tabla[72]["if"]              = "d28";
    tabla[72]["while"]           = "d29";
    tabla[72]["return"]          = "d30";
    tabla[72]["Sentencia"]       = "83";
    tabla[72]["Bloque"]          = "84";
    tabla[72]["LlamadaFunc"]     = "31";
    tabla[72]["SentenciaBloque"] = "82";

    // --- Estado 73 ---
    tabla[73]["{"]      = "d85";
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
    tabla[75]["opMul"]      = "d58";
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
    tabla[89]["identificador"]   = "d27";
    tabla[89]["{"]               = "d85";
    tabla[89]["if"]              = "d28";
    tabla[89]["while"]           = "d29";
    tabla[89]["return"]          = "d30";
    tabla[89]["Sentencia"]       = "83";
    tabla[89]["Bloque"]          = "84";
    tabla[89]["LlamadaFunc"]     = "31";
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
}


// ------------------------------------------------- //
//    MAPEO DE TOKENS DEL LEXICO A SIMBOLOS DE LA GRAMATICA
// ------------------------------------------------- //
string convertirToken(Token t) {
    if (t.tipo == "PalabraRes") {
        if (t.cadena == "if")     return "if";
        if (t.cadena == "else")   return "else";
        if (t.cadena == "while")  return "while";
        if (t.cadena == "return") return "return";

        if (t.cadena == "int"    || t.cadena == "float" ||
            t.cadena == "char"   || t.cadena == "bool"  ||
            t.cadena == "string" || t.cadena == "void") {
            return "tipo";
        }

        return t.cadena;
    }

    switch (t.tipo[0]) {
        case 'I':
            return "identificador";

        case 'E':
            return "entero";

        case 'D':
            if (t.tipo == "Decimal")     return "real";
            if (t.tipo == "Delimitador") return t.cadena;
            break;

        case 'C':
            if (t.tipo == "Cadena") return "cadena";
            break;

        case 'O':
            if (t.tipo == "Asignacion") return "=";

            if (t.cadena == "+"  || t.cadena == "-")  return "opSuma";
            if (t.cadena == "*"  || t.cadena == "/")  return "opMul";
            if (t.cadena == "<"  || t.cadena == ">"  ||
                t.cadena == "<=" || t.cadena == ">=") return "opRelac";
            if (t.cadena == "||")                     return "opOr";
            if (t.cadena == "&&")                     return "opAnd";
            if (t.cadena == "!")                      return "opNot";
            if (t.cadena == "==")                     return "opIgualdad";

            return t.cadena;
    }

    return t.cadena;
}


// ------------------------------------------------- //
//    FUNCIONES PARA MOSTRAR ESTADO DE LA PILA
// ------------------------------------------------- //

string pilaAString(stack<pair<string,int>> pila) {
    vector<pair<string,int>> temp;
    while (!pila.empty()) {
        temp.push_back(pila.top());
        pila.pop();
    }

    string resultado = "";
    for (int i = (int)temp.size() - 1; i >= 0; i--) {
        resultado += "[" + temp[i].first + ":" + to_string(temp[i].second) + "] ";
    }
    return resultado;
}

void mostrarPaso(int paso, string pilaStr, string tokenActual, string accion) {
    color(7);
    cout << left;
    cout << setw(5) << paso << "| ";

    string pilaVisible = pilaStr;
    if ((int)pilaVisible.length() > 45) {
        pilaVisible = "..." + pilaVisible.substr((int)pilaVisible.length() - 42);
    }
    color(11);
    cout << setw(47) << pilaVisible;

    color(14);
    cout << "| " << setw(18) << tokenActual;

    if (!accion.empty() && accion[0] == 'd') {
        color(10);
    } else if (!accion.empty() && accion[0] == 'r') {
        color(13);
    } else if (accion == "ACEPTO") {
        color(10);
    } else {
        color(12);
    }
    cout << "| " << accion << endl;
    color(15);
}

void mostrarEncabezado() {
    color(14);
    cout << left;
    cout << setw(5)  << "PASO"
         << "| "
         << setw(47) << "PILA (simbolo:estado)"
         << "| "
         << setw(18) << "TOKEN ACTUAL"
         << "| "
         << "ACCION" << endl;
    color(8);
    cout << string(5,  '-') << "+"
         << string(48, '-') << "+"
         << string(19, '-') << "+"
         << string(20, '-') << endl;
    color(15);
}


// ------------------------------------------------- //
//    FUNCION PRINCIPAL DEL ANALISIS SINTACTICO
// ------------------------------------------------- //

bool analizarSintaxis(vector<Token> &tokens) {

    // Agregar token de fin de entrada
    Token tokenFin;
    tokenFin.tipo   = "EOF";
    tokenFin.cadena = "$";
    tokenFin.linea  = -1;
    tokens.push_back(tokenFin);

    stack<pair<string,int>> pila;
    pila.push({"$inicio", 0});

    int indiceToken = 0;
    int paso = 1;

    bool hayError = false;
    bool acepto   = false;

    mostrarEncabezado();

    while (true) {
        int estadoActual = pila.top().second;

        Token tokenActual    = tokens[indiceToken];
        string simboloActual = convertirToken(tokenActual);

        string accion = "";
        if (tabla.count(estadoActual) && tabla[estadoActual].count(simboloActual)) {
            accion = tabla[estadoActual][simboloActual];
        }

        string pilaStr = pilaAString(pila);

        mostrarPaso(paso, pilaStr,
                    simboloActual + " [" + tokenActual.cadena + "]",
                    accion.empty() ? "ERROR" : accion);
        paso++;

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

        switch (accion[0]) {
            case 'd': {
                int nuevoEstado = stoi(accion.substr(1));
                pila.push({simboloActual, nuevoEstado});
                indiceToken++;
                break;
            }

            case 'r': {
                int numRegla = stoi(accion.substr(1));

                if (numRegla == 0) {
                    mostrarPaso(paso, pilaAString(pila),
                                simboloActual + " [" + tokenActual.cadena + "]",
                                "ACEPTO");
                    acepto = true;
                    goto terminar;
                }

                if (numRegla >= totalReglas) {
                    color(12);
                    cout << "\n  ERROR: regla " << numRegla << " no existe." << endl;
                    color(15);
                    hayError = true;
                    goto terminar;
                }

                {
                    Regla regla = reglas[numRegla];

                    for (int i = 0; i < regla.longitud; i++) {
                        if (pila.empty()) {
                            color(12);
                            cout << "\n  ERROR: pila vacia en reduccion r" << numRegla << endl;
                            color(15);
                            hayError = true;
                            goto terminar;
                        }
                        pila.pop();
                    }

                    int estadoDespues = pila.top().second;

                    string gotoAccion = "";
                    if (tabla.count(estadoDespues) && tabla[estadoDespues].count(regla.lhs)) {
                        gotoAccion = tabla[estadoDespues][regla.lhs];
                    }

                    if (gotoAccion.empty()) {
                        color(12);
                        cout << "\n  ERROR: no hay goto para '"
                             << regla.lhs << "' desde estado " << estadoDespues << endl;
                        color(15);
                        hayError = true;
                        goto terminar;
                    }

                    int estadoGoto = stoi(gotoAccion);
                    pila.push({regla.lhs, estadoGoto});
                }
                break;
            }

            default: {
                color(12);
                cout << "\n  ERROR: accion inesperada '" << accion << "'" << endl;
                color(15);
                hayError = true;
                goto terminar;
            }
        }
    }

    terminar:
    return acepto && !hayError;
}
