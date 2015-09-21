#include "networkio.h"
#include "settings.h"

#include <QDebug>
#include <QNetworkRequest>
#include <iterator>
#include <QUrl>
#include <QUrlQuery>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeData>
#include <QNetworkAccessManager>

NetworkIO::NetworkIO(Settings *s, QObject *parent) : StatusReporter(parent) {

  if (!QSslSocket::supportsSsl()) {
    qDebug() << "SSL UNSUPPORTED!";
    exit(1);
  }

  settings = s;
  accessManager = new QNetworkAccessManager(this);

  connect(accessManager, &QNetworkAccessManager::finished, this, &NetworkIO::finished);
  connect(accessManager, &QNetworkAccessManager::sslErrors, this, &NetworkIO::error_ssl);

  // Read the SSL certificate
  QFile file(":/keys/ssl.crt");
  file.open(QIODevice::ReadOnly);
  const QByteArray bytes = file.readAll();

  // Create a certificate object
  const QSslCertificate certificate(bytes);
  QSslSocket::addDefaultCaCertificate(certificate);
}
NetworkIO::~NetworkIO() {
}

void NetworkIO::upload(QString location) {

  if (currentlyUploading) {
    emitMessage(Notification, "Already uploading a file");
    return;
  } else {
    currentlyUploading = true;
  }

  post(getUrl(uploadLink), getCredentials(), location);
}

std::map<QString, QString> NetworkIO::getCredentials() {
  QString email = settings->getSetting("email").toString();
  QString pass = settings->getSetting("password").toString();

  if(email.length() <= 5 || pass.length() == 0) {
    emitMessage(Notification, "Please provide login credentials");
  }

  std::map<QString,QString> text {
    {"email",email},
    {"password",pass}
  };

  return text;
}

void NetworkIO::login() {
  post(getUrl(loginLink), getCredentials(), "");
}

void NetworkIO::abort() {}

void NetworkIO::post(QUrl url, std::map<QString, QString> text, QString location) {
  QNetworkRequest request(url);
  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  // Add all text form elements to the request
  for(auto& kv : text) {
    QHttpPart textpart;
    textpart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"" + kv.first + "\""));
    textpart.setBody(kv.second.toUtf8());
    multiPart->append(textpart);
  }

  // Add the file to the request
  // Only perform the reading whenever the file actually exists
  QFile file(location);
  if(location != "" && file.exists())
  {
    if(file.open(QIODevice::ReadOnly)) {

      //Read everything needed from the file
      QByteArray fileContent = file.readAll();
      QString filename = file.fileName();
      QString mimeType = QMimeDatabase().mimeTypeForFile(filename).name();

      file.close();

      //Construct the post request part
      QHttpPart filePart;
      filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType));
      filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
            QVariant("form-data; name=\"file\"; filename=\"" + filename + "\""));
      filePart.setBody(fileContent);

      //Add it to the full request
      multiPart->append(filePart);
    }
    else
    {
      emitMessage(Notification, "Unable to open file for uploading");
    }
  }

  // Perform the post request
  QNetworkReply *reply = accessManager->post(request, multiPart);

  //Delete the multipart whenever the accessmanager stops
  multiPart->setParent(reply);

  //Connect the progress indicators
  connect(reply, &QNetworkReply::uploadProgress, this,&NetworkIO::update);
}

QUrl NetworkIO::getUrl(QString target) {
  QString hostname = settings->getSetting("hostname").toString();
  QString port = settings->getSetting("port").toString();
  QString protocol = "http";

  QString urlString = protocol + "://" + hostname + ":" + port + target;
  qDebug() << urlString;

  return QUrl(urlString);
}

void NetworkIO::finished(QNetworkReply *reply) {
  QByteArray responseData = reply->readAll();
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  reply->deleteLater();
  markFinished();

  qDebug() << "Network reply: "
           << "status: " << statusCode << "\n"
           << "length: " << responseData.length() << "\n"
           << responseData;

  emitResult(QString(responseData));
}

void NetworkIO::update(qint64 a, qint64 b) {
  qDebug() << "Network update: " << a << "\t" << b;

  if (currentlyUploading) {
    emitMessage(Progress, QString::number(a) + "\t" + QString::number(b));
  }
}

void NetworkIO::error(QNetworkReply::NetworkError networkError) {
  qDebug() << "Network error: " << networkError;
  markFinished();
}

void NetworkIO::error_ssl(QNetworkReply *reply, QList<QSslError> errors) {
  qDebug() << "SSL error: " << reply->readAll() << errors;
  markFinished();
}
