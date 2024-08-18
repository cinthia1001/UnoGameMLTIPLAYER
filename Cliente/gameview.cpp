#include "gameview.h"
#include <QDebug>
#include <QPushButton>  // Incluir QPushButton para manejar los botones de las cartas

GameView::GameView(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // Inicializar el layout para mostrar a todos los jugadores
    playersLayout = new QVBoxLayout();
    
    gameLabel = new QLabel("¡El juego ha comenzado!", this);
    gameLabel->setAlignment(Qt::AlignCenter);

    handLayout = new QHBoxLayout();  // Layout para las cartas en mano

    mainLayout->addLayout(playersLayout);  // Jugadores a la izquierda
    mainLayout->addWidget(gameLabel);  // Mensaje del juego al centro
    mainLayout->addLayout(handLayout);  // Cartas a la derecha
    mainLayout->setAlignment(playersLayout, Qt::AlignLeft);
    mainLayout->setAlignment(gameLabel, Qt::AlignCenter);
    
    setLayout(mainLayout);
}

void GameView::addPlayerDisplay(const QString &name, const QPixmap &avatar, bool isCurrentPlayer) {
    PlayerDisplayInfo playerDisplay;

    playerDisplay.avatarLabel = new QLabel(this);
    playerDisplay.avatarLabel->setFixedSize(100, 100);
    playerDisplay.avatarLabel->setScaledContents(true);
    playerDisplay.avatarLabel->setStyleSheet("border-radius: 50px; border: 2px solid gray;");
    playerDisplay.avatarLabel->setPixmap(avatar);

    playerDisplay.nameLabel = new QLabel(name, this);

    if (isCurrentPlayer) {
        playerDisplay.nameLabel->setStyleSheet("color: red; font-weight: bold;");
    } else {
        playerDisplay.nameLabel->setStyleSheet("color: black;");
    }

    playerDisplay.nameLabel->setAlignment(Qt::AlignLeft);

    QVBoxLayout *playerLayout = new QVBoxLayout();
    playerLayout->addWidget(playerDisplay.avatarLabel);
    playerLayout->addWidget(playerDisplay.nameLabel);
    playerLayout->setAlignment(Qt::AlignCenter);

    playersLayout->addLayout(playerLayout);
    playerDisplays.append(playerDisplay);
}

void GameView::displayHand(const QList<Card> &hand) {
    // Limpiar el layout anterior
    QLayoutItem *child;
    while ((child = handLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    cardButtons.clear();  // Limpiar la lista de botones anteriores

    // Mostrar las cartas
    for (const Card &card : hand) {
        QPushButton *cardButton = new QPushButton(this);
        QPixmap pixmap(card.imagePath);

        if (pixmap.isNull()) {
            qDebug() << "Error: No se pudo cargar la imagen en la ruta:" << card.imagePath;
            pixmap = QPixmap(100, 150);
            pixmap.fill(Qt::gray);
        }

        cardButton->setIcon(pixmap.scaled(100, 150));
        cardButton->setIconSize(QSize(100, 150));
        cardButton->setEnabled(false);  // Inicialmente deshabilitado hasta que sea el turno del jugador

        connect(cardButton, &QPushButton::clicked, [this, card]() {
            // Aquí puedes emitir una señal o llamar a una función para jugar la carta
            qDebug() << "Carta jugada:" << card.name;
        });

        cardButtons.append(cardButton);
        handLayout->addWidget(cardButton);
    }
}

void GameView::enablePlay() {
    // Habilitar todos los botones de cartas para que el jugador pueda jugar
    for (QPushButton *button : cardButtons) {
        button->setEnabled(true);
    }
}

