#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include "tcpclient.h"
#include "applicationsettings.h"
#include "clipboardjsonformatter.h"

#include <QObject>
#include <QMimeData>
#include <QString>
#include <QTime>
#include <QJsonDocument>

class ClipShareRunner : public QObject
{
    Q_OBJECT
public:
    ClipShareRunner(QObject* parent = 0);
    ~ClipShareRunner() {
        delete tcpclient;
        delete settings;
    }
    void initialize();

signals:
    void writeToSocket(QString);
    void error(int, QString);
    void readingClipboardUpdate();

public slots:
    void processClipboardChange();
    void readFromSocket(QString);

    void displayError(int severity, QString msg) { qDebug() << severity << " : " << msg; }

private:
    TcpClient * tcpclient;
    QMimeData * mimeData;
    ApplicationSettings * settings;
    ClipboardJSONFormatter * formatter;

    QTime lastUpdated;
};

#endif // CLIPSHARERUNNER_H
