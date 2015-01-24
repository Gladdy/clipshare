#include "tcpclient.h"

#include <QDebug>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>

#include <iostream>

TcpClient::TcpClient(QString h, int p, QObject * parent) :
	QObject(parent),
	hostname(h),
	port(p)
{
	socket = new QTcpSocket(this);

	connect(socket,SIGNAL(connected()), this, SLOT(connected()));
	connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(socket,SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
}
void TcpClient::updateConnectString(QString username, QString password)
{
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
	}
	else
	{
		std::cout << "Connected to server!" << std::endl;
		writeToSocket(connectString);
	}
}

void TcpClient::writeToSocket(const QString& str)
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
}
void TcpClient::bytesWritten (qint64 bytes)
{
	qDebug() << "we wrote: " << bytes;
}
void TcpClient::readyRead()
{
	qDebug() << "Reading...";
	emit readFromSocket(socket->readAll());
}

