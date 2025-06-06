#include "Consulta.h"
#include <set>
#include <iostream>
#include <iomanip>
#include <algorithm>

std::vector<int> Consulta::buscarPalabra(const std::string& palabra, IndiceInvertido& indice) {
    std::vector<int> resultado;
    NodoPosteo* actual = indice.obtenerLista(palabra);
    while (actual) {
        resultado.push_back(actual->docID);
        actual = actual->siguiente;
    }
    return resultado;
}

/*std::vector<int> Consulta::buscarPalabra(const std::string &palabra, IndiceInvertido &indice)
{
    std::vector<int> resultado;

    // Depuración: Mostrar la palabra que estamos buscando
    std::cout << "DEBUG: Buscando palabra: '" << palabra << "'" << std::endl;

    NodoPosteo *actual = indice.obtenerLista(palabra);

    // Depuración: Verificar si se encontró la lista de posteo
    if (!actual)
    {
        std::cout << "DEBUG: No se encontró lista de posteo para: '" << palabra << "'" << std::endl;

        // Mostrar caracteres hexadecimales para diagnosticar problemas de codificación
        std::cout << "DEBUG: Códigos hex de la palabra: ";
        for (unsigned char c : palabra)
        {
            std::cout << std::hex << static_cast<int>(c) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    while (actual)
    {
        resultado.push_back(actual->docID);
        actual = actual->siguiente;
    }

    // Depuración: Mostrar resultados encontrados
    std::cout << "DEBUG: Documentos encontrados: " << resultado.size() << std::endl;

    return resultado;
}*/

std::vector<int> Consulta::interseccion(const std::vector<std::string>& palabras, IndiceInvertido& indice) {
    std::vector<std::set<int>> sets;
    for (const auto& p : palabras) {
        std::set<int> temp;
        NodoPosteo* actual = indice.obtenerLista(p);
        while (actual) {
            temp.insert(actual->docID);
            actual = actual->siguiente;
        }
        sets.push_back(temp);
    }

    if (sets.empty()) return {};
    std::set<int> resultado = sets[0];
    for (size_t i = 1; i < sets.size(); ++i) {
        std::set<int> temp;
        std::set_intersection(resultado.begin(), resultado.end(),
                              sets[i].begin(), sets[i].end(),
                              std::inserter(temp, temp.begin()));
        resultado = temp;
    }
    return std::vector<int>(resultado.begin(), resultado.end());
}