#include "Grafo.h"
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>

// Constructor simple
Grafo::Grafo() {}

void Grafo::agregarArista(int u, int v) {
    // Grafo no dirigido: incrementamos el peso en ambas direcciones
    // Si la entrada no existe, el mapa la crea automáticamente con valor 0 antes de incrementar.
    listaAdyacencia[u][v]++;
    listaAdyacencia[v][u]++;
}

// ¡Esta función no necesita ningún cambio! Su lógica sigue siendo válida.
void Grafo::construirDesdeResultados(const std::vector<std::vector<int>> &resultadosConsultas)
{
    for (const auto &conjuntoResultados : resultadosConsultas)
    {
        for (size_t i = 0; i < conjuntoResultados.size(); ++i)
        {
            for (size_t j = i + 1; j < conjuntoResultados.size(); ++j)
            {
                agregarArista(conjuntoResultados[i], conjuntoResultados[j]);
            }
        }
    }
}

void Grafo::mostrarGrafo() const
{
    std::cout << "Lista de Adyacencia (Grafo de Co-Relevancia):\n";
    std::cout << "Nodo <-> Vecino: Peso\n";
    // Iterar sobre cada nodo 'u' en el grafo
    for (const auto& par_u : listaAdyacencia) {
        int u = par_u.first;
        const auto& vecinos = par_u.second;
        // Iterar sobre los vecinos 'v' de 'u'
        for (const auto& par_v : vecinos) {
            int v = par_v.first;
            int peso = par_v.second;
            // Para no imprimir aristas duplicadas (u->v y v->u), solo mostramos si u < v
            if (u < v) {
                std::cout << "doc" << u << " <-> doc" << v << ": " << peso << std::endl;
            }
        }
    }
}

void Grafo::guardarEnArchivo(const std::string& nombreArchivo){
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para guardar el grafo: " << nombreArchivo << std::endl;
        return;
    }

    archivo << "Grafo de Co-Relevancia (Lista de Adyacencia)\n";
    archivo << "---------------------------------------------\n";
    archivo << "Nodo <-> Vecino: Peso (veces que aparecen juntos en top-K)\n\n";

    for (const auto& par_u : listaAdyacencia) {
        int u = par_u.first;
        const auto& vecinos = par_u.second;
        for (const auto& par_v : vecinos) {
            int v = par_v.first;
            int peso = par_v.second;
            if (u < v) {
                archivo << "doc" << u << " <-> doc" << v << ": " << peso << "\n";
            }
        }
    }
    std::cout << "Grafo de co-relevancia guardado en '" << nombreArchivo << "'." << std::endl;
}

void Grafo::calcularPageRank(const std::string &nombreArchivo, double d, int iteraciones)
{
    if (listaAdyacencia.empty())
    {
        std::cout << "El grafo está vacío, no se puede calcular PageRank." << std::endl;
        return;
    }

    std::map<int, double> pageRankScores;
    int numNodos = listaAdyacencia.size();

    // 1. Inicialización: todos los nodos empiezan con el mismo PageRank.
    for (const auto &par : listaAdyacencia)
    {
        pageRankScores[par.first] = 1.0 / numNodos;
    }

    std::cout << "\nCalculando PageRank con d=" << d << " y " << iteraciones << " iteraciones..." << std::endl;

    // 2. Proceso iterativo
    for (int i = 0; i < iteraciones; ++i)
    {
        std::map<int, double> nextPageRankScores;
        double sumaColgantes = 0.0;

        // Para cada nodo en el grafo...
        for (const auto &par_A : listaAdyacencia)
        {
            int nodo_A = par_A.first;
            double sumaVecinos = 0.0;

            // Calcular la suma de PR(Ti) / C(Ti)
            // Como el grafo es no dirigido, los nodos que apuntan a A son sus vecinos.
            for (const auto &par_vecino : par_A.second)
            {
                int nodo_Ti = par_vecino.first;
                int grado_C_Ti = listaAdyacencia.at(nodo_Ti).size(); // Grado de salida del vecino
                sumaVecinos += pageRankScores[nodo_Ti] / grado_C_Ti;
            }

            // Aplicar la fórmula de PageRank
            nextPageRankScores[nodo_A] = (1.0 - d) / numNodos + d * sumaVecinos;
        }
        // Actualizar los puntajes para la siguiente iteración
        pageRankScores = nextPageRankScores;
    }

    // 3. Preparar y guardar los resultados
    std::vector<std::pair<int, double>> resultadosOrdenados(pageRankScores.begin(), pageRankScores.end());

    // Ordenar los resultados de mayor a menor puntaje
    std::sort(resultadosOrdenados.begin(), resultadosOrdenados.end(),
              [](const auto &a, const auto &b)
              {
                  return a.second > b.second;
              });

    // Guardar en archivo
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open())
    {
        std::cerr << "Error: No se pudo abrir el archivo para guardar PageRank: " << nombreArchivo << std::endl;
        return;
    }

    archivo << "ID_Documento,Puntaje_PageRank\n";
    for (const auto &par : resultadosOrdenados)
    {
        archivo << par.first << "," << std::fixed << std::setprecision(10) << par.second << "\n";
    }

    std::cout << "Resultados de PageRank guardados en '" << nombreArchivo << "'." << std::endl;

    // Mostrar el top 20 en consola
    std::cout << "\n--- Top 20 Documentos por PageRank ---" << std::endl;
    for (int i = 0; i < 20 && i < resultadosOrdenados.size(); ++i)
    {
        std::cout << std::setw(2) << i + 1 << ". Documento " << std::setw(5) << resultadosOrdenados[i].first
                  << ": PageRank = " << std::fixed << std::setprecision(6) << resultadosOrdenados[i].second << std::endl;
    }
}
//metricas
int Grafo::getNumeroNodos()
{
    return listaAdyacencia.size();
}

int Grafo::getNumeroAristas()
{
    int totalAristas = 0;
    for (const auto &par : listaAdyacencia)
    {
        totalAristas += par.second.size();
    }
    // Como cada arista se guarda dos veces (u->v y v->u), dividimos por 2.
    return totalAristas / 2;
}