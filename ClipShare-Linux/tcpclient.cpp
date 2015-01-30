#include "tcpclient.h"

#include <QDebug>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>

#include <iostream>

TcpClient::TcpClient(QObject * parent) :
QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket,SIGNAL(connected()), this, SLOT(connected()));
    connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket,SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
}
void TcpClient::updateConnectInfo(QString username, QString password, QString h, int p)
{
    this->hostname = h;
    this->port = p;

    QJsonObject json;

    json.insert("username", username);
    json.insert("password", password);

    QJsonDocument doc {json};
    QByteArray jsonData = doc.toJson();

    connectString = QString(jsonData);
}

void TcpClient::initConnection()
{
    if(!connectString.length()) {
	return;
    }

    socket->connectToHost(hostname, port);

    if(!socket->waitForConnected(networkTimeout))
    {
	std::cerr << "Could not connect to server!" << std::endl;
	QThread::sleep(1);
	initConnection();
    }
    else
    {
	std::cout << "Connected to server!" << std::endl;
	writeToSocket(connectString);
    }
}

void TcpClient::writeToSocket(QString str)
{
    if(!connectedFlag) {
	initConnection();
	QThread::sleep(1);
    }

    socket->write(str.toLatin1());

    if(!socket->waitForBytesWritten(networkTimeout))
    {
	std::cerr << "Unable to write to the server" << std::endl;
	initConnection();
    }
}
TcpClient::~TcpClient()
{
    socket->close();
    delete socket;
}
void TcpClient::connected()
{
    qDebug() << "Connected!";
    connectedFlag = true;
}
void TcpClient::disconnected()
{
    qDebug() << "Disconnected!";
    connectedFlag = false;
    initConnection();
}
void TcpClient::bytesWritten (qint64 bytes)
{
    qDebug() << "wrote bytes: " << bytes;
}
void TcpClient::readyRead()
{
    QByteArray data = socket->readAll();
    readBuffer.append(data);

    if(isValidJSON(readBuffer))
    {
	QString readBufferString {readBuffer};
	emit readFromSocket(readBufferString);
	readBuffer.clear();
    }
}
bool TcpClient::isValidJSON(QByteArray data)
{
    QJsonDocument strJsonDoc = QJsonDocument::fromJson(data);

    if(strJsonDoc.isNull()) {
	return false;
    }

    return true;
}

