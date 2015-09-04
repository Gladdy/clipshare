#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "status.h"

#include <QObject>
#include <QMimeData>
#include <QJsonDocument>
#include <QList>
#include <QUrl>
#include <QImage>
#include <QDir>
#include <QDebug>
#include <QFile>

class Settings;

class Aggregator : public StatusReporter {
  Q_OBJECT
public:
  Aggregator(Settings*, QObject *parent = 0);
  ~Aggregator();
  QString aggregateClipboard();

private:
  const QString storageName = "storage";
  QString storageLocation; //the absolute path

  QString processFilesFolders(QList<QString> files, QList<QString> folders);
  int appendFile(QString sourceFileLocation, QDir rootDir,
                 QString targetArchiveLocation);

  /**
   * @brief getProperTarget
   *
   */
  QString resolveAvailable(QString wantedFileName);
  QDir getRootLocation(QList<QString> files, QList<QString> folders);

  Settings * settings;
};

#endif // AGGREGATOR_H
