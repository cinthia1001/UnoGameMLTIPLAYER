#include "client.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QDataStream>
#include <QBuffer>
#include "deck.h"

Client::Client(QWidget *parent) : QWidget(parent) {
    setFixedSize(400, 400);

    stackedWidget = new QStackedWidget(this);

    initialView = new QWidget(this);
    QVBoxLayout *initialLayout = new QVBoxLayout(initialView);

    avatarLabel = new QLabel(this);
    avatarLabel->setFixedSize(100, 100);
    avatarLabel->setScaledContents(true);
    avatarLabel->setStyleSheet("border-radius: 50px; border: 2px solid gray;");

    QPushButton *avatarButton = new QPushButton("Cargar Avatar", this);
    connect(avatarButton, &QPushButton::clicked, this, &Client::loadAvatar);

    QLabel *nameLabel = new QLabel("Nombre:", this);
    nameEdit = new QLineEdit(this);

    QLabel *serverIpLabel = new QLabel("IP del Servidor:", this);
    serverIpEdit = new QLineEdit(this);

    QPushButton *connectButton = new QPushButton("Conectarse", this);
    connect(connectButton, &QPushButton::clicked, this, &Client::connectToServer);

    QHBoxLayout *avatarLayout = new QHBoxLayout();
    avatarLayout->addWidget(avatarLabel);
    avatarLayout->addWidget(avatarButton);
    avatarLayout->setAlignment(Qt::AlignCenter);

    messageArea = new QTextEdit(this);
    messageArea->setReadOnly(true);

    initialLayout->addLayout(avatarLayout);
    initialLayout->addWidget(nameLabel);
    initialLayout->addWidget(nameEdit);
    initialLayout->addWidget(serverIpLabel);
    initialLayout->addWidget(serverIpEdit);
    initialLayout->addWidget(connectButton);
    initialLayout->addWidget(messageArea);

    initialView->setLayout(initialLayout);

    gameView = new GameView(this);

    stackedWidget->addWidget(initialView);
    stackedWidget->addWidget(gameView);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::readMessage);
}

void Client::connectToServer() {
    QString serverIp = serverIpEdit->text();
    tcpSocket->connectToHost(serverIp, 1234);

    if (tcpSocket->waitForConnected(5000)) {
        QString name = nameEdit->text();

        QByteArray avatarData;
        QBuffer buffer(&avatarData);
        buffer.open(QIODevice::WriteOnly);
        avatarPixmap.save(&buffer, "JPG");

        QDataStream out(tcpSocket);
        out.setVersion(QDataStream::Qt_6_0);
        out << QString("player_info") << name << avatarData;
    } else {
        messageArea->append("Error al conectarse al servidor.");
    }
}

void Client::readMessage() {
    if (!tcpSocket->isOpen()) {
        std::cout << "El socket no está abierto. No se puede leer el mensaje." << std::endl;
        return;
    }

    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_6_0);

    QString messageType;
    in >> messageType;

    if (messageType == "your_turn") {
        messageArea->append("Es tu turno. Juega una carta.");
        gameView->enablePlay();  // Habilitar la jugada para el jugador
    } else if (messageType == "card_played") {
        QString playerName, cardName;
        in >> playerName >> cardName;
        messageArea->append(playerName + " jugó " + cardName);
    }
}

void Client::playCard(const QString &cardName) {
    QDataStream out(tcpSocket);
    out.setVersion(QDataStream::Qt_6_0);
    out << QString("play_card") << cardName;
}

void Client::switchToGameView() {
    setFixedSize(1280, 720);

    stackedWidget->setCurrentWidget(gameView);
    showNormal();
    hide();
    show();
    qDebug() << "Cambiando a la vista del juego con resolución 720x1280.";
}

void Client::loadAvatar() {
    QString fileName = QFileDialog::getOpenFileName(this, "Seleccionar Avatar", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        QPixmap originalPixmap(fileName);

        QPixmap circularPixmap(avatarLabel->size());
        circularPixmap.fill(Qt::transparent);

        QPainter painter(&circularPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QPainterPath path;
        path.addEllipse(0, 0, circularPixmap.width(), circularPixmap.height());
        painter.setClipPath(path);

        painter.drawPixmap(0, 0, circularPixmap.width(), circularPixmap.height(), originalPixmap);

        avatarPixmap = circularPixmap;
        avatarLabel->setPixmap(avatarPixmap);

        qDebug() << "Avatar cargado y redimensionado circularmente desde:" << fileName;
    }
}

