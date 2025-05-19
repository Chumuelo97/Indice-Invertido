#include "StopWord.h"
#include <fstream>
#include <algorithm>

void StopWord::cargarDesdeArchivo(const std::string& archivo) {
    std::ifstream file(archivo);
    if(!file.is_open()) {
        cargarPorDefecto();//<- por si no abre el archivo stopwords
        return;
    }

    std::string palabra;
    while (file >> palabra) {
        std::transform(palabra.begin(), palabra.end(), palabra.begin(), ::tolower);
        palabrasStop.insert(palabra);
    }
    cargarPorDefecto(); 
}

bool StopWord::esStopWord(const std::string& palabra) {
    return palabrasStop.count(palabra);
}

void StopWord::cargarPorDefecto() {
    // No borrar, se incluyen para el correcto funcionamiento del programa
    const std::string especiales[] = {
        // Preposiciones en español
        "a", "ante", "bajo", "cabe", "con", "contra", "de", "desde",
        "durante", "en", "entre", "hacia", "hasta", "mediante",
        "para", "por", "según", "sin", "so", "sobre", "tras",
        "versus", "vía", "acerca", "alrededor", "aparte", "arriba",
        "abajo", "adentro", "afuera", "adelante", "atrás", "cerca",
        "debajo", "delante", "dentro", "detrás", "encima", "enfrente",
        "fuera", "lejos", "respecto", "junto",
        // Stopwords básicas en inglés
        "am", "and", "an", "are", "as", "at", "be", "by", "for", "from",
        "has", "he", "in", "is", "it", "its", "of", "on", "that", "the",
        "to", "was", "were", "will", "with", "i", "you", "your", "we", "my",
        "me", "this", "there", "what", "when", "who", "how", "why", "which"};
    for (const auto& p : especiales) {palabrasStop.insert(p);}
}