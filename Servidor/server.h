#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QTcpSocket>
#include <QList>
#include <QTableWidget>
#include "deck.h"

enum GameState {
    WaitingForPlayers,
    GameInProgress,
    GameEnded
};

struct PlayerInfo {
    QString name;
    QString ipAddress;
    int playerNumber;
    QPixmap avatar;
    QList<Card> hand;  // Lista de cartas en la mano del jugador
};

class Server : public QWidget {
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);

private slots:
    void newConnection();
    void readData();
    void startGame();
    void playerDisconnected();

private:
    void appendMessage(const QString &message, const QString &messageType);
    void showPlayerInfo();
    void updatePlayerInfo(QTcpSocket *socket, const QString &name, const QPixmap &avatar);
    void notifyCurrentPlayer();
    void nextTurn();
    void broadcastPlay(const QString &playerName, const QString &cardName);
    void endGame();
    void sendErrorMessage(QTcpSocket *socket, const QString &message);


    QTcpServer *tcpServer;             
    QTextEdit *messageArea;            
    QLabel *serverInfoLabel;           
    QPushButton *startButton;          
    QTableWidget *playerTable;         
    QList<PlayerInfo> players;         
    QList<QTcpSocket*> clientSockets;  
    int playerCount;                   
    int currentPlayerIndex;            
    GameState currentState;            
};

#endif // SERVER_H

