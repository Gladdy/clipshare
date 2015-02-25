#include "applicationsettings.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QDebug>

void ApplicationSettings::initialize()
{
    QFile configFile (configFilename);
    QJsonDocument configFileContents;

    /*
     *  PARSE CONFIG FILE AS JSON
     */
    if(!configFile.open(QIODevice::ReadOnly))
    {
        emitNotification("Error",tr("Could not open config file:") +" "+ configFilename);
        return;
    }
    else
    {
        QByteArray configFileContent = configFile.readAll();
        configFile.close();

        configFileContents = QJsonDocument::fromJson(configFileContent);

        if(configFileContents.isNull()) {
            emitNotification("Error", tr("Invalid config file"));
            return;
        }
    }

    QJsonObject configFileObject = configFileContents.object();

    configLock.lock();

    QJsonObject configObject = config.object();

    //  EXTRACT AUTHORISATION DATA: EMAIL AND PASSWORD
    if(configFileObject.contains("email") && configFileObject.contains("password"))
    {
        configObject["email"] = configFileObject["email"].toString();
        configObject["password"] = configFileObject["password"].toString();
    }
    else
    {
        emitNotification("Information",tr("No authorisation details provided!"));
    }


    //  EXTRACT CONNECTION DATA: HOST AND PORT
    if(configFileObject.contains("hostname") && configFileObject.contains("port"))
    {
        configObject["hostname"] = configFileObject["hostname"].toString();
        configObject["port"] = configFileObject["port"].toInt();
    }

    config = QJsonDocument(configObject);

    configLock.unlock();

    saveConfigToDisk();
}

void ApplicationSettings::loadDefaults()
{
    configLock.lock();

    QJsonObject configObject = config.object();
    configObject["hostname"] =  "84.85.97.221";
    configObject["port"] = 31443;
    configObject["uploadSizeLimit"] = 2000;
    configObject["copyTimePeriod"] = 1000;
    config = QJsonDocument(configObject);

    configLock.unlock();
}

void ApplicationSettings::saveConfigToDisk()
{
    configLock.lock();

    //qDebug() << config;

    configLock.unlock();
}

QJsonValue ApplicationSettings::getSetting(QString key)
{
    configLock.lock();

    QJsonObject configObject = config.object();
    QJsonValue settingFromConfig {};

    if(configObject.contains(key))
    {
        settingFromConfig = configObject.take(key);
    }

    configLock.unlock();

    return settingFromConfig;
}

void ApplicationSettings::setSetting(QString key, QJsonValue val)
{
    configLock.lock();

    QJsonObject configObject = config.object();
    configObject.insert(key,val);
    config = QJsonDocument(configObject);

    configLock.unlock();
}

bool ApplicationSettings::validateEmail(QString email)
{
    QRegularExpression regex("^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$");
    return regex.match(email).hasMatch();
}
bool ApplicationSettings::validateNumber(QString number, int min, int max) {
    int converted = number.toInt();
    if(number == QString::number(converted) && converted >= min && converted <= max) {
        return true;
    }
    return false;
}
