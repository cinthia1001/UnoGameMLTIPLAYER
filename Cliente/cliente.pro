TEMPLATE = app
TARGET = ClientApp

QT += core gui network widgets

# Detectar la ruta del archivo 'deck.h' automáticamente
SERVIDOR_DIR = $$PWD/../Servidor
INCLUDEPATH += $$SERVIDOR_DIR

SOURCES += main.cpp \
           client.cpp \
           gameview.cpp

HEADERS += client.h \
           gameview.h \
           $$SERVIDOR_DIR/deck.h  # Añadir deck.h automáticamente
