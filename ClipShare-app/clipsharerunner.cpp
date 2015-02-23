#include "clipsharerunner.h"

#include "applicationsettings.h"
#include "clipboardjsonformatter.h"
#include "networkmanager.h"

#include <QApplication>
#include <QClipboard>
#include <QJsonValue>
#include <QDebug>

ClipShareRunner::ClipShareRunner(QObject * parent) : QObject(parent)
{
    settings = new ApplicationSettings(this);
    formatter = new ClipboardJSONFormatter(settings, this);
    manager = new NetworkManager(settings, this);

    //Clipboard change
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(processClipboardChange()));

    //Netwerk connections
    connect(this,SIGNAL(emitNetworkRequest(QJsonDocument)),manager,SLOT(processNetworkRequest(QJsonDocument)));
    connect(manager,SIGNAL(emitNetworkResponse(QJsonDocument)),this,SLOT(processNetworkResponse(QJsonDocument)));

    //Errors
    connect(settings,SIGNAL(emitSettingsError(int,QString)),this,SLOT(processError(int,QString)));
}
void ClipShareRunner::initialize()
{
    settings->initialize();
}


void ClipShareRunner::processClipboardChange()
{
    clipboardTriggerList.push_back(QTime::currentTime());
    int period = settings->getSetting("copyTimePeriod").toInt();

    while(clipboardTriggerList.front().elapsed() > period) {
        clipboardTriggerList.removeFirst();
    }

    emitNotification("amount of clips: ", QString::number(clipboardTriggerList.length()));

    /*
    if(lastUpdated.elapsed() > 100)
    {
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
    */
}

void ClipShareRunner::processNetworkResponse(QJsonDocument doc)
{
    qDebug() << doc;

    /*
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
    */
}

void ClipShareRunner::processError(int severity, QString message)
{
    emitNotification(QString::number(severity),message);
}

void ClipShareRunner::processCommand(QString str, QString msg)
{
    qDebug() << "processing command in clipsharerunner: " << str << "\t" << msg;
}

