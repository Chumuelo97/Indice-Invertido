#ifndef PROCESADORDOCUMENTOS_H
#define PROCESADORDOCUMENTOS_H

#include <string>
#include "IndiceInvertido.h"
#include "StopWord.h"

bool letra2Ascii(unsigned char c);
class ProcesadorDocumentos {
public:
    void procesar(const std::string& texto, int docID, IndiceInvertido& indice, StopWord& filtro);
};

#endif