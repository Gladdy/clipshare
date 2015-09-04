#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "status.h"

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

class Settings;

class NetworkIO : public StatusReporter {
  Q_OBJECT
public:
  NetworkIO(Settings * s, QObject * parent = 0);
  ~NetworkIO();

signals:
  void emitNetworkResponse(QJsonDocument);
  void emitMessage(MessageType, QString);

public slots:
  void processNetworkRequest(QJsonDocument);
  void checkCredentials();
  void abortUpload();

private slots:
  void networkFinished(QNetworkReply *);
  void networkUpdate(qint64, qint64);

  void networkError(QNetworkReply::NetworkError);
  void sslErrors(QNetworkReply *, QList<QSslError>);

private:
  QList<QHttpPart> extractTextData(QJsonObject data);
  QHttpPart extractFileData(QString location);

  bool addCredentials(QJsonObject &data);
  void postRequest(QUrl url, QList<QHttpPart> data, QHttpPart fileData);

  QUrl getUrl(QString target);

  bool currentlyUploading = false;
  void markFinished() { currentlyUploading = false; }

  QSharedPointer<QFile> filePointer;
  QNetworkAccessManager *accessManager;
  Settings *settings;
};

#endif // NETWORKMANAGER_H
