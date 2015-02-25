#ifndef CLIPBOARDJSONFORMATTER_H
#define CLIPBOARDJSONFORMATTER_H

#include <QObject>

class ApplicationSettings;

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

signals:
    void emitNotification(QString, QString);

private:
    ApplicationSettings * settings;
};

#endif // CLIPBOARDJSONFORMATTER_H
