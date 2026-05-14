/*
ANALIZADOR SEMANTICO v1 [Arbol de Sintaxis Concreto / Verificacion de Tipos]
Angel Eduardo Munoz Perez
Abril 2026
ING COMPUTACION _ CUALTOS
Compiladores Prof. Meno

FUNCIONAMIENTO
==== Analizador Semantico (complemento del Analizador Sintactico)

     A partir del vector de tokens que produjo el analizador lexico, construimos
     un Arbol de Sintaxis Concreto (CST) recorriendo los tokens de forma descendente.
     Cada nodo del arbol guarda: tipo de nodo, valor (lexema), tipo de dato
     semantico inferido, y la linea donde aparece en el codigo fuente.

     CONSTRUCCION DEL ARBOL:
       Se recorre el vector de tokens buscando patrones de sentencias:
         - Declaracion de variable: tipo id [= Expresion] ;
         - Asignacion:              id = Expresion ;
         - If/While:                if/while ( Expresion )
         - Return:                  return Expresion ;
       Cada patron genera un sub-arbol con nodo raiz (sentencia) e hijos
       (operandos y operadores de la expresion).

     ANALISIS SEMANTICO:
       Se recorre el CST en postorden. Para cada nodo de operacion binaria
       (asignacion, aritmetica, comparacion, logica) se verifica que los tipos
       de dato de los hijos sean compatibles entre si.

       Reglas de compatibilidad de tipos:
         - int    op int    -> OK  (resultado int)
         - float  op float  -> OK  (resultado float)
         - int    op float  -> OK  (se permite mezcla numerica)
         - string op string -> OK  solo si el operador es + o ==
         - char   op char   -> OK  solo si el operador es ==
         - Cualquier mezcla de string/char con int/float -> ERROR SEMANTICO
         - Asignar string a int / float, o viceversa    -> ERROR SEMANTICO

       Reglas de declaracion de variables:
         - Una variable puede declararse solo UNA vez en el mismo ambito
         - Si se intenta declarar una variable que ya existe: ERROR SEMANTICO
         - La reasignacion (id = valor) SI esta permitida sin restriccion
         - Al detectar redeclaracion: imprime el nodo, la linea actual y la
           linea donde fue declarada originalmente, y el nombre de la variable

       Si hay un error: imprime el nodo, el tipo encontrado vs el esperado,
       y la linea del codigo fuente donde ocurre.
       Si no hay errores: imprime "El codigo ha pasado todas las pruebas".
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include "colores.cpp"
using namespace std;


// ------------------------------------------------- //
//    ESTRUCTURA DE TOKENS (igual que en lexico/sintactico)
// ------------------------------------------------- //
struct Token {
    string tipo;
    string cadena;
    short  linea;
};


// ------------------------------------------------- //
//    NODO DEL ARBOL DE SINTAXIS
//    Cada nodo representa un elemento del programa:
//    una sentencia, un operador, un operando, etc.
// ------------------------------------------------- //
struct NodoCST {
    string categoria;          // Tipo de nodo: "Sentencia", "Expresion", "Termino", "Operador", etc.
    string valor;              // Lexema o etiqueta del nodo  (ej. "int a = b + 1")
    string tipoDato;           // Tipo semantico inferido: "int", "float", "string", "char", "bool", "?"
    short  linea;              // Linea en el codigo fuente
    vector<NodoCST*> hijos;    // Sub-arboles hijos (operandos)

    // Constructor
    NodoCST(string cat, string val, string tipo, short lin)
        : categoria(cat), valor(val), tipoDato(tipo), linea(lin) {}

    // Destructor recursivo
    ~NodoCST() {
        for (int i = 0; i < (int)hijos.size(); i++) {
            delete hijos[i];
        }
    }
};


// ------------------------------------------------- //
//    VARIABLE GLOBAL DE ERROR SEMANTICO
//    Se declara aqui arriba porque la usan tanto
//    declararSimbolo (redeclaracion) como analizarNodo (tipos)
// ------------------------------------------------- //
bool huboError = false;


// ------------------------------------------------- //
//    TABLA DE SIMBOLOS
//    Guarda el tipo de dato y la linea de declaracion
//    de cada variable para poder:
//      1. Resolver el tipo cuando aparezca su ID
//      2. Detectar si la variable ya fue declarada antes
// ------------------------------------------------- //
struct InfoSimbolo {
    string tipo;   // Tipo semantico: "int", "float", "string", etc.
    short  linea;  // Linea donde fue declarada
};

map<string, InfoSimbolo> tablaSimbolos;

/*
   declararSimbolo: intenta registrar una variable nueva.
   Si el nombre ya existe en la tabla, es una redeclaracion:
   imprime el error con el nodo, la linea actual y la linea
   original, y el nombre del identificador repetido.
   Si no existe, la registra normalmente.
   Devuelve true si la declaracion fue valida, false si fue redeclaracion.
*/
bool declararSimbolo(string nombre, string tipo, short lineaActual) {
    if (tablaSimbolos.count(nombre)) {
        // ======== REPORTE DE ERROR: REDECLARACION ========
        huboError = true;
        color(12); // Rojo
        cout << "\n  [!] ERROR SEMANTICO" << endl;
        cout << "      Nodo    : Declaracion  '" << tipo << " " << nombre << "'" << endl;
        cout << "      Linea   : " << lineaActual << endl;
        cout << "      Causa   : La variable '" << nombre
             << "' ya ha sido declarada (declaracion original en linea "
             << tablaSimbolos[nombre].linea << ")" << endl;
        color(15);
        // =================================================
        return false;
    }
    tablaSimbolos[nombre] = {tipo, lineaActual};
    return true;
}

// Busca el tipo de un identificador; regresa "?" si no esta declarado
string buscarSimbolo(string nombre) {
    if (tablaSimbolos.count(nombre)) {
        return tablaSimbolos[nombre].tipo;
    }
    return "?";
}


// ------------------------------------------------- //
//    FUNCIONES AUXILIARES: CLASIFICACION DE TOKENS
// ------------------------------------------------- //

// Mapea la palabra reservada de tipo a string semantico
string tipoReservadoASemantico(string palabraRes) {
    if (palabraRes == "int")    return "int";
    if (palabraRes == "float")  return "float";
    if (palabraRes == "char")   return "char";
    if (palabraRes == "bool")   return "bool";
    if (palabraRes == "string") return "string";
    return "?";
}

// Determina el tipo semantico directo de un token terminal
string inferirTipoTerminal(Token t) {
    if (t.tipo == "Entero")   return "int";
    if (t.tipo == "Decimal")  return "float";
    if (t.tipo == "Cadena") {
        // Cadena de un caracter entre comillas simples -> char; resto -> string
        if (t.cadena.length() == 3 && t.cadena[0] == '\'') return "char";
        return "string";
    }
    if (t.tipo == "ID")       return buscarSimbolo(t.cadena);
    if (t.tipo == "PalabraRes") {
        if (t.cadena == "true" || t.cadena == "false") return "bool";
    }
    return "?";
}

// Decide si un operador es de comparacion/logica (resultado bool)
bool esOperadorLogico(string op) {
    return (op == "==" || op == "!=" || op == "<"  || op == ">"  ||
            op == "<=" || op == ">=" || op == "&&" || op == "||" ||
            op == "!"  || op == "!=");
}

// Decide si un operador es solo de cadenas (concatenacion)
bool esOperadorCadena(string op) {
    return (op == "+");
}


// ------------------------------------------------- //
//    FUNCIONES DE COMPATIBILIDAD DE TIPOS
// ------------------------------------------------- //

/*
   Reglas:
     - Numericos (int, float) son compatibles entre si
     - string + string es valido (concatenacion), string == string tambien
     - char  == char  es valido
     - Mezcla string/char con numerico -> ERROR
     - bool solo es compatible con bool en operaciones logicas
*/
bool sonCompatibles(string tipoIzq, string tipoDer, string operador) {
    // Si alguno es desconocido, lo dejamos pasar (ya se reportara como no declarado)
    if (tipoIzq == "?" || tipoDer == "?") return true;

    // Numericos entre si: siempre compatibles
    bool izqNum = (tipoIzq == "int" || tipoIzq == "float");
    bool derNum = (tipoDer  == "int" || tipoDer  == "float");
    if (izqNum && derNum) return true;

    // Mismo tipo: siempre compatible
    if (tipoIzq == tipoDer) return true;

    // string con operadores validos: + y ==
    if (tipoIzq == "string" && tipoDer == "string") {
        return (operador == "+" || operador == "==");
    }

    // Mezcla invalida
    return false;
}

// Infiere el tipo resultado de una operacion binaria
string inferirTipoResultado(string tipoIzq, string tipoDer, string operador) {
    if (esOperadorLogico(operador)) return "bool";
    if (tipoIzq == "float" || tipoDer == "float") return "float";
    if (tipoIzq == "string" || tipoDer == "string") return "string";
    if (tipoIzq == "char"   || tipoDer == "char")   return "char";
    return tipoIzq; // Por defecto devuelve el tipo del lado izquierdo
}


// ------------------------------------------------- //
//    CONSTRUCCION DEL ARBOL CST
//    Recorre el vector de tokens y genera sub-arboles
//    por cada sentencia reconocida
// ------------------------------------------------- //

// Declaracion adelantada
NodoCST* construirExpresion(vector<Token> &tokens, int &pos, int limite);


// Construye un nodo terminal (hoja del arbol) desde el token actual
NodoCST* construirTerminal(Token t) {
    string tipo = inferirTipoTerminal(t);
    return new NodoCST("Termino", t.cadena, tipo, t.linea);
}

/*
   construirExpresion: lee tokens desde pos hasta limite (exclusive)
   y construye un sub-arbol de expresion.
   Maneja expresiones binarias del tipo: termino op termino op termino ...
   y devuelve un nodo raiz "Expresion" con sus hijos ya enlazados.
*/
NodoCST* construirExpresion(vector<Token> &tokens, int &pos, int limite) {
    if (pos >= limite) return nullptr;

    // Leer el primer operando (termino)
    NodoCST* izq = construirTerminal(tokens[pos]);
    pos++;

    // Si no hay mas tokens en esta expresion, devolver el termino solo
    while (pos < limite) {
        Token tOp = tokens[pos];
        // Debe ser un operador
        if (tOp.tipo != "Operador" && tOp.tipo != "Asignacion") break;

        string op = tOp.cadena;
        pos++;

        if (pos >= limite) break;

        // Leer el operando derecho
        NodoCST* der = construirTerminal(tokens[pos]);
        pos++;

        // Crear nodo de operacion binaria
        NodoCST* nodoOp = new NodoCST("Expresion", op, "?", tOp.linea);
        nodoOp->hijos.push_back(izq);
        nodoOp->hijos.push_back(der);

        // El resultado de esta operacion es el nuevo operando izquierdo
        izq = nodoOp;
    }

    return izq;
}

/*
   construirSentencia: detecta el patron de la sentencia que empieza en pos
   y construye el sub-arbol correspondiente.
   Patrones reconocidos:
     1. Declaracion:  tipo id ;
     2. Decl+Asig:    tipo id = Expresion ;
     3. Asignacion:   id = Expresion ;
     4. If/While:     if/while ( Expresion )
     5. Return:       return Expresion ;
*/
NodoCST* construirSentencia(vector<Token> &tokens, int &pos) {
    int total = (int)tokens.size();

    if (pos >= total) return nullptr;

    Token actual = tokens[pos];

    // ---- PATRON 1 y 2: Declaracion de variable (tipo id ...) ----
    if (actual.tipo == "PalabraRes" &&
        (actual.cadena == "int"    || actual.cadena == "float" ||
         actual.cadena == "char"   || actual.cadena == "bool"  ||
         actual.cadena == "string")) {

        string tipoDeclared = tipoReservadoASemantico(actual.cadena);
        short lineaDec = actual.linea;
        pos++; // consumir tipo

        if (pos >= total || tokens[pos].tipo != "ID") {
            return nullptr; // error de sintaxis, no construimos
        }

        string nombreVar = tokens[pos].cadena;
        pos++; // consumir id

        // Registrar en tabla de simbolos (verifica redeclaracion)
        declararSimbolo(nombreVar, tipoDeclared, lineaDec);

        // Nodo raiz de la declaracion
        NodoCST* nodoDec = new NodoCST("Declaracion",
            tipoDeclared + " " + nombreVar, tipoDeclared, lineaDec);

        // Verificar si hay asignacion: tipo id = Expresion ;
        if (pos < total && tokens[pos].tipo == "Asignacion") {
            pos++; // consumir '='

            // Recolectar tokens de la expresion hasta ';' o ',' o ')'
            int inicio = pos;
            while (pos < total &&
                   tokens[pos].cadena != ";" &&
                   tokens[pos].cadena != "," ) {
                pos++;
            }
            int fin = pos;

            // Construir sub-arbol de la expresion
            int p = inicio;
            NodoCST* nodExp = construirExpresion(tokens, p, fin);
            if (nodExp) {
                NodoCST* nodoAsig = new NodoCST("Asignacion", "=", "?", lineaDec);
                nodoAsig->hijos.push_back(
                    new NodoCST("Termino", nombreVar, tipoDeclared, lineaDec));
                nodoAsig->hijos.push_back(nodExp);
                nodoDec->hijos.push_back(nodoAsig);
            }
        }

        // Consumir ';' si esta
        if (pos < total && tokens[pos].cadena == ";") pos++;

        return nodoDec;
    }

    // ---- PATRON 3: Asignacion simple (id = Expresion ;) ----
    if (actual.tipo == "ID" &&
        pos + 1 < total &&
        tokens[pos + 1].tipo == "Asignacion") {

        string nombreVar = actual.cadena;
        short lineaAsig  = actual.linea;
        pos += 2; // consumir id y '='

        // Recolectar la expresion hasta ';'
        int inicio = pos;
        while (pos < total && tokens[pos].cadena != ";") pos++;
        int fin = pos;

        int p = inicio;
        NodoCST* nodExp = construirExpresion(tokens, p, fin);

        // Consumir ';'
        if (pos < total && tokens[pos].cadena == ";") pos++;

        NodoCST* nodoAsig = new NodoCST("Asignacion", "=", "?", lineaAsig);
        nodoAsig->hijos.push_back(
            new NodoCST("Termino", nombreVar, buscarSimbolo(nombreVar), lineaAsig));
        if (nodExp) nodoAsig->hijos.push_back(nodExp);
        return nodoAsig;
    }

    // ---- PATRON 4: If / While (palabra ( Expresion ) ) ----
    if (actual.tipo == "PalabraRes" &&
        (actual.cadena == "if" || actual.cadena == "while")) {

        string tipoEst = actual.cadena;
        short lineaEst = actual.linea;
        pos++; // consumir if/while

        // Consumir '('
        if (pos < total && tokens[pos].cadena == "(") pos++;

        // Recolectar expresion hasta ')'
        int inicio = pos;
        int depth  = 1;
        while (pos < total && depth > 0) {
            if      (tokens[pos].cadena == "(") depth++;
            else if (tokens[pos].cadena == ")") depth--;
            if (depth > 0) pos++;
            else           break;
        }
        int fin = pos;

        int p = inicio;
        NodoCST* nodExp = construirExpresion(tokens, p, fin);

        // Consumir ')'
        if (pos < total && tokens[pos].cadena == ")") pos++;

        NodoCST* nodoEst = new NodoCST("Estructura", tipoEst, "bool", lineaEst);
        if (nodExp) nodoEst->hijos.push_back(nodExp);
        return nodoEst;
    }

    // ---- PATRON 5: Return (return Expresion ;) ----
    if (actual.tipo == "PalabraRes" && actual.cadena == "return") {
        short lineaRet = actual.linea;
        pos++; // consumir 'return'

        int inicio = pos;
        while (pos < total && tokens[pos].cadena != ";") pos++;
        int fin = pos;

        if (pos < total && tokens[pos].cadena == ";") pos++;

        int p = inicio;
        NodoCST* nodExp = construirExpresion(tokens, p, fin);

        NodoCST* nodoRet = new NodoCST("Return", "return", "?", lineaRet);
        if (nodExp) nodoRet->hijos.push_back(nodExp);
        return nodoRet;
    }

    // Token no reconocido como inicio de sentencia: avanzar
    pos++;
    return nullptr;
}

/*
   construirArbol: crea el arbol raiz "Programa" y llena sus hijos
   llamando a construirSentencia por cada sentencia del codigo
*/
NodoCST* construirArbol(vector<Token> &tokens) {
    NodoCST* raiz = new NodoCST("Programa", "programa", "?", 0);

    int pos = 0;
    while (pos < (int)tokens.size()) {
        NodoCST* sentencia = construirSentencia(tokens, pos);
        if (sentencia) {
            raiz->hijos.push_back(sentencia);
        }
    }

    return raiz;
}


// ------------------------------------------------- //
//    IMPRESION DEL ARBOL (para trazabilidad)
//    Muestra el arbol con indentacion por nivel
// ------------------------------------------------- //
void imprimirArbol(NodoCST* nodo, int nivel = 0) {
    if (!nodo) return;

    string indent(nivel * 3, ' ');

    // Color segun categoria del nodo
    if      (nodo->categoria == "Programa")    color(14); // Amarillo
    else if (nodo->categoria == "Declaracion") color(11); // Cyan
    else if (nodo->categoria == "Asignacion")  color(12); // Rojo
    else if (nodo->categoria == "Expresion")   color(13); // Magenta
    else if (nodo->categoria == "Termino")     color(10); // Verde
    else if (nodo->categoria == "Estructura")  color(9);  // Azul
    else if (nodo->categoria == "Return")      color(15); // Blanco
    else                                       color(8);  // Gris

    cout << indent
         << "[" << nodo->categoria << "]"
         << "  val='" << nodo->valor << "'"
         << "  tipo=" << nodo->tipoDato
         << "  linea=" << nodo->linea
         << endl;

    color(15);

    for (int i = 0; i < (int)nodo->hijos.size(); i++) {
        imprimirArbol(nodo->hijos[i], nivel + 1);
    }
}


// ------------------------------------------------- //
//    ANALISIS SEMANTICO
//    Recorre el arbol en postorden, verifica tipos
//    en cada nodo de operacion y reporta errores
// ------------------------------------------------- //

/*
   analizarNodo: visita recursivamente cada nodo del arbol.
   En postorden: primero analiza los hijos y ya tienen tipo inferido,
   luego verifica la compatibilidad en el nodo padre.
   Devuelve el tipo semantico resultante del sub-arbol.
*/
string analizarNodo(NodoCST* nodo) {
    if (!nodo) return "?";

    // -- Caso hoja: Termino (ya tiene tipo inferido al construir) --
    if (nodo->categoria == "Termino") {
        return nodo->tipoDato;
    }

    // -- Recorrer hijos primero (postorden) --
    for (int i = 0; i < (int)nodo->hijos.size(); i++) {
        string tipoHijo = analizarNodo(nodo->hijos[i]);
        // Actualizar tipo del hijo por si fue resuelto recursivamente
        nodo->hijos[i]->tipoDato = tipoHijo;
    }

    // -- Verificar nodo de Expresion binaria (2 hijos: izq y der) --
    if (nodo->categoria == "Expresion" && (int)nodo->hijos.size() == 2) {
        string tipoIzq = nodo->hijos[0]->tipoDato;
        string tipoDer = nodo->hijos[1]->tipoDato;
        string op      = nodo->valor;

        if (!sonCompatibles(tipoIzq, tipoDer, op)) {
            // ======== REPORTE DE ERROR SEMANTICO ========
            huboError = true;
            color(12); // Rojo
            cout << "\n  [!] ERROR SEMANTICO" << endl;
            cout << "      Nodo    : Expresion  '" << op << "'" << endl;
            cout << "      Linea   : " << nodo->linea << endl;
            cout << "      Tipo izq: " << tipoIzq
                 << "  ('" << nodo->hijos[0]->valor << "')" << endl;
            cout << "      Tipo der: " << tipoDer
                 << "  ('" << nodo->hijos[1]->valor << "')" << endl;
            cout << "      Causa   : Operacion '" << op
                 << "' entre tipos incompatibles ["
                 << tipoIzq << " y " << tipoDer << "]" << endl;
            color(15);
            // ============================================
        }

        // Inferir tipo resultado para seguir propagando hacia arriba
        string tipoRes = inferirTipoResultado(tipoIzq, tipoDer, op);
        nodo->tipoDato = tipoRes;
        return tipoRes;
    }

    // -- Verificar nodo de Asignacion (2 hijos: variable y expresion) --
    if (nodo->categoria == "Asignacion" && (int)nodo->hijos.size() == 2) {
        string tipoVar = nodo->hijos[0]->tipoDato;
        string tipoExp = nodo->hijos[1]->tipoDato;

        if (!sonCompatibles(tipoVar, tipoExp, "=")) {
            // ======== REPORTE DE ERROR SEMANTICO ========
            huboError = true;
            color(12);
            cout << "\n  [!] ERROR SEMANTICO" << endl;
            cout << "      Nodo    : Asignacion  '='" << endl;
            cout << "      Linea   : " << nodo->linea << endl;
            cout << "      Variable: '" << nodo->hijos[0]->valor
                 << "'  tipo=" << tipoVar << endl;
            cout << "      Expresion tipo: " << tipoExp
                 << "  ('" << nodo->hijos[1]->valor << "')" << endl;
            cout << "      Causa   : No se puede asignar ["
                 << tipoExp << "] a variable de tipo ["
                 << tipoVar << "]" << endl;
            color(15);
            // ============================================
        }

        nodo->tipoDato = tipoVar;
        return tipoVar;
    }

    // -- Declaracion: propagar el tipo declarado --
    if (nodo->categoria == "Declaracion") {
        return nodo->tipoDato;
    }

    // -- Estructura (if/while): la condicion debe ser o resolverse a bool o numerico --
    if (nodo->categoria == "Estructura" && !nodo->hijos.empty()) {
        string tipoCond = nodo->hijos[0]->tipoDato;
        // Solo bloqueamos cadenas usadas como condicion
        if (tipoCond == "string" || tipoCond == "char") {
            huboError = true;
            color(12);
            cout << "\n  [!] ERROR SEMANTICO" << endl;
            cout << "      Nodo    : Condicion en '" << nodo->valor << "'" << endl;
            cout << "      Linea   : " << nodo->linea << endl;
            cout << "      Tipo    : " << tipoCond << endl;
            cout << "      Causa   : La condicion no puede ser de tipo ["
                 << tipoCond << "]" << endl;
            color(15);
        }
        return "bool";
    }

    // -- Return: propagar el tipo de la expresion que devuelve --
    if (nodo->categoria == "Return" && !nodo->hijos.empty()) {
        return nodo->hijos[0]->tipoDato;
    }

    return nodo->tipoDato;
}

/*
   analizarSemantica: punto de entrada del analisis.
   Recorre todos los hijos del nodo raiz (sentencias del programa).
*/
void analizarSemantica(NodoCST* raiz) {
    for (int i = 0; i < (int)raiz->hijos.size(); i++) {
        analizarNodo(raiz->hijos[i]);
    }
}


// ------------------------------------------------- //
//    FUNCIONES DEL ANALIZADOR LEXICO
//    (reproducidas del lexico para que este archivo
//     sea autocontenido y no dependa del .exe del lexico)
// ------------------------------------------------- //

bool esComentarioLinea(string linea, int pos) {
    return (pos + 1 < (int)linea.length() &&
            linea[pos] == '/' && linea[pos + 1] == '/');
}

bool esComentarioMultilineaInicio(string linea, int pos) {
    return (pos + 1 < (int)linea.length() &&
            linea[pos] == '/' && linea[pos + 1] == '*');
}

bool esComentarioMultilineaFin(string linea, int pos) {
    return (pos + 1 < (int)linea.length() &&
            linea[pos] == '*' && linea[pos + 1] == '/');
}

bool esCaracterEscape(string linea, int pos) {
    return (pos > 0 && linea[pos - 1] == '\\');
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
                for (int j = i + 1; j < (int)linea.length(); j++) {
                    resultado += linea[j];
                }
                break;
            }
        }
        return resultado;
    }

    bool banderaString    = false;
    bool banderaCaracter  = false;

    for (int i = 0; i < (int)linea.length(); i++) {
        char c = linea[i];
        switch (c) {
            case '"':
                if (!banderaCaracter && !esCaracterEscape(linea, i))
                    banderaString = !banderaString;
                resultado += c;
                break;
            case '\'':
                if (!banderaString && !esCaracterEscape(linea, i))
                    banderaCaracter = !banderaCaracter;
                resultado += c;
                break;
            case '/':
                if (banderaString || banderaCaracter) {
                    resultado += c;
                } else {
                    if      (esComentarioMultilineaInicio(linea, i)) { banderaComentarioMultilinea = true; i++; }
                    else if (esComentarioLinea(linea, i))              return resultado;
                    else                                               resultado += c;
                }
                break;
            default:
                resultado += c;
                break;
        }
    }
    return resultado;
}

// Lista de palabras reservadas
string palabrasReservadas[] = {
    "if", "else", "for", "while", "do", "switch", "case",
    "return", "break", "continue", "int", "float", "char",
    "bool", "cin", "cout", "string"
};
int totalPalabrasReservadas = 17;

bool esPalabraReservada(string p) {
    for (int i = 0; i < totalPalabrasReservadas; i++) {
        if (palabrasReservadas[i] == p) return true;
    }
    return false;
}

string leerIdentificador(string linea, int &pos) {
    string res = "";
    while (pos < (int)linea.length() && (isalnum(linea[pos]) || linea[pos] == '_')) {
        res += linea[pos++];
    }
    pos--;
    return res;
}

string leerNumero(string linea, int &pos, bool &esDecimal) {
    string res = "";
    esDecimal = false;
    while (pos < (int)linea.length() && (isdigit(linea[pos]) || linea[pos] == '.')) {
        if (linea[pos] == '.') esDecimal = true;
        res += linea[pos++];
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
    if (!linea.empty() && linea[0] == '#') return; // ignorar librerias

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
        else if (c == '=' && i+1 < (int)linea.length() && linea[i+1] == '=') { t.cadena = "=="; t.tipo = "Operador"; i++; }
        else if (c == '|' && i+1 < (int)linea.length() && linea[i+1] == '|') { t.cadena = "||"; t.tipo = "Operador"; i++; }
        else if (c == '&' && i+1 < (int)linea.length() && linea[i+1] == '&') { t.cadena = "&&"; t.tipo = "Operador"; i++; }
        else if (c == '<' && i+1 < (int)linea.length() && linea[i+1] == '<') { t.cadena = "<<"; t.tipo = "Operador"; i++; }
        else if (c == '>' && i+1 < (int)linea.length() && linea[i+1] == '>') { t.cadena = ">>"; t.tipo = "Operador"; i++; }
        else {
            switch (c) {
                case '+': case '-': case '*': case '/':
                case '%': case '>': case '<': case '!':
                    t.cadena = string(1, c); t.tipo = "Operador"; break;
                case '=':
                    t.cadena = "="; t.tipo = "Asignacion"; break;
                case ';': case '{': case '}': case '(': case ')': case '[': case ']': case ',':
                    t.cadena = string(1, c); t.tipo = "Delimitador"; break;
                default:
                    t.cadena = string(1, c); t.tipo = "Desconocido"; break;
            }
        }

        tokens.push_back(t);
    }
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
        if (archivoOrigen == "AnalizadorSemantico.cpp" ||
            archivoOrigen == "analizadorS.cpp"         ||
            archivoOrigen == "analizadorL.cpp") {
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

    color(2);
    cout << "Total de tokens encontrados: " << tokens.size() << endl;
    color(15);

    // ==================================================
    // PARTE 3: CONSTRUCCION DEL ARBOL DE SINTAXIS (CST)
    // ==================================================
    color(3);
    cout << "\n[PARTE 3] Construyendo arbol de sintaxis..." << endl;
    color(15);

    NodoCST* arbol = construirArbol(tokens);

    color(3);
    cout << "\n===== ARBOL DE SINTAXIS CONCRETO =====" << endl;
    color(15);
    imprimirArbol(arbol);

    // ==================================================
    // PARTE 4: ANALISIS SEMANTICO
    // ==================================================
    color(3);
    cout << "\n[PARTE 4] Iniciando analisis semantico..." << endl;
    color(15);

    color(3);
    cout << "\n===== RESULTADO DEL ANALISIS SEMANTICO =====" << endl;
    color(15);

    analizarSemantica(arbol);

    // Veredicto final
    cout << "\n" << string(60, '=') << endl;
    if (!huboError) {
        color(10); // Verde brillante
        cout << "  El codigo ha pasado todas las pruebas." << endl;
    } else {
        color(12); // Rojo
        cout << "  *** ANALISIS FALLIDO: El codigo contiene errores semanticos ***" << endl;
    }
    color(15);
    cout << string(60, '=') << endl;

    // Liberar memoria del arbol
    delete arbol;

    return 0;
}// fin de main
