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
    settings = new ApplicationSettings();

    //Clipboard change
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(processClipboardChange()));

    //Netwerk connections
    connect(this,SIGNAL(writeToSocket(QString)),tcpclient,SLOT(writeToSocket(QString)));
    connect(tcpclient,SIGNAL(readFromSocket(QString)),this,SLOT(readFromSocket(QString)));

    //Errors
    connect(this,SIGNAL(error(int,QString)),this,SLOT(displayError(int,QString)));



    lastUpdated = QTime::currentTime();
}
void ClipShareRunner::initialize()
{
    settings->initialize();
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
 * @brief
 * @param
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

