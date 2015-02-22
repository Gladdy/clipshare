#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QString>
#include <QJsonDocument>

struct ConnectInfo {
    QString email;
    QString password;
    QString hostname;
    int port;
};

class ApplicationSettings
{
public:
    ApplicationSettings();
    ~ApplicationSettings();

    void initialize();
    ConnectInfo getConnectInfo();

private:
    QString email;
    QString password;

    QString hostname = "84.85.97.221";
    int port = 31415;

    int maxTransmitSize;

    const QString configFilename = "config.txt";
    QJsonDocument config;
};

#endif // APPLICATIONSETTINGS_H
