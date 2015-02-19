#include "clipsharerunner.h"
#include "tcpclient.h"

#include <QApplication>
#include <QClipboard>
#include <QFile>

#include <QJsonObject>
#include <QJsonValue>

ClipShareRunner::ClipShareRunner(QObject * p) :
    QObject(p)
{
    tcpclient = new TcpClient(this);
    mimeData = new QMimeData();

    //Clipboard change
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(processClipboardChange()));

    //Netwerk connections
    connect(this,SIGNAL(writeToSocket(QString)),tcpclient,SLOT(writeToSocket(QString)));
    connect(tcpclient,SIGNAL(readFromSocket(QString)),this,SLOT(readFromSocket(QString)));

    //Errors
    connect(this,SIGNAL(error(int,QString)),this,SLOT(displayError(int,QString)));

    supportedTypes.append("text/plain");
    supportedTypes.append("text/html");

    lastUpdated = QTime::currentTime();
}
void ClipShareRunner::initialize() {
    readConfigFile();
    applyConnectionConfig();
    applyTransmissionConfig();
}
void ClipShareRunner::displayError(int severity, QString msg) {
    qDebug() << severity << " : " << msg;
}

/**
 * @brief ClipShareRunner::processClipboardChange
 * emit only whenever the last clipboard change was more than 100 msec ago
 * solution to both overflowing the server and the crossplatform troubles
 */
void ClipShareRunner::processClipboardChange()
{
    if(lastUpdated.elapsed() > 100)
    {
        emit readingClipboardUpdate();

        const QMimeData * changedMimeData = QApplication::clipboard()->mimeData();
        QJsonObject clipboardJSON;

        for(QString t : supportedTypes) {
            if(changedMimeData->hasFormat(t)) {
                QByteArray data = changedMimeData->data(t);
                clipboardJSON.insert(t,QString(data));
            }
        }

        QJsonDocument doc { clipboardJSON };
        QString clipboardJSONString {doc.toJson()};

        if(clipboardJSONString.length() > maxTransmitSize)
        {
            emit error(2, "transmitting too much data, not executed");
        }
        else
        {
            emit writeToSocket(clipboardJSONString);
        }
    }
}

/**
 * @brief ClipShareRunner::readFromSocket performs a clipboard operation thus must be locked
 * @param str
 */
void ClipShareRunner::readFromSocket(QString str)
{
    mimeData = new QMimeData();

    QJsonDocument strJsonDoc = QJsonDocument::fromJson(str.toLatin1());
    QJsonObject strJsonObject = strJsonDoc.object();

    for(QString type : supportedTypes)
    {
        if(strJsonObject.contains(type))
        {
            QByteArray data = strJsonObject[type].toString().toLatin1();
            mimeData->setData(type, data);
        }
    }

    lastUpdated = QTime::currentTime();
    QApplication::clipboard()->setMimeData(mimeData);
    emit writingToClipboard();
}

void ClipShareRunner::readConfigFile()
{
    QFile configFile (configFilename);

    if(!configFile.open(QIODevice::ReadOnly)) {
        emit error(1,"Could not open config file: " + configFilename);
        return;
    }
    else
    {
        QByteArray configFileContent = configFile.readAll();
        configFile.close();

        config = QJsonDocument::fromJson(configFileContent);

        if(config.isNull()) {
            emit error(1, "invalid config file");
        }
    }
}

void ClipShareRunner::applyConnectionConfig()
{
    QJsonObject configJSON = config.object();

    if(configJSON.contains("email") && configJSON.contains("password") && configJSON.contains("hostname") && configJSON.contains("port"))
    {
        QString email = configJSON["email"].toString();
        QString password = configJSON["password"].toString();
        QString hostname = configJSON["hostname"].toString();
        int port = configJSON["port"].toInt();

        tcpclient->disconnect();
        tcpclient->updateConnectInfo(email,password,hostname,port);
        tcpclient->initConnection();
    }
    else
    {
        emit error(1, "invalid connection info in config data");
    }
}
void ClipShareRunner::applyTransmissionConfig()
{
    QJsonObject configJSON = config.object();

    if(configJSON.contains("maxclipsize"))
    {
        maxTransmitSize = configJSON["maxclipsize"].toInt();
    }
    else
    {
        emit error(2, "no max clipsize in the config file");
    }
}
