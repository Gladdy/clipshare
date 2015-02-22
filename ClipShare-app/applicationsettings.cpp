#include "applicationsettings.h"
#include <QFile>

ApplicationSettings::ApplicationSettings()
{
    supportedTypes.append("text/plain");
    supportedTypes.append("text/html");
}

ApplicationSettings::~ApplicationSettings()
{

}

void ApplicationSettings::initialize() {

    QFile configFile (configFilename);

    /*
     *  PARSE CONFIG FILE AS JSON
     */
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
            return;
        }
    }

    QJsonObject configJSON = config.object();

    /*
     *  EXTRACT AUTHORISATION DATA: EMAIL AND PASSWORD
     */
    if(configJSON.contains("email") && configJSON.contains("password"))
    {
        email = configJSON["email"].toString();
        password = configJSON["password"].toString();
    }
    else
    {
        emit error(1, "no authorisation details provided!");
    }

    /*
     *  EXTRACT CONNECTION DATA: HOST AND PORT
     */
    if(configJSON.contains("hostname") && configJSON.contains("port"))
    {
        hostname = configJSON["hostname"].toString();
        port = configJSON["port"].toInt();
    }
    else
    {
        emit error(2, "no custom server provided");
    }

    /*
     *  EXTRACT MAX TRANSMISSION SIZE
     */
    if(configJSON.contains("maxclipsize"))
    {
        maxTransmitSize = configJSON["maxclipsize"].toInt();
    }
    else
    {
        emit error(2, "no max clipsize in the config file");
    }

}

ConnectInfo ApplicationSettings::getConnectInfo() {
    return ConnectInfo {email, password, hostname, port};
}
