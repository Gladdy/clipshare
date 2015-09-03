#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include "messagetype.h"

#include <QObject>
#include <QString>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMutex>

/**
 * @brief The ApplicationSettings class
 *
 * FIXED
 * String hostname
 * int port
 * int sizeLimit
 *
 * CHANGEABLE
 * String email
 * String password
 * bool trayNotification
 * bool storeLocally
 * int doubleCopyPeriod
 */
class ApplicationSettings : public QObject
{
    Q_OBJECT
public:
    ApplicationSettings(QObject* parent = 0) : QObject(parent)
    {
        loadDefaults();
    }
    void loadConfigFile();

    //Fixed getters
    QString getConnectString() { return QString(hostname + ":" + QString::number(port)); }
    QString getHostname() { return hostname; }
    int getSizeLimit() { return sizeLimit; }

    //Modifiable getters and setters
    QString getEmail() { return email; }
    bool setEmail(QString);

    QString getPassword() { return password; }
    bool setPassword(QString p);

    bool getTrayNotification() { return trayNotification; }
    bool setTrayNotification(bool t) { trayNotification = t; return true; }

    bool getLocalStorage() { return localStorage; }
    bool setLocalStorage(bool l) { localStorage = l; return true; }

    int getDoubleCopyPeriod() { return doubleCopyPeriod; }
    bool setDoubleCopyPeriod(QString d);

    void saveConfigToDisk();

signals:
    void emitMessage(MessageType, QString message);

private:
    void loadDefaults();
    bool validateEmail(QString);
    int validateNumber(QString, int, int);

    QString hostname;
    int port;
    int sizeLimit;

    QString email;
    QString password;
    bool trayNotification;
    bool localStorage;
    int doubleCopyPeriod;

    const QString configFilename = "config.cfg";
};

#endif // APPLICATIONSETTINGS_H
