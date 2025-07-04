#include "Consulta.h"
#include <set>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>


std::vector<int> Consulta::buscarPalabra(const std::string &palabra, IndiceInvertido &indice)
{
    std::vector<int> resultado;
    NodoPosteo *actual = indice.obtenerLista(palabra);
    while (actual)
    {
        resultado.push_back(actual->docID);
        actual = actual->siguiente;
    }
    return resultado;
}

std::vector<int> Consulta::interseccion(const std::vector<std::string> &palabras, IndiceInvertido &indice)
{
    if (palabras.empty())
    {
        return {};
    }

    // Obtener la lista de documentos para la primera palabra.
    // Este será nuestro conjunto de resultados inicial.
    NodoPosteo *actual = indice.obtenerLista(palabras[0]);
    if (!actual)
    {
        // Si la primera palabra no está en el índice, no hay intersección posible.
        return {};
    }

    std::set<int> resultado;
    while (actual)
    {
        resultado.insert(actual->docID);
        actual = actual->siguiente;
    }

    // Intersectar el conjunto de resultados con las listas de las palabras restantes.
    for (size_t i = 1; i < palabras.size(); ++i)
    {
        // Si en algún punto el resultado se vuelve vacío, ya no hay nada que hacer.
        if (resultado.empty())
        {
            break;
        }

        NodoPosteo *lista_actual = indice.obtenerLista(palabras[i]);
        if (!lista_actual)
        {
            // Si una de las siguientes palabras no está en el índice, la intersección es vacía.
            return {};
        }

        std::set<int> temp_set;
        while (lista_actual)
        {
            // Solo nos interesan los docID que ya están en nuestro conjunto de resultados.
            if (resultado.count(lista_actual->docID))
            {
                temp_set.insert(lista_actual->docID);
            }
            lista_actual = lista_actual->siguiente;
        }

        // El nuevo conjunto de resultados es este conjunto temporal.
        resultado = temp_set;
    }

    // Convertir el conjunto final a un vector para devolverlo.
    return std::vector<int>(resultado.begin(), resultado.end());
}

void Consulta::cargarPageRank(const std::string &archivo)
{
    std::ifstream file(archivo);
    if (!file.is_open())
    {
        std::cerr << "Error: No se pudo abrir el archivo de PageRank: " << archivo << std::endl;
        return;
    }

    puntajePR.clear();
    std::string linea;
    std::getline(file, linea);

    while (std::getline(file, linea))
    {
        std::stringstream ss(linea);
        std::string id_str, score_str;

        std::getline(ss, id_str, ',');
        std::getline(ss, score_str, ',');

        try
        {
            int docID = std::stoi(id_str);
            double score = std::stod(score_str);
            puntajePR[docID] = score;
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Error al convertir los datos de PageRank: " << e.what() << " en la línea: " << linea << std::endl;
            continue; 
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Error: Valor fuera de rango en la línea: " << linea << std::endl;
            continue; 
        }
    }
    std::cout << "Puntajes de PageRank cargados para " << puntajePR.size() << " documentos." << std::endl;
}

std::vector<int> Consulta::reordenarPuntajePr(const std::vector<int> &resultados)
{
    std::vector<int> resultadosReordenados = resultados;

    // Ordenar el vector usando una función lambda que compara los puntajes de PageRank
    std::sort(resultadosReordenados.begin(), resultadosReordenados.end(),
              [this](int a, int b)
              {
                  // Obtener puntajes -> Si un doc no tiene puntaje, se le asigna 0.
                  double scoreA = puntajePR.count(a) ? puntajePR.at(a) : 0.0;
                  double scoreB = puntajePR.count(b) ? puntajePR.at(b) : 0.0;
                  // Ordenar de mayor a menor puntaje
                  return scoreA > scoreB;
              });

    return resultadosReordenados;
}