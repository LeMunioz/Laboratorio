#pragma once
// -----------------------------------------------------------------------
// ANALIZADOR SEMANTICO v1 [Arbol de Sintaxis Concreto / Verificacion de Tipos]
// Angel Eduardo Munoz Perez | Abril 2026
//
// NOTA: Token y funciones lexicas vienen de analizadorLexico.cpp
//       Este archivo solo define lo exclusivo del analisis semantico.
// -----------------------------------------------------------------------

#include <map>

// ------------------------------------------------- //
//    NODO DEL ARBOL DE SINTAXIS CONCRETO (CST)
// ------------------------------------------------- //
struct NodoCST {
    string categoria;
    string valor;
    string tipoDato;
    short  linea;
    vector<NodoCST*> hijos;

    NodoCST(string cat, string val, string tipo, short lin)
        : categoria(cat), valor(val), tipoDato(tipo), linea(lin) {}

    ~NodoCST() {
        for (int i = 0; i < (int)hijos.size(); i++) {
            delete hijos[i];
        }
    }
};


// ------------------------------------------------- //
//    VARIABLE GLOBAL DE ERROR SEMANTICO
// ------------------------------------------------- //
bool huboError = false;


// ------------------------------------------------- //
//    TABLA DE SIMBOLOS
// ------------------------------------------------- //
struct InfoSimbolo {
    string tipo;
    short  linea;
};

map<string, InfoSimbolo> tablaSimbolos;

bool declararSimbolo(string nombre, string tipo, short lineaActual) {
    if (tablaSimbolos.count(nombre)) {
        huboError = true;
        color(12);
        cout << "\n  [!] ERROR SEMANTICO" << endl;
        cout << "      Nodo    : Declaracion  '" << tipo << " " << nombre << "'" << endl;
        cout << "      Linea   : " << lineaActual << endl;
        cout << "      Causa   : La variable '" << nombre
             << "' ya ha sido declarada (declaracion original en linea "
             << tablaSimbolos[nombre].linea << ")" << endl;
        color(15);
        return false;
    }
    tablaSimbolos[nombre] = {tipo, lineaActual};
    return true;
}

string buscarSimbolo(string nombre) {
    if (tablaSimbolos.count(nombre)) {
        return tablaSimbolos[nombre].tipo;
    }
    return "?";
}


// ------------------------------------------------- //
//    FUNCIONES AUXILIARES: CLASIFICACION DE TOKENS
// ------------------------------------------------- //

string tipoReservadoASemantico(string palabraRes) {
    if (palabraRes == "int")    return "int";
    if (palabraRes == "float")  return "float";
    if (palabraRes == "char")   return "char";
    if (palabraRes == "bool")   return "bool";
    if (palabraRes == "string") return "string";
    return "?";
}

string inferirTipoTerminal(Token t) {
    if (t.tipo == "Entero")   return "int";
    if (t.tipo == "Decimal")  return "float";
    if (t.tipo == "Cadena") {
        if (t.cadena.length() == 3 && t.cadena[0] == '\'') return "char";
        return "string";
    }
    if (t.tipo == "ID")       return buscarSimbolo(t.cadena);
    if (t.tipo == "PalabraRes") {
        if (t.cadena == "true" || t.cadena == "false") return "bool";
    }
    return "?";
}

bool esOperadorLogico(string op) {
    return (op == "==" || op == "!=" || op == "<"  || op == ">"  ||
            op == "<=" || op == ">=" || op == "&&" || op == "||" ||
            op == "!"  || op == "!=");
}


// ------------------------------------------------- //
//    FUNCIONES DE COMPATIBILIDAD DE TIPOS
// ------------------------------------------------- //

bool sonCompatibles(string tipoIzq, string tipoDer, string operador) {
    if (tipoIzq == "?" || tipoDer == "?") return true;

    bool izqNum = (tipoIzq == "int" || tipoIzq == "float");
    bool derNum = (tipoDer  == "int" || tipoDer  == "float");
    if (izqNum && derNum) return true;

    if (tipoIzq == tipoDer) return true;

    if (tipoIzq == "string" && tipoDer == "string") {
        return (operador == "+" || operador == "==");
    }

    return false;
}

string inferirTipoResultado(string tipoIzq, string tipoDer, string operador) {
    if (esOperadorLogico(operador))                       return "bool";
    if (tipoIzq == "float" || tipoDer == "float")         return "float";
    if (tipoIzq == "string" || tipoDer == "string")       return "string";
    if (tipoIzq == "char"   || tipoDer == "char")         return "char";
    return tipoIzq;
}


// ------------------------------------------------- //
//    CONSTRUCCION DEL ARBOL CST
// ------------------------------------------------- //

NodoCST* construirExpresion(vector<Token> &tokens, int &pos, int limite);

NodoCST* construirTerminal(Token t) {
    string tipo = inferirTipoTerminal(t);
    return new NodoCST("Termino", t.cadena, tipo, t.linea);
}

NodoCST* construirExpresion(vector<Token> &tokens, int &pos, int limite) {
    if (pos >= limite) return nullptr;

    NodoCST* izq = construirTerminal(tokens[pos]);
    pos++;

    while (pos < limite) {
        Token tOp = tokens[pos];
        if (tOp.tipo != "Operador" && tOp.tipo != "Asignacion") break;

        string op = tOp.cadena;
        pos++;

        if (pos >= limite) break;

        NodoCST* der = construirTerminal(tokens[pos]);
        pos++;

        NodoCST* nodoOp = new NodoCST("Expresion", op, "?", tOp.linea);
        nodoOp->hijos.push_back(izq);
        nodoOp->hijos.push_back(der);

        izq = nodoOp;
    }

    return izq;
}

NodoCST* construirSentencia(vector<Token> &tokens, int &pos) {
    int total = (int)tokens.size();

    if (pos >= total) return nullptr;

    Token actual = tokens[pos];

    // Patron 1 y 2: Declaracion de variable
    if (actual.tipo == "PalabraRes" &&
        (actual.cadena == "int"    || actual.cadena == "float" ||
         actual.cadena == "char"   || actual.cadena == "bool"  ||
         actual.cadena == "string")) {

        string tipoDeclared = tipoReservadoASemantico(actual.cadena);
        short lineaDec = actual.linea;
        pos++;

        if (pos >= total || tokens[pos].tipo != "ID") return nullptr;

        string nombreVar = tokens[pos].cadena;
        pos++;

        declararSimbolo(nombreVar, tipoDeclared, lineaDec);

        NodoCST* nodoDec = new NodoCST("Declaracion",
            tipoDeclared + " " + nombreVar, tipoDeclared, lineaDec);

        if (pos < total && tokens[pos].tipo == "Asignacion") {
            pos++;

            int inicio = pos;
            while (pos < total &&
                   tokens[pos].cadena != ";" &&
                   tokens[pos].cadena != ",") {
                pos++;
            }
            int fin = pos;

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

        if (pos < total && tokens[pos].cadena == ";") pos++;

        return nodoDec;
    }

    // Patron 3: Asignacion simple
    if (actual.tipo == "ID" &&
        pos + 1 < total &&
        tokens[pos + 1].tipo == "Asignacion") {

        string nombreVar = actual.cadena;
        short lineaAsig  = actual.linea;
        pos += 2;

        int inicio = pos;
        while (pos < total && tokens[pos].cadena != ";") pos++;
        int fin = pos;

        int p = inicio;
        NodoCST* nodExp = construirExpresion(tokens, p, fin);

        if (pos < total && tokens[pos].cadena == ";") pos++;

        NodoCST* nodoAsig = new NodoCST("Asignacion", "=", "?", lineaAsig);
        nodoAsig->hijos.push_back(
            new NodoCST("Termino", nombreVar, buscarSimbolo(nombreVar), lineaAsig));
        if (nodExp) nodoAsig->hijos.push_back(nodExp);
        return nodoAsig;
    }

    // Patron 4: If / While
    if (actual.tipo == "PalabraRes" &&
        (actual.cadena == "if" || actual.cadena == "while")) {

        string tipoEst = actual.cadena;
        short lineaEst = actual.linea;
        pos++;

        if (pos < total && tokens[pos].cadena == "(") pos++;

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

        if (pos < total && tokens[pos].cadena == ")") pos++;

        NodoCST* nodoEst = new NodoCST("Estructura", tipoEst, "bool", lineaEst);
        if (nodExp) nodoEst->hijos.push_back(nodExp);
        return nodoEst;
    }

    // Patron 5: Return
    if (actual.tipo == "PalabraRes" && actual.cadena == "return") {
        short lineaRet = actual.linea;
        pos++;

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

    // Token no reconocido: avanzar
    pos++;
    return nullptr;
}

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
//    IMPRESION DEL ARBOL CST
// ------------------------------------------------- //
void imprimirArbol(NodoCST* nodo, int nivel = 0) {
    if (!nodo) return;

    string indent(nivel * 3, ' ');

    if      (nodo->categoria == "Programa")    color(14);
    else if (nodo->categoria == "Declaracion") color(11);
    else if (nodo->categoria == "Asignacion")  color(12);
    else if (nodo->categoria == "Expresion")   color(13);
    else if (nodo->categoria == "Termino")     color(10);
    else if (nodo->categoria == "Estructura")  color(9);
    else if (nodo->categoria == "Return")      color(15);
    else                                       color(8);

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
//    ANALISIS SEMANTICO (recorrido postorden)
// ------------------------------------------------- //

string analizarNodo(NodoCST* nodo) {
    if (!nodo) return "?";

    if (nodo->categoria == "Termino") {
        return nodo->tipoDato;
    }

    for (int i = 0; i < (int)nodo->hijos.size(); i++) {
        string tipoHijo = analizarNodo(nodo->hijos[i]);
        nodo->hijos[i]->tipoDato = tipoHijo;
    }

    if (nodo->categoria == "Expresion" && (int)nodo->hijos.size() == 2) {
        string tipoIzq = nodo->hijos[0]->tipoDato;
        string tipoDer = nodo->hijos[1]->tipoDato;
        string op      = nodo->valor;

        if (!sonCompatibles(tipoIzq, tipoDer, op)) {
            huboError = true;
            color(12);
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
        }

        string tipoRes = inferirTipoResultado(tipoIzq, tipoDer, op);
        nodo->tipoDato = tipoRes;
        return tipoRes;
    }

    if (nodo->categoria == "Asignacion" && (int)nodo->hijos.size() == 2) {
        string tipoVar = nodo->hijos[0]->tipoDato;
        string tipoExp = nodo->hijos[1]->tipoDato;

        if (!sonCompatibles(tipoVar, tipoExp, "=")) {
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
        }

        nodo->tipoDato = tipoVar;
        return tipoVar;
    }

    if (nodo->categoria == "Declaracion") {
        return nodo->tipoDato;
    }

    if (nodo->categoria == "Estructura" && !nodo->hijos.empty()) {
        string tipoCond = nodo->hijos[0]->tipoDato;
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

    if (nodo->categoria == "Return" && !nodo->hijos.empty()) {
        return nodo->hijos[0]->tipoDato;
    }

    return nodo->tipoDato;
}

void analizarSemantica(NodoCST* raiz) {
    for (int i = 0; i < (int)raiz->hijos.size(); i++) {
        analizarNodo(raiz->hijos[i]);
    }
}
