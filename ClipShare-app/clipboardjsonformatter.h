#ifndef CLIPBOARDJSONFORMATTER_H
#define CLIPBOARDJSONFORMATTER_H

#include "messagetype.h"

#include <QObject>
#include <QMimeData>
#include <QJsonDocument>
#include <QList>
#include <QUrl>
#include <QImage>
#include <QDir>
#include <QDebug>

class ApplicationSettings;

class ClipboardJSONFormatter : public QObject
{
    Q_OBJECT
public:
    ClipboardJSONFormatter(ApplicationSettings * s, QObject* parent = 0) :
        QObject(parent),
        settings(s)
    {
        storageLocation = QDir::currentPath() + "/storage/";
        qDebug() << "STORAGE LOCATION : " << storageLocation;
        QDir dir(storageLocation);
        if (!dir.exists()) { dir.mkdir("."); }
    }
    ~ClipboardJSONFormatter() {}
    QJsonDocument getRequestFormat(const QMimeData* data);

signals:
    void emitMessage(MessageType, QString message);

private:
    /**
     *  @brief processImage
     *  Store the QImage in the temporary folder of the application,
     *  return the local file location
     */
    QString processImage(QImage img);

    /**
     * @brief processFiles
     * Take the list of file or folder locations, merge them all into a single file
     * return the location of the merged file
     */
    QString processFilesFolders(QList<QString> files, QList<QString> folders);

    /**
     * @brief appendFile
     * append the file found at sourceFileLocation to the archive at the targetArchiveLocation,
     * keeping track of the relative root directory and using the miniz.c code
     * @return the added size
     */
    int appendFile(QString sourceFileLocation, QDir rootDir, QString targetArchiveLocation);

    /**
     * @brief getProperTarget
     *
     */
    QString getProperTarget(QString wantedFileName);

    QString storageLocation;
    ApplicationSettings * settings;
};

#endif // CLIPBOARDJSONFORMATTER_H
