#!/bin/bash
# filepath: ejecutar.sh

cd ..
# Verificar que se proporcionen suficientes argumentos
if [ $# -lt 1 ]; then
    echo "Uso: $0 <modo>"
    echo "  <modo>: 1 para archivos individuales, 2 para archivo gov2_pages.dat"
    exit 1
fi

MODO=$1

# Verificar que los archivos existan
if [ ! -f "stopwords_espaniol.dat.txt" ]; then
    echo "Error: No se encuentra el archivo de stopwords en español"
    exit 1
fi

if [ ! -f "stopwords_english.dat.txt" ]; then
    echo "Error: No se encuentra el archivo de stopwords en inglés"
    exit 1
fi

# Si el modo es 2, verificar que existe el archivo gov2_pages.dat
if [ "$MODO" == "2" ]; then
    if [ ! -f "gov2_pages.dat" ]; then
        echo "Error: No se encuentra el archivo gov2_pages.dat"
        exit 1
    fi
fi

# Verificar que el ejecutable exista
if [ ! -f "./buscador" ]; then
    echo "Error: El ejecutable 'buscador' no existe. Ejecute primero compilar.sh"
    exit 1
fi

# Ejecutar el programa pasando el modo como argumento
# Esta línea es la clave para la compatibilidad con main.cpp
echo "Iniciando buscador con modo $MODO..."
./buscador $MODO

echo "Ejecución completada."