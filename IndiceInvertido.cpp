/*#include "IndiceInvertido.h"
#include <iostream>
#include <algorithm>

IndiceInvertido::IndiceInvertido() : maxDocID(0) {}

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
}*/

#include "IndiceInvertido.h"
#include <iostream>
#include <algorithm> // Para std::max

// Constructor: inicializa maxDocID en 0
IndiceInvertido::IndiceInvertido() : maxDocID(0) {}

// Destructor: libera la memoria de las listas de posteo
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

void IndiceInvertido::insertarPalabra(const std::string &palabra, int docID)
{
    // Actualiza el ID de documento más alto visto hasta ahora
    this->maxDocID = std::max(this->maxDocID, docID);

    NodoPosteo *&cabeza = vocabulario[palabra];

    // Si la lista está vacía, crea el primer nodo y termina
    if (!cabeza)
    {
        cabeza = new NodoPosteo(docID);
        return;
    }

    // Recorre la lista para evitar insertar un docID duplicado
    NodoPosteo *actual = cabeza;
    while (true)
    {
        if (actual->docID == docID)
        {
            return; // El docID ya existe en la lista, no hacer nada
        }
        if (!actual->siguiente)
        {
            break; // Llegamos al final de la lista
        }
        actual = actual->siguiente;
    }

    // Inserta el nuevo nodo al final de la lista
    actual->siguiente = new NodoPosteo(docID);
}

// Devuelve el ID de documento más alto
int IndiceInvertido::obtenerTotalDocumentos() const
{
    return this->maxDocID;
}

// Obtiene la lista de posteo para una palabra
NodoPosteo *IndiceInvertido::obtenerLista(const std::string &palabra)
{
    auto it = vocabulario.find(palabra);
    if (it != vocabulario.end())
    {
        return it->second;
    }
    return nullptr;
}

// Muestra el contenido del índice invertido
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