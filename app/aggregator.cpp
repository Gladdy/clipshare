#include "aggregator.h"
#include "settings.h"
#include "miniz.c"

#include <QDebug>

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDirIterator>
#include <QJsonObject>

#include <QApplication>
#include <QClipboard>

Aggregator::Aggregator(Settings *s, QObject *parent)
    : StatusReporter(parent),
      settings(s)
{

  QDir dir(QDir::currentPath());

  if (!dir.exists(storageName)) {
    dir.mkdir(storageName);
  }

  dir.cd(storageName);
  storageLocation = dir.absolutePath() + "/";
}

Aggregator::~Aggregator() {

}

QString Aggregator::aggregateClipboard() {

  const QMimeData *mimeData = QApplication::clipboard()->mimeData();

  if(mimeData->hasUrls())
  {
    qDebug() << "Detected file(s)";

    // Files were copied
    QList<QUrl> urls = mimeData->urls();

    QList<QString> files;
    QList<QString> folders;

    // Check the validity of the files and folders and add them to the lists
    for (QUrl url : urls) {
      QString localname = url.toLocalFile();
      QDir dir(localname);
      QFile file(localname);

      if (dir.exists())
      {
        folders.append(dir.absolutePath());
        qDebug() << "FOLDER: " << dir;
      }
      else if (file.exists())
      {
        QFileInfo info(file);
        files.append(info.absoluteFilePath());
        qDebug() << "FILE: " << info.absoluteFilePath();
      }
      else
      {
        qDebug() << "URL not found: " << url;
      }
    }

    // Process the files and folders and return the possible location
    return processFilesFolders(files, folders);
  }
  else
  {
    qDebug() << "Detected text or html snippet";

    // Text or an html snippet
    QString content;
    QString filename;

    if(mimeData->hasHtml()) {
      content = mimeData->html();
      filename = resolveAvailable("html-snippet.html");
    }
    else if(mimeData->hasText())
    {
      content = mimeData->text();
      filename = resolveAvailable("text-snippet.txt");
    }


    QFile file (filename);
    if(file.open(QIODevice::ReadWrite)) {
      file.write(content.toStdString().c_str(), content.length());
      file.close();
      return filename;
    }
  }
  return "";
}

QString Aggregator::processFilesFolders(QList<QString> files,
                                        QList<QString> folders) {
  QDir dir(".");
  if (!dir.exists(storageName)) {
    dir.mkdir(storageName);
  }

  // Initialize variables and fetch settings
  qint64 totalSize = 0;
  qint64 maxsize = settings->getSetting("maxsize").toInt();
  bool overflow = false;
  QString filename;

  // Zip when multiple files or a folder
  if (files.length() > 1 || folders.length())
  {
    qDebug() << "PREPARE FOR ZIPPING";

    // Extract a logical filename from the files and folders
    if (files.length() > 1) {

      QFileInfo info0(files.at(0));
      filename = info0.baseName().append("_etc");

    } else {
      // Get a name from the folders
      QDir dir(folders.at(0));
      filename = dir.dirName();
    }

    // Check and fix the availability of the filename with extension
    filename.append(".zip");
    filename = resolveAvailable(filename);

    // Find the root directory of the selected files/folder(s)
    QDir rootDir = getRootLocation(files,folders);

    // Add the files to the archive
    for (QString file : files) {
      if (totalSize > maxsize) {
        overflow = true;
        break;
      }
      totalSize += appendFile(file, rootDir, filename);
      qDebug() << totalSize << maxsize;
    }

    // Add the folders to the archive by adding the files in the folders
    for (QString folder : folders) {
      QDir dir(folder);
      dir.setFilter(QDir::Files);
      QDirIterator it(dir, QDirIterator::Subdirectories);

      while (it.hasNext()) {
        if (totalSize > maxsize) {
          overflow = true;
          break;
        }
        totalSize += appendFile(it.next(), rootDir, filename);
        qDebug() << totalSize << maxsize;
      }
    }
  }
  else
  {
    //Only a single file is selected
    QString sourceFilename = files.first();
    QFile sourceFile(sourceFilename);

    if(!sourceFile.exists())
    {
      //What is even going on
      emitMessage(Notification, tr("File does not exist: ") + sourceFilename);
      return "";
    }
    else if(sourceFile.size() > maxsize)
    {
      //Too large, too bad
      overflow = true;
    }
    else
    {
      qDebug() << "going to copy";

      //Yes, correct filename and copy
      QFileInfo info(sourceFile);
      filename = resolveAvailable(info.fileName());

      QFile::copy(sourceFilename, filename);
    }
  }

  // Handle a possible overflow
  if (overflow == true) {
    emitMessage(Notification, tr("Unable to process file due to size") +
                ", max " + QString::number(maxsize/1000) + "KB");

    //Attempt to remove the file again
    QFile file (filename);
    if(file.exists()) {
      file.remove();
    }
    return "";
  }

  return filename;
}

int Aggregator::appendFile(QString sourceFileLocation, QDir rootDir,
                           QString targetArchiveLocation) {
  qDebug() << "ROOTDIR: " << rootDir.path();

  int size = 0;
  QFile sourceFile(sourceFileLocation);
  QString localFilename = rootDir.relativeFilePath(sourceFileLocation);

  if (sourceFile.open(QIODevice::ReadOnly)) {
    QByteArray qdata = sourceFile.readAll();
    sourceFile.close();

    size = qdata.size();

    const char *comment = "Adding file";

    char archive_name_buffer[1024];
    char archive_local_filename_buffer[1024];
    const char *file_data = qdata.constData();

    sprintf(archive_name_buffer, "%s",
            targetArchiveLocation.toLatin1().constData());
    sprintf(archive_local_filename_buffer, "%s",
            localFilename.toLatin1().constData());

    mz_zip_add_mem_to_archive_file_in_place(
        archive_name_buffer, archive_local_filename_buffer, file_data, size,
        comment, (unsigned short)strlen(comment), MZ_BEST_COMPRESSION);

    qDebug() << "Added\n" << localFilename << " from\n" << sourceFileLocation
             << " to\n" << targetArchiveLocation << size;
  } else {
    qDebug() << "Error adding: " << sourceFileLocation
             << sourceFile.errorString();
  }

  return size;
}

/*
 * Input:   a bare filename which gets requested to be used, eg. main.cpp
 * Output:  the fully formed path to the storage directory with a guaranteed, nonexisting filename
 */
QString Aggregator::resolveAvailable(QString filenameWithExtension) {


  //int periodPosition = filenameWithExtension.indexOf('.');
  QString name = filenameWithExtension.section('.', 0, 0);
  QString extension = filenameWithExtension.section('.', 1);


  QString target = storageLocation + name + "." + extension;
  QFile *targetFile = new QFile(target);
  int i = 2;

  // Make sure that we have an unique target file name
  while (targetFile->exists()) {
    target = storageLocation + name + "-" + QString::number(i) + "." + extension;
    delete targetFile;
    targetFile = new QFile(target);
    i++;
  }
  delete targetFile;

  return target;
}

QDir Aggregator::getRootLocation(QList<QString> files, QList<QString> folders) {

  // First guess for the rootlocation
  QString rootLocation = files.length() ? files[0] : folders[0];

  // Perform a longest prefix search over both lists.
  // Simply consider every single string and

  // Put all
  QList<QString> aggregatelist;
  aggregatelist.append(files);
  aggregatelist.append(folders);

  for (QString location : aggregatelist) {
    QString tempRootLocation = "";
    int i = 0;
    while (i < location.length() && i < rootLocation.length() && location.at(i) == rootLocation.at(i)) {
      tempRootLocation.append(location.at(i));
      i++;
    }
    rootLocation = tempRootLocation;
  }

  // Make sure that you end up with a folder location
  // Ending in '/'
  int i = rootLocation.length() - 1;
  while (i > 0) {
    if (rootLocation.at(i) == '/')
      break;
    i--;
  }

  return QDir(rootLocation.left(i));
}
