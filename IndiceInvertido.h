#ifndef INDICEINVERTIDO_H
#define INDICEINVERTIDO_H

#include <string>
#include <map>
#include <algorithm>

struct NodoPosteo {
    int docID;
    NodoPosteo* siguiente;
    NodoPosteo(int id) : docID(id), siguiente(nullptr) {}
};

class IndiceInvertido {
private:
    std::map<std::string, NodoPosteo*> vocabulario;
    int maxDocID;

public:
    IndiceInvertido();
    void insertarPalabra(const std::string& palabra, int docID);
    NodoPosteo* obtenerLista(const std::string& palabra);
    int obtenerTotalDocumentos() const;
    void mostrarIndice();
    ~IndiceInvertido();
};

#endif