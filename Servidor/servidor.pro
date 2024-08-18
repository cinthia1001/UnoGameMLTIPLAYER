TEMPLATE = app
TARGET = ServerApp
INCLUDEPATH += .

QT += core gui network widgets

SOURCES += main.cpp \
           server.cpp

HEADERS += server.h \
                deck.h
