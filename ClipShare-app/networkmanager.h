#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QJsonDocument>

class ApplicationSettings;

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    NetworkManager(ApplicationSettings * s, QObject* parent = 0) : QObject(parent)
    {
        settings = s;
    }
    ~NetworkManager()
    {

    }

signals:
    void emitNetworkResponse(QJsonDocument);

public slots:
    void processNetworkRequest(QJsonDocument);

private:
    ApplicationSettings * settings;
};

#endif // NETWORKMANAGER_H
