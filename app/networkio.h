#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "status.h"

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QList>
#include <QSslError>
#include <QHttpPart>
#include <QFile>

#include <map>

class Settings;

class NetworkIO : public StatusReporter {
  Q_OBJECT
public:
  NetworkIO(Settings * s, QObject * parent = 0);
  ~NetworkIO();

signals:
  void emitResult(QString);
  void emitMessage(MessageType, QString);

public slots:
  void upload(QString);
  void login();
  void abort();

private slots:
  void finished(QNetworkReply *);
  void update(qint64, qint64);

  void error(QNetworkReply::NetworkError);
  void error_ssl(QNetworkReply *, QList<QSslError>);

private:
  void post(QUrl, std::map<QString, QString>, QString);

  QUrl getUrl(QString target);

  bool currentlyUploading = false;
  void markFinished() { currentlyUploading = false; }

  QNetworkAccessManager *accessManager;
  Settings *settings;
};

#endif // NETWORKMANAGER_H
