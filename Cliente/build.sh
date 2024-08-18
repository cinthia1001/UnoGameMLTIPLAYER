#!/bin/bash

# Limpiar los archivos generados por la compilación anterior
clean() {
    echo "Limpiando archivos generados..."
    if [ -f Makefile ]; then
        make clean
    fi
    rm -rf *.o *.obj moc_* ui_* qrc_* *.a *.so *.dll *.dylib *.exe *.app
    rm -rf Makefile
    echo "Archivos limpiados."
}

# Compilar el proyecto
build() {
    echo "Generando Makefile con qmake6..."
    qmake6 || { echo "Error al ejecutar qmake6"; exit 1; }
    
    echo "Compilando el proyecto..."
    make || { echo "Error al compilar el proyecto"; exit 1; }
    
    echo "Compilación completa."
}

# Ejecutar el ejecutable generado
run() {
    # Buscar el ejecutable generado
    executable=$(find . -maxdepth 1 -type f -executable -name '*')
    
    if [ -z "$executable" ]; then
        echo "No se encontró ningún ejecutable para ejecutar."
    else
        echo "Ejecutando el programa: $executable"
        ./$executable
    fi
}

# Ejecutar las funciones
clean
build
run
