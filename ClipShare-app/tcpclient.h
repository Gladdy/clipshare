#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    TcpClient(QObject * parent = 0);
    ~TcpClient();
    void updateConnectInfo(QString username, QString password, QString hostname, int port);
    void initConnection();

signals:
    void readFromSocket(QString);

public slots:
    void writeToSocket(QString);

    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

private:	
    QTcpSocket * socket;

    int networkTimeout = 10000; //milliseconds

    QString connectString;
    QString hostname;
    int port;
    bool connectedFlag = false;

    QByteArray readBuffer;
    bool isValidJSON(QByteArray);

    bool closing = false;
};

#endif // TCPCLIENT_H
