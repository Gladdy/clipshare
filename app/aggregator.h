#ifndef CLIPBOARDJSONFORMATTER_H
#define CLIPBOARDJSONFORMATTER_H

#include "status.h"

#include <QObject>
#include <QMimeData>
#include <QJsonDocument>
#include <QList>
#include <QUrl>
#include <QImage>
#include <QDir>
#include <QDebug>

class Settings;

class Aggregator : public StatusReporter {
  Q_OBJECT
public:
  Aggregator(Settings*, QObject *parent = 0);
  ~Aggregator() {}
  QJsonDocument getRequestFormat(const QMimeData *data);

signals:
  void emitMessage(MessageType, QString message);

private:
  const QString storageDir = "storage";
  /**
   *  @brief processImage
   *  Store the QImage in the temporary folder of the application,
   *  return the local file location
   */
  QString processImage(QImage img);

  /**
   * @brief processFiles
   * Take the list of file or folder locations, merge them all into a single
   * file
   * return the location of the merged file
   */
  QString processFilesFolders(QList<QString> files, QList<QString> folders);

  /**
   * @brief appendFile
   * append the file found at sourceFileLocation to the archive at the
   * targetArchiveLocation,
   * keeping track of the relative root directory and using the miniz.c code
   * @return the added size
   */
  int appendFile(QString sourceFileLocation, QDir rootDir,
                 QString targetArchiveLocation);

  /**
   * @brief getProperTarget
   *
   */
  QString getProperTarget(QString wantedFileName);

  QString storageLocation;
  Settings * settings;
};

#endif // CLIPBOARDJSONFORMATTER_H
