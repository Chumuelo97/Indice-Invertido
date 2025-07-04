#ifndef CONSULTA_H
#define CONSULTA_H

#include <string>
#include <vector>
#include <map>
#include "IndiceInvertido.h"

class Consulta {
private:
    std::map<int, double> puntajePR;
public:
    std::vector<int> buscarPalabra(const std::string& palabra, IndiceInvertido& indice);
    std::vector<int> interseccion(const std::vector<std::string>& palabras, IndiceInvertido& indice);

    //vamos a cargar los puntajes y reordenar el vector de resultados desde los puntajes cargados
    void cargarPageRank(const std::string &archivo);
    std::vector<int> reordenarPuntajePr(const std::vector<int>& resultados);
};

#endif