#include "IndiceInvertido.h"
#include <iostream>
#include <algorithm>

void IndiceInvertido::insertarPalabra(const std::string &palabra, int docID)
{
    NodoPosteo *&cabeza = vocabulario[palabra];
    NodoPosteo *actual = cabeza;

    while (actual)
    {
        if (actual->docID == docID)
        {
            return;
        } // ya tah
        if (!actual->siguiente)
        {
            break;
        }
        actual = actual->siguiente;
    }

    NodoPosteo *nuevo = new NodoPosteo(docID);
    if (!cabeza)
    {
        cabeza = nuevo;
    }
    else
    {
        actual->siguiente = nuevo;
    }
}

NodoPosteo *IndiceInvertido::obtenerLista(const std::string &palabra)
{
    auto it = vocabulario.find(palabra);
    if (it != vocabulario.end())
    {
        return it->second;
    }
    return nullptr;
}



void IndiceInvertido::mostrarIndice()
{
    for (const auto &par : vocabulario)
    {
        std::cout << par.first << " -> ";
        NodoPosteo *actual = par.second;
        while (actual)
        {
            std::cout << "[doc" << actual->docID << "] ";
            actual = actual->siguiente;
        }
        std::cout << "\n";
    }
}

IndiceInvertido::~IndiceInvertido()
{
    for (auto &par : vocabulario)
    {
        NodoPosteo *actual = par.second;
        while (actual)
        {
            NodoPosteo *temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }
}