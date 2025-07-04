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
        std::cerr << "No se pudo abrir el archivo: " << archivoPath << std::endl;
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

    std::cout << "Se procesaron " << contadorLineas << " líneas del archivo." << std::endl;
}

std::vector<std::vector<int>> ejecutarConsultaDesdeLog(const std::string &logPath, IndiceInvertido &indice, StopWord &stop, Consulta &buscador, int topK = 10)
{
    std::vector<std::vector<int>> todosLosResultados;
    std::ifstream logFile(logPath);
    if (!logFile.is_open())
    {
        std::cerr << "No se pudo abrir el archivo de log de consultas: " << logPath << std::endl;
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
                if (isalnum(c)) // Usar isalnum para caracteres alfanuméricos
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

    std::cout << "Ejecución de consultas offline completada. Resultados guardados en resultado/ResultadosConsultas.txt\n";
    return todosLosResultados;
}

void realizarComparacionRankings(Consulta &consulta, IndiceInvertido &indice, StopWord &stop)
{
    std::cout << "\n\n--- 5. Comparación de Ranking (Sin PageRank vs. Con PageRank) ---\n";

    std::vector<std::string> consultasEjemplo = {
        "search engine",
        "computer science",
        "global warming",
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
        std::cout << "Modo 1 no implementado para el reporte completo. Ejecute con modo 2.\n";
    }
    else if (modo == 2)
    {
        std::cout << "Cargando desde gov2_pages.dat...\n";
        cargarDesdeGov2("archivos/gov2_pages.dat", indice, stop);

        // --- 4. REPORTE DE MÉTRICAS ---
        std::cout << "\n--- 4. Reporte de Métricas ---\n";

        // a. y d. (Construcción del Grafo)
        std::cout << "a. Procesando consultas del log para construir el grafo...\n";
        auto inicio_grafo = std::chrono::high_resolution_clock::now();

        std::vector<std::vector<int>> resultados = ejecutarConsultaDesdeLog("archivos/Log-Queries.dat", indice, stop, buscador, 10);

        Grafo grafoCoRelevancia;
        grafoCoRelevancia.construirDesdeResultados(resultados);

        auto fin_grafo = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempo_grafo = fin_grafo - inicio_grafo;

        std::cout << "   - Consultas usadas para el grafo: " << resultados.size() << "\n";
        std::cout << "d. Tiempo de construcción del grafo: " << tiempo_grafo.count() << " segundos\n";

        // b. Métricas del grafo
        std::cout << "b. Métricas del grafo:\n";
        std::cout << "   - Nodos: " << grafoCoRelevancia.getNumeroNodos() << "\n";
        std::cout << "   - Aristas: " << grafoCoRelevancia.getNumeroAristas() << "\n";

        grafoCoRelevancia.guardarEnArchivo("resultado/GrafoCoRelevancia.txt");

        // c. y d. (Cálculo de PageRank)
        auto inicio_pr = std::chrono::high_resolution_clock::now();
        int iteraciones_pr = 100;

        grafoCoRelevancia.calcularPageRank("resultado/PageRank_Resultados.txt", 0.85, iteraciones_pr);

        auto fin_pr = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempo_pr = fin_pr - inicio_pr;
        std::cout << "c. Iteraciones para PageRank: " << iteraciones_pr << " (fijas)\n";
        std::cout << "d. Tiempo de cálculo de PageRank: " << tiempo_pr.count() << " segundos\n";

        // --- 3. USO DE PAGERANK EN EL BUSCADOR ---
        buscador.cargarPageRank("resultado/PageRank_Resultados.txt");

        // --- 5. COMPARACIÓN DE RANKING ---
        realizarComparacionRankings(buscador, indice, stop);
    }
    else
    {
        std::cerr << "Modo no válido: " << modo << ". Use 1 o 2.\n";
        return 1;
    }

    return 0;
}
/*void cargarDesdeArchivos(const std::vector<std::string> &archivos, IndiceInvertido &indice, StopWord &stopwords)
{
    ProcesadorDocumentos proc;
    for (size_t i = 0; i < archivos.size(); ++i)
    {
        std::ifstream archivo(archivos[i]);
        if (!archivo.is_open())
        {
            std::cerr << "No se pudo abrir el archivo: " << archivos[i] << "\n";
            continue;
        }
        std::stringstream buffer;
        buffer << archivo.rdbuf();
        proc.procesar(buffer.str(), static_cast<int>(i + 1), indice, stopwords);
    }
}

void cargarDesdeGov2(const std::string &archivoPath, IndiceInvertido &indice, StopWord &stopwords)
{
    std::ifstream archivo(archivoPath);
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir el archivo: " << archivoPath << std::endl;
        return;
    }
    std::string linea;
    int docID = 1;
    ProcesadorDocumentos proc;
    int contadorLineas = 0;

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

    std::cout << "Se procesaron " << contadorLineas << " líneas del archivo." << std::endl;
}

std::vector<std::vector<int>> ejecutarConsultaDesdeLog(const std::string &logPath, IndiceInvertido &indice, StopWord &stop, Consulta &buscador, int topK = 10)
{
    std::vector<std::vector<int>> todosLosResultados;
    std::ifstream logFile(logPath);
    if (!logFile.is_open())
    {
        std::cerr << "No se pudo abrir el archivo de log de consultas: " << logPath << std::endl;
        return todosLosResultados;
    }

    std::ofstream resultados("resultado/ResultadosConsultas.txt");
    if (!resultados.is_open())
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
            for (unsigned char c : palabra)
                if (letra2Ascii(c))
                    limpia += std::tolower(c);
            if (!limpia.empty() && !stop.esStopWord(limpia))
                palabras.push_back(limpia);
        }

        std::vector<int> docs;
        if (palabras.size() == 1)
            docs = buscador.buscarPalabra(palabras[0], indice);
        else if (palabras.size() > 1)
            docs = buscador.interseccion(palabras, indice);

        // Recolectar los top-K documentos para el grafo y el archivo
        std::vector<int> topDocs;
        int count = 0;
        for (int id : docs)
        {
            topDocs.push_back(id);
            if (++count >= topK)
                break;
        }
        todosLosResultados.push_back(topDocs);

        // Escribir en el archivo de resultados
        resultados << "Consulta " << numConsulta << ": " << consulta << std::endl;
        resultados << "Top " << topK << " documentos relevantes: ";
        for (int id : topDocs)
        {
            resultados << "doc" << id << " ";
        }
        resultados << std::endl
                   << std::endl;
        numConsulta++;
    }

    std::cout << "Ejecución de consultas offline completada. Resultados guardados en resultado/ResultadosConsultas.txt\n";
    return todosLosResultados;
}

void realizarComparacionRankings(Consulta &consulta, IndiceInvertido &indice)
{
    std::cout << "\n\n--- 5. Comparación de Ranking (Sin PageRank vs. Con PageRank) ---\n";

    std::vector<std::string> consultasEjemplo = {
        "search engine",
        "computer science",
        "world cup",
        "global warming",
        "health care"};

    for (const auto &query_str : consultasEjemplo)
    {
        std::cout << "\n======================================================\n";
        std::cout << "Consulta: \"" << query_str << "\"\n";
        std::cout << "======================================================\n";

        ProcesadorDocumentos p;
        std::vector<std::string> terminos = p.procesar(query_str);

        // Búsqueda normal
        std::vector<int> resultados_normal = consulta.interseccion(terminos, indice);

        // Búsqueda con PageRank
        std::vector<int> resultados_pagerank = consulta.reordenarPuntajePr(resultados_normal);

        std::cout << "Top 5 Sin PageRank:\t\tTop 5 Con PageRank:\n";
        std::cout << "---------------------\t\t---------------------\n";
        for (int i = 0; i < 5; ++i)
        {
            // Imprimir resultado sin el pagerankkk
            if (i < resultados_normal.size())
            {
                std::cout << "doc" << resultados_normal[i];
            }
            std::cout << "\t\t\t\t";
            // Imprimir resultado con PageRankkkk
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
        std::cout << "Cargando desde archivos individuales...\n";
        std::vector<std::string> archivos = {
            "archivos/doc1.txt",
            "archivos/doc2.txt",
            "archivos/doc3.txt"};
        cargarDesdeArchivos(archivos, indice, stop);
    }
    else if (modo == 2)
    {
        /*std::cout << "Cargando desde gov2_pages.dat...\n";
        cargarDesdeGov2("archivos/gov2_pages.dat", indice, stop);

        std::cout << "\nEjecutando log de consultas offline...\n";
        std::vector<std::vector<int>> resultados = ejecutarConsultaDesdeLog("archivos/Log-Queries.dat", indice, stop, buscador, 10);

        Grafo grafoCoRelevancia;
        grafoCoRelevancia.construirDesdeResultados(resultados);

        std::cout << "\n-----------------------------------\n";
        grafoCoRelevancia.mostrarGrafo();
        
        std::cout << "Cargando desde gov2_pages.dat...\n";
        cargarDesdeGov2("archivos/gov2_pages.dat", indice, stop);

        std::cout << "\nEjecutando log de consultas offline...\n";
        std::vector<std::vector<int>> resultados = ejecutarConsultaDesdeLog("archivos/Log-Queries.dat", indice, stop, buscador, 10);

        // Obtener el número total de documentos del índice
        //int totalDocumentos = indice.obtenerTotalDocumentos();

        // Crear el grafo con el tamaño correcto para la matriz
        Grafo grafoCoRelevancia;
        grafoCoRelevancia.construirDesdeResultados(resultados);

        std::cout << "\n-----------------------------------\n";
        //grafoCoRelevancia.mostrarGrafo();

        grafoCoRelevancia.guardarEnArchivo("resultado/GrafoCoRelevancia.txt");
        grafoCoRelevancia.calcularPageRank("resultado/PageRank_Resultados.txt");

        // --- 4. REPORTE DE MÉTRICAS ---
        std::cout << "\n--- 4. Reporte de Métricas ---\n";

        // a. Número total de consultas
        std::cout << "a. Procesando consultas del log para construir el grafo...\n";
        auto inicio_grafo = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<int>> resultados = ejecutarConsultaDesdeLog("archivos/Log-Queries.dat", indice, stop, buscador, 10);
        std::cout << "   - Consultas usadas para el grafo: " << resultados.size() << "\n";

        // d. Tiempo de construcción del grafo
        Grafo grafoCoRelevancia;
        grafoCoRelevancia.construirDesdeResultados(resultados);
        auto fin_grafo = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempo_grafo = fin_grafo - inicio_grafo;
        std::cout << "d. Tiempo de construcción del grafo: " << tiempo_grafo.count() << " segundos\n";

        // b. Número de nodos y aristas
        std::cout << "b. Métricas del grafo:\n";
        std::cout << "   - Nodos: " << grafoCoRelevancia.getNumeroNodos() << "\n";
        std::cout << "   - Aristas: " << grafoCoRelevancia.getNumeroAristas() << "\n";

        // c. y d. Cálculo de PageRank y tiempo
        auto inicio_pr = std::chrono::high_resolution_clock::now();
        int iteraciones_pr = 100;
        grafoCoRelevancia.calcularPageRank("resultado/PageRank_Resultados.txt", 0.85, iteraciones_pr);
        auto fin_pr = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempo_pr = fin_pr - inicio_pr;
        std::cout << "c. Iteraciones para PageRank: " << iteraciones_pr << " (fijas)\n";
        std::cout << "d. Tiempo de cálculo de PageRank: " << tiempo_pr.count() << " segundos\n";

        // --- 3. USO DE PAGERANK EN EL BUSCADOR ---
        // Cargar los puntajes para poder usarlos
        Consulta.cargarPageRank("resultado/PageRank_Resultados.txt");

        // --- 5. COMPARACIÓN DE RANKING ---
        realizarComparacionRankings(consulta, indice);
    }
    else
    {
        std::cerr << "Modo no válido: " << modo << ". Use 1 o 2.\n";
        return 1;
    }

    return 0;
}*/