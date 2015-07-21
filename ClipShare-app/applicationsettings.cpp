#include "applicationsettings.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QDebug>

void ApplicationSettings::loadConfigFile()
{
    QFile configFile (configFilename);
    QJsonDocument configFileContents;

    if(!configFile.open(QIODevice::ReadOnly))
    {
        emitMessage(Error,tr("Could not open config file:") +" "+ configFilename);
        return;
    }
    else
    {
        QByteArray configFileContent = configFile.readAll();
        configFile.close();

        configFileContents = QJsonDocument::fromJson(configFileContent);

        if(configFileContents.isNull()) {
            emitMessage(Error, tr("Invalid config file"));
            return;
        }
    }

    QJsonObject configFileObject = configFileContents.object();

    //  EXTRACT AUTHORISATION DATA: EMAIL AND PASSWORD
    if(configFileObject.contains("email") && configFileObject.contains("password"))
    {
        setEmail(configFileObject["email"].toString());
        setPassword(configFileObject["password"].toString());
    }
    else
    {
        emitMessage(Notification,tr("No authorisation details provided in config file!"));
    }

    if(configFileObject.contains("trayNotification")) {
        setTrayNotification(configFileObject["trayNotification"].toBool());
    }

    if(configFileObject.contains("localStorage")) {
        setLocalStorage(configFileObject["localStorage"].toBool());
    }

    if(configFileObject.contains("doubleCopyPeriod")) {
        setLocalStorage(configFileObject["doubleCopyPeriod"].toInt());
    }

    saveConfigToDisk();
}
bool ApplicationSettings::setEmail(QString e) {
    if(validateEmail(e)) {
        email = e;
        return true;
    }
    return false;
}
bool ApplicationSettings::setPassword(QString p) {
    if(p.length()) {
        password = p;
        return true;
    }
    return false;
}
bool ApplicationSettings::setDoubleCopyPeriod(QString d) {
    int vn = validateNumber(d, 100, 5000);
    if(vn != -1){
        doubleCopyPeriod = vn;
        return true;
    }
    return false;
}


void ApplicationSettings::loadDefaults()
{
    hostname = "84.85.97.221";
    port = 31443;
    sizeLimit = 20000000;       //in bytes, 20MB

    email = "";
    password = "";
    trayNotification = true;
    localStorage = true;
    doubleCopyPeriod = 1000;    //in msec, 1 second
}

void ApplicationSettings::saveConfigToDisk()
{

    qDebug() << "ApplicationSettings : saving to disk";

}

bool ApplicationSettings::validateEmail(QString email)
{
    QRegularExpression regex("^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$");
    return regex.match(email).hasMatch();
}
int ApplicationSettings::validateNumber(QString number, int min, int max) {
    int converted = number.toInt();
    if(number == QString::number(converted) && converted >= min && converted <= max) {
        return converted;
    }
    return -1;
}
