#ifndef CLIPBOARDJSONFORMATTER_H
#define CLIPBOARDJSONFORMATTER_H

#include <QObject>
#include <QMimeData>
#include <QJsonDocument>
#include <QList>
#include <QUrl>
#include <QImage>

class ApplicationSettings;
class QuaZip;

class ClipboardJSONFormatter : public QObject
{
    Q_OBJECT
public:
    ClipboardJSONFormatter(ApplicationSettings * s, QObject* parent = 0) : QObject(parent)
    {
        settings = s;
    }
    ~ClipboardJSONFormatter() {

    }
    QJsonDocument getRequestFormat(const QMimeData* data);

signals:
    void emitNotification(QString, QString);

private:
    QString processImage(QImage img);
    QString processFiles(QList<QString> files);
    int appendFile(QString files, QString root, QString target);


    ApplicationSettings * settings;
};

#endif // CLIPBOARDJSONFORMATTER_H
