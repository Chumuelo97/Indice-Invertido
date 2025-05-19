#ifndef INDICEINVERTIDO_H
#define INDICEINVERTIDO_H

#include <string>
#include <map>

struct NodoPosteo {
    int docID;
    NodoPosteo* siguiente;
    NodoPosteo(int id) : docID(id), siguiente(nullptr) {}
};

class IndiceInvertido {
private:
    std::map<std::string, NodoPosteo*> vocabulario;

public:
    void insertarPalabra(const std::string& palabra, int docID);
    NodoPosteo* obtenerLista(const std::string& palabra);
    void mostrarIndice();
    ~IndiceInvertido();
};

#endif