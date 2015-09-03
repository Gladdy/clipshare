#include "clipboardjsonformatter.h"
#include "applicationsettings.h"
#include "miniz.c"

#include <QDebug>

#include <QDir>
#include <QFileInfo>
#include <QFile>
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
        QList<QUrl> urls = dataSource->urls();

        QList<QString> files;
        QList<QString> folders;

        qDebug() << "Files to be added";
        for(QUrl url : urls )
        {
            QString localname = url.toLocalFile();
            QDir dir (localname);
            QFile file (localname);

            if(dir.exists())
            {
                folders.append(dir.absolutePath());
                qDebug() << "FOLDER: " << dir;
            }
            else if (file.exists())
            {
                QFileInfo info (file);
                files.append(info.absoluteFilePath());
                qDebug() << "FILE: " << info.absoluteFilePath();
            }
            else {
                qDebug() << "URL not found: " << url;
            }
        }

        if(files.length() || folders.length())
        {
            QString location = processFilesFolders(files, folders);

            if(location != "")
            {
                mimeDataResult.insert("type","file");
                mimeDataResult.insert("location",location);
            }
            else
            {
                emitMessage(Error,tr("Copied files are too large to be sent"));
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

    qDebug() << "Finished formatting clipboard as JSON";

    return doc;
}
QString ClipboardJSONFormatter::processImage(QImage img) {
    return "hai";
}
QString ClipboardJSONFormatter::processFilesFolders(QList<QString> files, QList<QString> folders)
{
    QDir dir(storageLocation);
    if (!dir.exists()) { dir.mkdir("."); }

    /*
     *  Fetch settings and initialize variables
     */
    qint64 totalSize = 0;
    qint64 maxSize = settings->getSizeLimit();
    bool overflow = false;

    //You need to zip when there are multiple locations selected or you selected a folder
    bool needsZipping = (files.length() > 1 || folders.length());

    if(needsZipping)
    {
        qDebug() << "PREPARE FOR ZIPPING";

        // Extract a logical filename from the files and folders
        // Find the root directory of the selected files
        QString targetFilename;
        QString rootLocation;

        if(files.length() > 1)
        {
            // Get a name from the files
            QFileInfo info0 (files.at(0));
            QFileInfo info1 (files.at(1));
            targetFilename = info0.baseName() + "_" + info1.baseName();

            if(files.length() > 2) {
                targetFilename.append("_etc");
            }

            rootLocation = files.at(0);
        }
        else
        {
            // Get a name from the folders
            QDir dir (folders.at(0));
            targetFilename = dir.dirName();
            rootLocation = folders.at(0);
        }

        targetFilename.append(".zip");

        QString target = getProperTarget(targetFilename);

        for(QString file : files) {
            QString tempRootLocation = "";
            int i = 0;
            while(i < file.length() && i < rootLocation.length() && file.at(i) == rootLocation.at(i)) {
                tempRootLocation.append(file.at(i));
                i++;
            }
            rootLocation = tempRootLocation;
        }
        for(QString folder : folders) {
            QString tempRootLocation = "";
            int i = 0;
            while(i < folder.length() && i < rootLocation.length() && folder.at(i) == rootLocation.at(i)) {
                tempRootLocation.append(folder.at(i));
                i++;
            }
            rootLocation = tempRootLocation;
        }

        //Deal with the case that all selected files start with the same characters.
        //If so, you'd end up with '/home/martijn/te' for 'text.txt' and 'test.txt' instead of /home/martijn
        int i = rootLocation.length() - 1;
        while(i > 0) {
            if(rootLocation.at(i) == '/') break;
            i--;
        }

        rootLocation = rootLocation.left(i);
        QDir rootDir (rootLocation);


        /*
         * Add the files to the archive
         */
        for(QString file : files)
        {
            if(totalSize > maxSize) { overflow = true; break;}
            totalSize += appendFile(file, rootDir, target);
            qDebug() << totalSize << maxSize;
        }

        /*
         * Add the files in the folders recursively to the archive
         */
        for(QString folder : folders)
        {
            QDir dir (folder);
            dir.setFilter(QDir::Files);
            QDirIterator it(dir, QDirIterator::Subdirectories);

            while(it.hasNext())
            {
                if(totalSize > maxSize) { overflow = true; break;}
                totalSize += appendFile(it.next(), rootDir, target);
                qDebug() << totalSize << maxSize;
            }
        }

        if(overflow == false)
        {
            return target;
        }
    }
    else
    {
        /**
         *  Simply emit the filename for uploading
         *  If there is only a single entry and that is not a folder
         */
        QString fileLocation = files.first();
        QFileInfo info (fileLocation);
        QString target = getProperTarget(info.fileName());
        QFile::copy(fileLocation, target);
        QFile targetFile (target);

        if(targetFile.exists())
        {
            if(targetFile.size() <= maxSize)
            {
                qDebug() << "Copied to : " + target;
                return target;
            }
            else
            {
                qDebug () << "File too large";
            }
        }
        else
        {
            return "";
        }
    }

    return "";
}
int ClipboardJSONFormatter::appendFile(QString sourceFileLocation, QDir rootDir, QString targetArchiveLocation)
{
    qDebug() << "ROOTDIR: " << rootDir.path();

    int size = 0;
    QFile sourceFile (sourceFileLocation);
    QString localFilename = rootDir.relativeFilePath(sourceFileLocation);

    if(sourceFile.open(QIODevice::ReadOnly))
    {
        QByteArray qdata = sourceFile.readAll();
        sourceFile.close();

        size = qdata.size();

        const char * comment = "Adding file";

        char archive_name_buffer[1024];
        char archive_local_filename_buffer[1024];
        const char * file_data = qdata.constData();

        sprintf(archive_name_buffer, "%s", targetArchiveLocation.toLatin1().constData());
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

        qDebug() << "Added\n" << localFilename << " from\n" << sourceFileLocation << " to\n" << targetArchiveLocation << size;
    }
    else
    {
        qDebug() << "Error adding: " << sourceFileLocation << sourceFile.errorString();
    }

    return size;
}
QString ClipboardJSONFormatter::getProperTarget(QString wantedFileName)
{
    QString target = storageLocation + wantedFileName;
    QFile * targetFile = new QFile(target);
    int i = 2;

    //Make sure that we have an unique target file name
    while(targetFile->exists()) {
        target = storageLocation + QString::number(i) + "-"+ wantedFileName;
        delete targetFile;
        targetFile = new QFile(target);
        i++;
    }
    delete targetFile;

    return target;
}
