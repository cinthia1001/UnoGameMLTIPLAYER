#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QString>
#include <QList>
#include <QPushButton>  // Incluir QPushButton para manejar los botones de las cartas
#include "deck.h"       // Incluir el archivo donde está definido el struct Card

class GameView : public QWidget {
    Q_OBJECT

public:
    explicit GameView(QWidget *parent = nullptr);

    void setPlayerInfo(const QString &name, const QPixmap &avatar);
    void addPlayerDisplay(const QString &name, const QPixmap &avatar, bool isCurrentPlayer);
    void displayHand(const QList<Card> &hand);  // Nuevo método para mostrar la mano del jugador
    void enablePlay();  // Habilitar la jugada para el jugador

private:
    struct PlayerDisplayInfo {
        QLabel *avatarLabel;
        QLabel *nameLabel;
    };

    QVBoxLayout *playersLayout;
    QHBoxLayout *handLayout;  // Layout para mostrar las cartas en la mano del jugador
    QLabel *gameLabel;
    QList<PlayerDisplayInfo> playerDisplays;

    QList<QPushButton*> cardButtons;  // Lista de botones para las cartas en la mano del jugador
};

#endif // GAMEVIEW_H

