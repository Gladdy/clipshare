#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject
{
	Q_OBJECT
public:
	TcpClient(QString hostname, int port, QObject * parent = 0);
	~TcpClient();

signals:
	void readFromSocket(const QString& data);

public slots:
	void writeToSocket(const QString&);

	void connected();
	void disconnected();
	void bytesWritten(qint64 bytes);
	void readyRead();

private:	
	QTcpSocket * socket;

	void initConnection();

	int networkTimeout = 1000; //milliseconds

	QString hostname;
	int port;
	bool connectedFlag = false;
};

#endif // TCPCLIENT_H
