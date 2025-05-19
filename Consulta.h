#ifndef CONSULTA_H
#define CONSULTA_H

#include <string>
#include <vector>
#include "IndiceInvertido.h"

class Consulta {
public:
    std::vector<int> buscarPalabra(const std::string& palabra, IndiceInvertido& indice);
    std::vector<int> interseccion(const std::vector<std::string>& palabras, IndiceInvertido& indice);
};

#endif