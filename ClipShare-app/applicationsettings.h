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

signals:
    void emitSettingsError(int, QString);

private:
    void loadDefaults();
    void saveConfigToDisk();

    const QString configFilename = "config.cfg";
    QJsonDocument config;
    QMutex configLock;
};

#endif // APPLICATIONSETTINGS_H
