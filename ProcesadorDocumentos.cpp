#include "ProcesadorDocumentos.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <string>
#include <iostream>

bool letra2Ascii(unsigned char c)
{
    return isalnum(c);
}

static std::string limpiarPalabra(const std::string &palabra)
{
    std::string limpia;
    for (unsigned char c : palabra ) //<-- Estos son los codigos ASCII para poder considerar letras con tildes y ñ-Ñ!
    {
        if (letra2Ascii(c))
        {
            limpia += std::tolower(c);
        }
    }
    return limpia;
}

void ProcesadorDocumentos::procesar(const std::string& texto, int docID, IndiceInvertido& indice, StopWord& filtro) {
    std::stringstream ss(texto);
    std::string palabra;

    while (ss >> palabra) {
        std::string limpia = limpiarPalabra(palabra);
        if (!limpia.empty() && !filtro.esStopWord(limpia)) {
            indice.insertarPalabra(limpia, docID);
        }
    }
}