#ifndef GRAFO_H
#define GRAFO_H

#include <vector>
#include <iostream>
#include <string>
#include <map> 

class Grafo
{
private:
    std::map<int, std::map<int, int>> listaAdyacencia;
public:
    Grafo();

    void agregarArista(int u, int v);
    void construirDesdeResultados(const std::vector<std::vector<int>> &resultadosConsultas);
    void mostrarGrafo() const;
    void guardarEnArchivo(const std::string &nombreArchivo);
    //metodos para aplicar e implementar en pagerank --- ya tah
    void calcularPageRank(const std::string& nombreArchivo, double d = 0.85, int iteraciones = 100);
    
    //metricas para page rankkk
    int getNumeroNodos();
    int getNumeroAristas();
};

#endif // GRAFO_H