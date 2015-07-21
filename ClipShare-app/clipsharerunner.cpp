#include "clipsharerunner.h"

#include "applicationsettings.h"
#include "clipboardjsonformatter.h"
#include "networkmanager.h"

#include <QApplication>
#include <QClipboard>
#include <QJsonValue>
#include <QJsonObject>
#include <QDebug>

ClipShareRunner::ClipShareRunner(QObject * parent) : QObject(parent)
{
    settings = new ApplicationSettings(this);
    formatter = new ClipboardJSONFormatter(settings, this);
    manager = new NetworkManager(settings, this);

    //Clipboard change
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(processClipboardChange()));

    //Network connections
    connect(this,SIGNAL(emitNetworkRequest(QJsonDocument)),manager,SLOT(processNetworkRequest(QJsonDocument)));
    connect(manager,SIGNAL(emitNetworkResponse(QJsonDocument)),this,SLOT(processNetworkResponse(QJsonDocument)));

    //Notifications
    connect(settings,SIGNAL(emitMessage(MessageType,QString)),this,SLOT(processMessage(MessageType,QString)));
    connect(manager,SIGNAL(emitMessage(MessageType,QString)),this,SLOT(processMessage(MessageType,QString)));
    connect(formatter,SIGNAL(emitMessage(MessageType,QString)),this,SLOT(processMessage(MessageType,QString)));

    settings->loadConfigFile();
    manager->checkCredentials();
}
void ClipShareRunner::processClipboardChange()
{
    qDebug() << "Clipboard changed!";

    if(ignoreClipboardChange) {
        ignoreClipboardChange = false;
        return;
    }

    //Don't trigger 2 times within 50 milliseconds
    if(clipboardTriggerList.last().elapsed() < 50) {
        return;
    }

    //Filter out the events outside the DoubleCopyPeriod
    clipboardTriggerList.push_back(QTime::currentTime());
    int period = settings->getDoubleCopyPeriod();
    while(clipboardTriggerList.front().elapsed() > period) {
        clipboardTriggerList.removeFirst();
    }

    if(clipboardTriggerList.length() >= 2)
    {
        const QMimeData * mimeData = QApplication::clipboard()->mimeData();
        QJsonDocument request = formatter->getRequestFormat(mimeData);
        emitNetworkRequest(request);
    }
}

void ClipShareRunner::processNetworkResponse(QJsonDocument doc)
{
    QJsonObject response = doc.object();

    if(response.contains("loggedin"))
    {
        if(response["loggedin"].toBool() == true)
        {
            emitMessage(Login, "Correct");
            loggedIn = true;
        }
        else
        {
            emitMessage(Login, "Incorrect");
            loggedIn = false;
        }
    }

    if(response.contains("url"))
    {
        QString url = response["url"].toString();
        setClipboardText(url);
        emitMessage(URL, url);
    }
}
void ClipShareRunner::setClipboardText(QString url)
{
    ignoreClipboardChange = true;
    QMimeData * mimeData = new QMimeData();
    mimeData->setText(url);
    QApplication::clipboard()->setMimeData(mimeData);
}

void ClipShareRunner::processMessage(MessageType type, QString message)
{
    emitMessage(type, message);
}

void ClipShareRunner::processCommand(CommandType type, QString message)
{
    switch(type) {
    case Connect:
        if(message == "CheckCredentials")
        {
            manager->checkCredentials();
        }
        break;
    }
}
