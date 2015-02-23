#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>
#include <QString>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMutex>

class ApplicationSettings : public QObject
{
    Q_OBJECT
public:
    ApplicationSettings(QObject* parent = 0) : QObject(parent)
    {
        loadDefaults();
    }
    void initialize();

    void setSetting(QString, QJsonValue);
    QJsonValue getSetting(QString);
    bool validateEmail(QString);
    bool validateNumber(QString, int, int);
    void saveConfigToDisk();

signals:
    void emitSettingsError(int, QString);

private:
    void loadDefaults();

    const QString configFilename = "config.cfg";
    QJsonDocument config;
    QMutex configLock;
};

#endif // APPLICATIONSETTINGS_H
