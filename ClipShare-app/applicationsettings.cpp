#include "applicationsettings.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
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
        emitSettingsError(1,"Could not open config file: " + configFilename);
        return;
    }
    else
    {
        QByteArray configFileContent = configFile.readAll();
        configFile.close();

        configFileContents = QJsonDocument::fromJson(configFileContent);

        if(configFileContents.isNull()) {
            emitSettingsError(1, "invalid config file");
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
        emitSettingsError(1, "no authorisation details provided!");
    }


    //  EXTRACT CONNECTION DATA: HOST AND PORT
    if(configFileObject.contains("hostname") && configFileObject.contains("port"))
    {
        configObject["hostname"] = configFileObject["hostname"].toString();
        configObject["port"] = configFileObject["port"].toInt();
    }
    else
    {
        emitSettingsError(2, "no custom server provided");
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
    configObject["uploadSizeLimit"] = 2000000;
    configObject["copyTimePeriod"] = 1000;
    config = QJsonDocument(configObject);

    configLock.unlock();
}

void ApplicationSettings::saveConfigToDisk()
{
    configLock.lock();

    qDebug() << config;

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
