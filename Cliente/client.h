#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QTextEdit>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QList>
#include "gameview.h"  

struct PlayerInfo {
    QString name;
    QPixmap avatar;
};

class Client : public QWidget {
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);

private slots:
    void connectToServer();
    void readMessage();
    void switchToGameView();
    void loadAvatar();
    void playCard(const QString &cardName);

private:
    QTcpSocket *tcpSocket;
    QLineEdit *nameEdit;
    QLineEdit *serverIpEdit;
    QTextEdit *messageArea;  
    QStackedWidget *stackedWidget;  
    QWidget *initialView;  
    GameView *gameView;  

    QLabel *avatarLabel;  
    QPixmap avatarPixmap;  

    QList<PlayerInfo> players;  
};

#endif // CLIENT_H

