#include "server.h"
#include <QVBoxLayout>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QDateTime>
#include <QTextCursor>
#include <QBuffer>
#include <QDataStream>
#include <QHeaderView>
#include "deck.h"

Server::Server(QWidget *parent) : QWidget(parent), playerCount(0), currentPlayerIndex(-1), currentState(WaitingForPlayers) {
    setFixedSize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    serverInfoLabel = new QLabel(this);
    QString ipAddress;

    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            ipAddress = address.toString();
    }
    serverInfoLabel->setText("<b>IP del Servidor:</b> " + ipAddress + "<br><b>Puerto:</b> 1234");

    messageArea = new QTextEdit(this);
    messageArea->setReadOnly(true);

    startButton = new QPushButton("Start", this);
    connect(startButton, &QPushButton::clicked, this, &Server::startGame);

    QHBoxLayout *rightLayout = new QHBoxLayout();
    rightLayout->addWidget(messageArea);
    rightLayout->addWidget(startButton);

    mainLayout->addWidget(serverInfoLabel);
    mainLayout->addLayout(rightLayout);

    playerTable = new QTableWidget(this);
    playerTable->setColumnCount(3);
    playerTable->setHorizontalHeaderLabels({"Nombre", "IP", "Avatar"});
    playerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mainLayout->addWidget(playerTable);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);

    if (!tcpServer->listen(QHostAddress::Any, 1234)) {
        appendMessage("Fallo al iniciar el servidor.", "error");
    } else {
        appendMessage("Servidor iniciado en " + ipAddress + ":" + QString::number(tcpServer->serverPort()), "info");
    }
}

void Server::newConnection() {
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    clientSockets.append(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::readData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::playerDisconnected);

    playerCount++;
    QString clientInfo = QString("Jugador %1 conectado: %2")
                             .arg(playerCount)
                             .arg(clientSocket->peerAddress().toString());
    appendMessage(clientInfo, "connection");

    // Enviar mensaje de conexión exitosa al cliente con su número de jugador
    QDataStream out(clientSocket);
    out.setVersion(QDataStream::Qt_6_0);
    out << QString("welcome_message") << QString("Eres el jugador %1 conectado exitosamente").arg(playerCount);
}

void Server::readData() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        QDataStream in(clientSocket);
        in.setVersion(QDataStream::Qt_6_0);

        QString actionType;
        in >> actionType;

        if (actionType == "play_card") {
            if (clientSockets[currentPlayerIndex] == clientSocket) {
                QString cardName;
                in >> cardName;

                // Lógica para validar y aplicar la jugada
                broadcastPlay(players[currentPlayerIndex].name, cardName);

                nextTurn();
            } else {
                sendErrorMessage(clientSocket, "No es tu turno.");
            }
        } else if (actionType == "player_info") {
            QString name;
            QByteArray avatarData;
            in >> name >> avatarData;

            QPixmap avatar;
            avatar.loadFromData(avatarData, "JPG");

            updatePlayerInfo(clientSocket, name, avatar);
        }
    }
}

void Server::startGame() {
    if (currentState == WaitingForPlayers && playerCount >= 2) {
        appendMessage("Iniciando la partida...", "info");

        currentState = GameInProgress;

        // Crear y barajar el mazo
        QList<Card> deck = createDeck();
        shuffleDeck(deck);

        // Repartir 7 cartas a cada jugador
        for (PlayerInfo &player : players) {
            for (int i = 0; i < 7; ++i) {
                if (!deck.isEmpty()) {
                    Card card = deck.takeFirst();
                    player.hand.append(card);
                }
            }
        }

        notifyCurrentPlayer();
    } else {
        appendMessage("Se necesitan al menos 2 jugadores para iniciar la partida.", "error");
    }
}

void Server::notifyCurrentPlayer() {
    if (currentPlayerIndex >= 0 && currentPlayerIndex < players.size()) {
        QTcpSocket *currentSocket = clientSockets[currentPlayerIndex];
        QDataStream out(currentSocket);
        out.setVersion(QDataStream::Qt_6_0);
        out << QString("your_turn");
    }
}

void Server::nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    notifyCurrentPlayer();
}

void Server::broadcastPlay(const QString &playerName, const QString &cardName) {
    for (QTcpSocket *socket : clientSockets) {
        QDataStream out(socket);
        out.setVersion(QDataStream::Qt_6_0);
        out << QString("card_played") << playerName << cardName;
    }
}

void Server::playerDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        int index = clientSockets.indexOf(clientSocket);
        if (index != -1) {
            clientSockets.removeAt(index);
            players.removeAt(index);
            playerCount--;

            if (playerCount < 2 && currentState == GameInProgress) {
                endGame();
                appendMessage("El juego terminó porque hay menos de 2 jugadores.", "info");
            }
        }
    }
}

void Server::endGame() {
    currentState = GameEnded;
    appendMessage("El juego ha terminado.", "info");
}

void Server::showPlayerInfo() {
    playerTable->setRowCount(players.size());

    for (int i = 0; i < players.size(); ++i) {
        PlayerInfo &player = players[i];

        QTableWidgetItem *nameItem = new QTableWidgetItem(player.name);
        QTableWidgetItem *ipItem = new QTableWidgetItem(player.ipAddress);

        playerTable->setRowHeight(i, 100);

        QLabel *avatarLabel = new QLabel;
        avatarLabel->setPixmap(player.avatar.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        playerTable->setItem(i, 0, nameItem);
        playerTable->setItem(i, 1, ipItem);
        playerTable->setCellWidget(i, 2, avatarLabel);
    }

    playerTable->horizontalHeader()->setMinimumHeight(50);
}

void Server::appendMessage(const QString &message, const QString &messageType) {
    QString formattedMessage;

    if (messageType == "info") {
        formattedMessage = "<span style='color: blue;'><b>[INFO]</b> " + message + "</span>";
    } else if (messageType == "error") {
        formattedMessage = "<span style='color: red;'><b>[ERROR]</b> " + message + "</span>";
    } else if (messageType == "connection") {
        formattedMessage = "<span style='color: green;'><b>[CONEXIÓN]</b> " + message + "</span>";
    } else {
        formattedMessage = message;
    }

    formattedMessage = "<hr>" + formattedMessage + "<br><i>" + QDateTime::currentDateTime().toString("hh:mm:ss") + "</i>";
    
    messageArea->moveCursor(QTextCursor::End);
    messageArea->insertHtml(formattedMessage);
    messageArea->moveCursor(QTextCursor::End);
}

void Server::updatePlayerInfo(QTcpSocket *socket, const QString &name, const QPixmap &avatar) {
    PlayerInfo playerInfo;
    playerInfo.name = name;
    playerInfo.ipAddress = socket->peerAddress().toString();
    playerInfo.playerNumber = playerCount;
    playerInfo.avatar = avatar;

    players.append(playerInfo);
    showPlayerInfo();
}
void Server::sendErrorMessage(QTcpSocket *socket, const QString &message) {
    QDataStream out(socket);
    out.setVersion(QDataStream::Qt_6_0);
    out << QString("error_message") << message;
}

