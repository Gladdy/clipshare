#include "clipboardjsonformatter.h"
#include "applicationsettings.h"
#include "miniz.c"

#include <QDebug>

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QJsonObject>

QJsonDocument ClipboardJSONFormatter::getRequestFormat(const QMimeData *dataSource)
{
    qDebug() << "Getting formats";

    QJsonObject mimeDataResult;

    QStringList formats = dataSource->formats();

    for(QString format : formats)
    {
        if( !(format.startsWith("application/") || format.startsWith("image/")) )
        {
            qDebug() << "Found format: " << format;
            mimeDataResult.insert(format, QString(dataSource->data(format)));
        }
        else
        {
            qDebug() << "Found format: " << format << "!! NOT PULLING !!";
        }
    }

    /*
     *  The clipboard contains either an image or a file, but not both for this purpose
     */
    if(dataSource->hasImage())
    {
        QVariant v = dataSource->imageData();
        QImage image = v.value<QImage>();

        qDebug() << "has image (not yet added to the JSON) : " << image.size();
        //mimeDataResult.insert("type","image");
    }
    else if(dataSource->hasUrls())
    {
        QList<QUrl> files = dataSource->urls();
        QList<QString> stringFiles;

        qDebug() << "Files to be added";
        for(QUrl url : files ) {
            QString localFileName = url.toLocalFile();
            QFile localFile (localFileName);
            if(localFile.exists()) {
                stringFiles.append(localFileName);
                qDebug() << localFileName;
            }
        }

        if(stringFiles.length() > 0)
        {
            QString location = processFiles(stringFiles);

            if(location != "")
            {
                mimeDataResult.insert("type","file");
                mimeDataResult.insert("location",location);
            }
            else
            {
                emitNotification("Error",tr("Copied files are too large to be sent"));
                return QJsonDocument();
            }
        }
    }

    /*
     *  Additionally, add the textual representation to the JSON structure.
     */
    if(!mimeDataResult.contains("type")) {
        mimeDataResult.insert("type","text");
    }
    if(dataSource->hasText()) {
        mimeDataResult.insert("text/plain",dataSource->text());
    }
    if(dataSource->hasHtml()) {
        mimeDataResult.insert("text/html",dataSource->html());
    }


    QJsonDocument doc;
    doc.setObject(mimeDataResult);

    qDebug() << "ended clipboardjsonformatter";

    return doc;
}
QString ClipboardJSONFormatter::processImage(QImage img) {
    return "";
}
QString ClipboardJSONFormatter::processFiles(QList<QString> files)
{
    qint64 totalSize = 0;
    qint64 maxSize = settings->getSetting("uploadSizeLimit").toInt()*1000;
    bool overflow = false;


    QDir rootDir (files.first());
    if(files.length() == 1 && !rootDir.exists())
    {
        /**
         *  Simply emit the filename for uploading
         *  If there is only a single entry and that is not a folder
         */
        QString onlyFile = files.first();

        QFile file (onlyFile);
        if(file.open(QIODevice::ReadOnly))
        {
            QByteArray data = file.readAll();
            totalSize = data.length();
            file.close();

            qDebug() << "Opened: " << onlyFile << totalSize;

            if(totalSize < maxSize)
            {
                return onlyFile;
            }
        }
        else
        {
            qDebug() << "Error: " << onlyFile << file.errorString();
        }

        return "";
    }
    else
    {
        /**
         * Start zipping files because (possibly multiple) folders or multiple files were copied
         * Delete the archive to start fresh
         */
        QString target = "buffer";
        target.append(".zip");
        target.prepend(QDir::currentPath() + "/");
        remove(target.toLatin1().constData());

        QList<QString> fileLocations;
        QList<QString> folderLocations;

        /*
         * Split the selection into files and folders
         */
        for(QString filename : files)
        {
            QDir dir (filename);
            if(dir.exists()) {
                folderLocations.append(filename);
            } else {
                fileLocations.append(filename);
            }
        }

        /*
         * Extract the root of the copying from the elements
         */
        QString rootElement;
        if(fileLocations.length()) {
            rootElement = fileLocations.at(0);
        } else {
            rootElement = folderLocations.at(0);
        }

        int i = rootElement.size() - 1;

        while(rootElement.at(i) != '/' && rootElement.at(i) != '\\') {
            i--;
        }

        QString root = rootElement;
        root.truncate(i+1);

        /*
         * Add the files to the archive
         */
        for(QString filename : fileLocations)
        {
            if(overflow == true) { break; }

            totalSize += appendFile(filename, root, target);
            if(totalSize > maxSize) { overflow = true; }
            qDebug() << totalSize << maxSize;
        }

        /*
         * Add the files in the folders recursively to the archive
         */
        for(QString filename : folderLocations)
        {
            if(overflow == true) { break; }

            QDir dir (filename);
            dir.setFilter(QDir::Files);
            QDirIterator it(dir, QDirIterator::Subdirectories);

            while(it.hasNext() && overflow == false)
            {
                totalSize += appendFile(it.next(), root, target);
                if(totalSize > maxSize) { overflow = true; }
                qDebug() << totalSize << maxSize;
            }
        }

        if(overflow == true)
        {
            remove(target.toLatin1().constData());
            return "";
        }
        else
        {
            return target;
        }
    }
}
int ClipboardJSONFormatter::appendFile(QString filename, QString root, QString target)
{
    int size = 0;
    QFile file (filename);

    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray qdata = file.readAll();
        size = qdata.size();

        QString localFilename = filename.mid(root.size());
        const char * comment = "Adding file";

        char archive_name_buffer[1024];
        char archive_local_filename_buffer[1024];
        const char * file_data = qdata.constData();

        sprintf(archive_name_buffer, "%s", target.toLatin1().constData());
        sprintf(archive_local_filename_buffer, "%s", localFilename.toLatin1().constData());

        mz_zip_add_mem_to_archive_file_in_place(
                    archive_name_buffer,
                    archive_local_filename_buffer,
                    file_data,
                    size,
                    comment,
                    (unsigned short)strlen(comment),
                    MZ_BEST_COMPRESSION
                    );

        qDebug() << "Added" << filename << " : " << localFilename << " @ " << target << size;
        file.close();
    }
    else
    {
        //qDebug() << "Error: " << filename << file.errorString();
    }

    return size;
}
