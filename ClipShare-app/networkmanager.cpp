#include "networkmanager.h"
#include "applicationsettings.h"

#include <QDebug>
#include <QNetworkRequest>
#include <iterator>
#include <QUrl>
#include <QUrlQuery>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeData>

NetworkManager::NetworkManager(ApplicationSettings * s, QObject* parent) :
    QObject(parent)
{
    settings = s;
    accessManager = new QNetworkAccessManager(this);

    connect(accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(networkFinished(QNetworkReply*)));
    connect(accessManager,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),this,SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

    // Read the SSL certificate
    QFile file(":/keys/ssl.crt");
    file.open(QIODevice::ReadOnly);
    const QByteArray bytes = file.readAll();

    // Create a certificate object
    const QSslCertificate certificate(bytes);
    QSslSocket::addDefaultCaCertificate(certificate);
}
NetworkManager::~NetworkManager() {}

void NetworkManager::processNetworkRequest(QJsonDocument jdoc)
{
    QJsonObject jobject = jdoc.object();
    QString type;

    if(addCredentials(jobject) == false) {
        return;
    }

    if(jobject.contains("type"))
    {
        type = jobject["type"].toString();
        jobject.erase(jobject.find("type"));
    }
    else
    {
        emitNotification("Error",tr("Malformed internal JSON"));
        return;
    }

    if(type == "text")
    {
        //extractTextData(jobject, "postData");
    }
    else if(type == "file" || type == "image")
    {
        QString location;

        if(jobject.contains("location") == false)
        {
            emitNotification("Error",tr("No location specified"));
            return;
        }
        else
        {
            location = jobject["location"].toString();
            jobject.erase(jobject.find("type"));
        }

        //extractFileData(jobject, type, location, "postData");
    }
    else
    {
        emitNotification("Error",tr("Invalid data type"));
        return;
    }
}
void NetworkManager::checkCredentials()
{
    QJsonObject jobject;

    if(addCredentials(jobject) == true)
    {
        postRequest(getUrl("login_api"),extractTextData(jobject));
    }
}
void NetworkManager::abortUpload() {

}
void NetworkManager::testUpload()
{
    if(currentlyUploading) {
        emitNotification("Error","Already uploading a file");
        return;
    }
    else
    {
        currentlyUploading = true;
    }


    QJsonObject jobject;

    if(addCredentials(jobject) == true)
    {
        QList<QHttpPart> http = extractTextData(jobject);
        http.append(extractFileData(":/icons/beach.jpg"));

        postRequest(getUrl("content"),http);
    }
}

bool NetworkManager::addCredentials(QJsonObject& object)
{
    QString email = settings->getSetting("email").toString();
    QString password = settings->getSetting("password").toString();

    if(email.length() == 0 || password.length() == 0)
    {
        emitNotification("Error","Could not find login details");
        return false;
    }

    object.insert("email",email);
    object.insert("password",password);
    return true;
}
QList<QHttpPart> NetworkManager::extractTextData(QJsonObject data)
{
    QList<QHttpPart> textParts;
    QJsonObject::iterator jsonIterator;
    for(jsonIterator = data.begin(); jsonIterator != data.end(); jsonIterator++)
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+jsonIterator.key()+ "\""));
        textPart.setBody(jsonIterator.value().toString().toLatin1());
        textParts.append(textPart);
    }

    return textParts;
}
QHttpPart NetworkManager::extractFileData(QString location)
{
    QHttpPart filePart;
    filePointer.reset(new QFile(location));

    if(!filePointer->open(QIODevice::ReadOnly))
    {
        emitNotification("Error","Invalid filename in uploader");
        return filePart;
    }

    QString filename = filePointer->fileName();
    QMimeDatabase database;
    QMimeType mimeType = database.mimeTypeForFile(filename);

    QString mimeTypeString = mimeType.name();

    qDebug() << "mimetype" << mimeTypeString;

    filePart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant(mimeTypeString));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\""));
    filePart.setBodyDevice(filePointer.data());

    return filePart;
}
void NetworkManager::postRequest(QUrl url, QList<QHttpPart> data)
{
    QNetworkRequest request(url);

    QHttpMultiPart * multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    for(QHttpPart part : data) {
        multiPart->append(part);
    }

    multiPart->dumpObjectInfo();

    QNetworkReply * reply = accessManager->post(request, multiPart);

    multiPart->setParent(reply);

    connect(reply,SIGNAL(uploadProgress(qint64,qint64)),this,SLOT(networkUpdate(qint64,qint64)));
}
QUrl NetworkManager::getUrl(QString target)
{
    QString hostname = settings->getSetting("hostname").toString();
    int portInt = settings->getSetting("port").toInt();
    QString port = QString::number(portInt);

    QString urlString = "https://"+hostname+":"+port+"/"+target;
    return QUrl(urlString);
}
void NetworkManager::networkFinished(QNetworkReply* reply)
{
    qDebug() << "Network reply: " << reply->readAll();
    reply->deleteLater();
    markFinished();
}
void NetworkManager::networkUpdate(qint64 a, qint64 b)
{
    qDebug() << "Network update: " << a << "\t" << b;

    if(currentlyUploading) {
        emitNotification("Progress", QString::number(a) + "\t" + QString::number(b));
    }
}
void NetworkManager::networkError(QNetworkReply::NetworkError networkError)
{
    qDebug() << "Network error: " << networkError;
    markFinished();
}
void NetworkManager::sslErrors(QNetworkReply* reply, QList<QSslError> errors)
{
    qDebug() << "SSL error: " << reply->readAll() << errors;
    markFinished();
}
