#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <cctype> // Para temas de numeros y mayusculas a minusculas
#include "IndiceInvertido.h"
#include "ProcesadorDocumentos.h"
#include "StopWord.h"
#include "Consulta.h"
#include "Grafo.h"

void cargarDesdeGov2(const std::string &archivoPath, IndiceInvertido &indice, StopWord &stopwords)
{
    std::ifstream archivo(archivoPath);
    if (!archivo.is_open())
    {
        std::cerr << "ERROR: No se pudo abrir el archivo: " << archivoPath << std::endl;
        return;
    }
    std::string linea;
    int docID = 1;
    ProcesadorDocumentos proc;
    int contadorLineas = 0;

    // Limitar a 3000 líneas para una ejecución de prueba más rápida
    while (std::getline(archivo, linea) && contadorLineas < 3000)
    {
        size_t pos = linea.find_last_of("||");
        if (pos != std::string::npos && pos + 2 < linea.size())
        {
            std::string contenido = linea.substr(pos + 2);
            proc.procesar(contenido, docID, indice, stopwords);
            ++docID;
        }
        ++contadorLineas;
    }

    std::cout << "Se procesaron " << contadorLineas << " lineas" << std::endl;
}

std::vector<std::vector<int>> ejecutarConsultaDesdeLog(const std::string &logPath, IndiceInvertido &indice, StopWord &stop, Consulta &buscador, int topK = 10)
{
    std::vector<std::vector<int>> todosLosResultados;
    std::ifstream logFile(logPath);
    if (!logFile.is_open())
    {
        std::cerr << "No se pudo abrir log de consultas: " << logPath << std::endl;
        return todosLosResultados;
    }

    std::ofstream resultadosFile("resultado/ResultadosConsultas.txt");
    if (!resultadosFile.is_open())
    {
        std::cerr << "No se pudo crear el archivo de resultados." << std::endl;
        return todosLosResultados;
    }

    std::string consulta;
    int numConsulta = 1;
    while (std::getline(logFile, consulta))
    {
        std::stringstream ss(consulta);
        std::string palabra;
        std::vector<std::string> palabras;
        while (ss >> palabra)
        {
            std::string limpia;
            for (char c : palabra)
            {
                if (isalnum(c)) //para caracteres alfanuméricos
                {
                    limpia += tolower(c);
                }
            }
            if (!limpia.empty() && !stop.esStopWord(limpia))
            {
                palabras.push_back(limpia);
            }
        }

        std::vector<int> docs;
        if (palabras.size() == 1)
            docs = buscador.buscarPalabra(palabras[0], indice);
        else if (palabras.size() > 1)
            docs = buscador.interseccion(palabras, indice);

        std::vector<int> topDocs;
        int count = 0;
        for (int id : docs)
        {
            if (++count > topK)
                break;
            topDocs.push_back(id);
        }
        todosLosResultados.push_back(topDocs);

        resultadosFile << "Consulta " << numConsulta << ": " << consulta << std::endl;
        resultadosFile << "Top " << topK << " documentos relevantes: ";
        for (int id : topDocs)
        {
            resultadosFile << "doc" << id << " ";
        }
        resultadosFile << std::endl
                       << std::endl;
        numConsulta++;
    }

    std::cout << "Ejecucion de consultas offline completada. Resultados guardados en resultado/ResultadosConsultas.txt\n";
    return todosLosResultados;
}

void realizarComparacionRanking(Consulta &consulta, IndiceInvertido &indice, StopWord &stop)
{
    std::cout << "\n\n--- Comparacion de Ranking (Sin PageRank v/s Con PageRank) ---\n";

    std::vector<std::string> consultasEjemplo = {
        "WW2",
        "WWW",
        "global",
        "health care",
        "zoo",
        "austin texas"};

    for (const auto &query_str : consultasEjemplo)
    {
        std::cout << "\n======================================================\n";
        std::cout << "Consulta: \"" << query_str << "\"\n";
        std::cout << "======================================================\n";

        std::vector<std::string> terminos;
        std::stringstream ss(query_str);
        std::string palabra;
        while (ss >> palabra)
        {
            std::string limpia;
            for (char c : palabra)
            {
                if (isalnum(c))
                {
                    limpia += tolower(c);
                }
            }
            if (!limpia.empty() && !stop.esStopWord(limpia))
            {
                terminos.push_back(limpia);
            }
        }

        std::vector<int> resultados_normal = consulta.interseccion(terminos, indice);
        std::vector<int> resultados_pagerank = consulta.reordenarPuntajePr(resultados_normal);

        std::cout << "Top 5 Sin PageRank:\t\tTop 5 Con PageRank:\n";
        std::cout << "---------------------\t\t---------------------\n";
        for (int i = 0; i < 5; ++i)
        {
            if (i < resultados_normal.size())
            {
                std::cout << "doc" << resultados_normal[i];
            }
            std::cout << "\t\t\t\t";
            if (i < resultados_pagerank.size())
            {
                std::cout << "doc" << resultados_pagerank[i];
            }
            std::cout << "\n";
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <modo>\n";
        std::cerr << "  <modo>: 1 para archivos individuales, 2 para archivo gov2_pages.dat\n";
        return 1;
    }

    int modo = std::atoi(argv[1]);

    IndiceInvertido indice;
    StopWord stop;
    Consulta buscador;

    stop.cargarDesdeArchivo("archivos/stopwords_english.dat.txt");
    stop.cargarDesdeArchivo("archivos/stopwords_espaniol.dat.txt");

    if (modo == 1)
    {
        std::cout << "No se requiere el modo 1 por temas del proyecto. Ejecute con modo 2.\n";
    }
    else if (modo == 2)
    {
        std::cout << "Cargando desde gov2_pages.dat...\n";
        cargarDesdeGov2("archivos/gov2_pages.dat", indice, stop);

        // --- REPORTE DE MÉTRICAS ---
        std::cout << "\n--- Reporte de Metricas ---\n";

        // a. y d. (Construcción del Grafo)
        std::cout << "...Procesando consultas del log para construir el grafo...\n";
        auto inicio_grafo = std::chrono::high_resolution_clock::now();

        std::vector<std::vector<int>> resultados = ejecutarConsultaDesdeLog("archivos/Log-Queries.dat", indice, stop, buscador, 10);

        Grafo grafoCoRelevancia;
        grafoCoRelevancia.construirDesdeResultados(resultados);

        auto fin_grafo = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempo_grafo = fin_grafo - inicio_grafo;

        std::cout << "Consultas usadas para el grafo: " << resultados.size() << "\n";
        std::cout << "Tiempo de construcción del grafo: " << tiempo_grafo.count() << " segundos\n";

        //Métricas del grafo
        std::cout << "Métricas del grafo:\n";
        std::cout << "Nodos: " << grafoCoRelevancia.getNumeroNodos() << "\n";
        std::cout << "Aristas: " << grafoCoRelevancia.getNumeroAristas() << "\n";

        grafoCoRelevancia.guardarEnArchivo("resultado/GrafoCoRelevancia.txt");

        //Cálculo de tiempo pageranl
        auto inicio_pr = std::chrono::high_resolution_clock::now();
        int iteraciones_pr = 100;

        grafoCoRelevancia.calcularPageRank("resultado/PageRank_Resultados.txt", 0.85, iteraciones_pr);

        auto fin_pr = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempo_pr = fin_pr - inicio_pr;
        std::cout << "Iteraciones para PageRank: " << iteraciones_pr << " (fijas)\n";
        std::cout << "Tiempo de calculo de PageRank: " << tiempo_pr.count() << " segundos\n";

        // --- uso pageRan enbuscador ---
        buscador.cargarPageRank("resultado/PageRank_Resultados.txt");

        // comparacion de ranking ---
        realizarComparacionRanking(buscador, indice, stop);
    }
    else
    {
        std::cerr << "Modo no valido: " << modo << ". Use 1 o 2.\n";
        return 1;
    }

    return 0;
}
