CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/usr/include/c++/13 -I/usr/include
SRC = main.cpp IndiceInvertido.cpp ProcesadorDocumentos.cpp StopWord.cpp Consulta.cpp grafo.cpp
HEADERS = IndiceInvertido.h ProcesadorDocumentos.h StopWord.h Consulta.h grafo.h
EXEC = buscador

all: $(EXEC)

$(EXEC): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXEC)

clean:
	rm -f $(EXEC)