#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QList>
#include <QSslError>
#include <QHttpPart>
#include <QSharedPointer>
#include <QFile>

class ApplicationSettings;

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    NetworkManager(ApplicationSettings * s, QObject* parent = 0);
    ~NetworkManager();

signals:
    void emitNetworkResponse(QJsonDocument);
    void emitNotification(QString, QString);

public slots:
    void processNetworkRequest(QJsonDocument);
    void checkCredentials();
    void abortUpload();

    void testUpload();

private slots:
    void networkFinished(QNetworkReply*);
    void networkUpdate(qint64, qint64);

    void networkError(QNetworkReply::NetworkError);
    void sslErrors(QNetworkReply*,QList<QSslError>);

private:
    QList<QHttpPart> extractTextData(QJsonObject data);
    QHttpPart extractFileData(QString location);

    bool addCredentials(QJsonObject& data);
    void postRequest(QUrl url, QList<QHttpPart> data);

    QUrl getUrl(QString target);

    bool currentlyUploading = false;
    void markFinished() { currentlyUploading = false; }

    QSharedPointer<QFile> filePointer;
    QNetworkAccessManager* accessManager;
    ApplicationSettings * settings;
};

#endif // NETWORKMANAGER_H
