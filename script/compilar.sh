#!/bin/bash
# filepath: script/compilar.sh

# Ir al directorio principal donde está el Makefile
cd ..

# Compilar el programa usando make
make clean
make

# Verificar si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "Compilación exitosa. El ejecutable 'buscador' ha sido creado."
else
    echo "Error durante la compilación."
    exit 1
fi