#ifndef STOPWORD_H
#define STOPWORD_H

#include <set>
#include <string>

class StopWord {
private:
    std::set<std::string> palabrasStop;
    void cargarPorDefecto();

public:
    void cargarDesdeArchivo(const std::string& archivo);
    bool esStopWord(const std::string& palabra);
};

#endif