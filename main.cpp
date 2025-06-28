#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "IndiceInvertido.h"
#include "ProcesadorDocumentos.h"
#include "StopWord.h"
#include "Consulta.h"

void cargarDesdeArchivos(const std::vector<std::string> &archivos, IndiceInvertido &indice, StopWord &stopwords)
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
    std::string linea;
    int docID = 1;
    ProcesadorDocumentos proc;
    int contadorLineas = 0; // Contador para limitar a 1000 líneas

    while (std::getline(archivo, linea) && contadorLineas < 3000) // Limitar a 3000 líneas por el tamaño del archivo
    {
        size_t pos = linea.find_last_of("||");
        if (pos != std::string::npos && pos + 2 < linea.size())
        {
            std::string contenido = linea.substr(pos + 2);
            proc.procesar(contenido, docID, indice, stopwords);
            ++docID;
        }
        ++contadorLineas; // Incrementar el contador por cada línea leída
    }

    std::cout << "Se procesaron " << contadorLineas << " líneas del archivo." << std::endl;
}

int main(int argc, char *argv[])
{
    int modo = 1;

    if (argc == 1)
    {
        modo = std::atoi(argv[1]);
    }else if (argc == 2)
    {
        modo = std::atoi(argv[2]);
    }
    
    IndiceInvertido indice;
    StopWord stop;
    Consulta buscador;

    std::string opcion;
    std::cout << "Selecciona la fuente de documentos:\n";
    std::cout << "1. Archivos individuales (doc1.txt, doc2.txt, doc3.txt)\n";
    std::cout << "2. Archivo gov2_pages.dat\n> ";
    std::getline(std::cin, opcion);

    // hacer una logica para stopwords en inglés y en español NO OLVIDAR (listo)
    stop.cargarDesdeArchivo("stopwords_english.dat.txt");
    stop.cargarDesdeArchivo("stopwords_espaniol.dat.txt");

    if (opcion == "1")
    {
        std::vector<std::string> archivos = {
            "doc1.txt",
            "doc2.txt",
            "doc3.txt"};
        cargarDesdeArchivos(archivos, indice, stop);
    }
    else
    {
        cargarDesdeGov2("gov2_pages.dat", indice, stop);
    }

    std::cout << "-----------------------------------";
    std::cout << "\nÍndice invertido construido:\n";
    indice.mostrarIndice();
    std::cout << "\n-----------------------------------\n";
    std::string entrada;
    std::cout << "\nConsulta interactiva (escribe 'salir' para terminar):\n";

    while (true)
    {
        std::cout << "\n> ";
        std::getline(std::cin, entrada);
        if (entrada == "salir")
        {
            break;
        }

        std::stringstream ss(entrada);
        std::string palabra;
        std::vector<std::string> palabras;
        while (ss >> palabra)
        {
            std::string limpia;
            for (unsigned char c : palabra)
                if (letra2Ascii(c))
                {
                    limpia += std::tolower(c);
                }
            if (!limpia.empty() && !stop.esStopWord(limpia))
            {
                palabras.push_back(limpia);
            }
        }

        if (palabras.empty())
        {
            std::cout << "No se ingresaron palabras.\n";
        }
        else if (palabras.size() == 1)
        {
            auto docs = buscador.buscarPalabra(palabras[0], indice);
            std::cout << "La palabra '" << palabras[0] << "' aparece en los documentos: \n";

            for (int id : docs)
                std::cout << "doc" << id << " ";
            std::cout << "\n";
        }
        else
        {
            auto docs = buscador.interseccion(palabras, indice);
            std::cout << "Los documentos que contienen todas las palabras son: ";
            for (int id : docs)
                std::cout << "doc" << id << " ";
            std::cout << "\n";
        }
    }

    return 0;
}