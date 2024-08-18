#ifndef DECK_H
#define DECK_H

#include <QString>
#include <QPixmap>
#include <QList>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <algorithm>
#include <random>
#include <iostream>

struct Card {
    QString name;      // Nombre de la carta (por ejemplo, "dos_amarillo")
    QString imagePath; // Ruta a la imagen de la carta (por ejemplo, "/home/eduardoramirez/Documentos/QUno/src/dos_amarillo.png")
};

// Crear el mazo de cartas
inline QList<Card> createDeck() {
    QList<Card> deck;

    // Ruta base donde se encuentran las imágenes
    QString basePath = "/home/eduardoramirez/Documentos/QUno/src/";

    // Cartas numéricas
    QStringList colors = {"amarillo", "azul", "rojo", "verde"};
    QStringList numbers = {"cero", "uno", "dos", "tres", "cuatro", "cinco", "seis", "siete", "ocho", "nueve"};

    for (const QString &color : colors) {
        for (const QString &number : numbers) {
            QString name = number + "_" + color;
            QString imagePath = basePath + name + ".png";
            deck.append({name, imagePath});
        }
    }

    // Cartas especiales
    QStringList specialNames = {"cambia_color", "mas_cuatro"};
    for (const QString &specialName : specialNames) {
        deck.append({specialName, basePath + specialName + ".png"});
    }

    QStringList specialCards = {"cambia_sentido", "mas_dos", "pierde_turno"};
    for (const QString &color : colors) {
        for (const QString &specialCard : specialCards) {
            QString name = specialCard + "_" + color;
            QString imagePath = basePath + name + ".png";
            deck.append({name, imagePath});
        }
    }

    return deck;
}

// Barajar el mazo de cartas
inline void shuffleDeck(QList<Card> &deck) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

// Mostrar el mazo en la pantalla de juego
inline void displayDeck(QWidget *parent, const QList<Card> &deck) {
    QVBoxLayout *layout = new QVBoxLayout(parent);

    // Crear un layout en cuadrícula para mostrar las cartas
    QGridLayout *gridLayout = new QGridLayout();

    int row = 0;
    int col = 0;

    for (const Card &card : deck) {
        QLabel *cardLabel = new QLabel(parent);
        QPixmap pixmap(card.imagePath);
        cardLabel->setPixmap(pixmap.scaled(100, 150)); // Escalar la imagen de la carta
        gridLayout->addWidget(cardLabel, row, col);

        // Ajustar las posiciones en la cuadrícula
        if (++col == 5) {  // Cambiar la cantidad de columnas según sea necesario
            col = 0;
            ++row;
        }
    }

    layout->addLayout(gridLayout);
    parent->setLayout(layout);
}

// Función para imprimir las cartas en la consola
inline void printDeckToConsole(const QList<Card> &deck) {
    std::cout << "Cartas en el mazo:" << std::endl;
    for (const Card &card : deck) {
        std::cout << card.name.toStdString() << std::endl;
    }
}

#endif // DECK_H
