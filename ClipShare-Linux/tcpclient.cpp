#include "tcpclient.h"

#include <QDebug>
#include <QThread>

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

	initConnection();
}

void TcpClient::initConnection()
{
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
	}
}

void TcpClient::writeToSocket(const QString& str)
{
	if(!connectedFlag) {
		initConnection();
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

