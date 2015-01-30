#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include "tcpclient.h"

#include <QObject>
#include <QApplication>
#include <QClipboard>

class ClipShareRunner : public QObject
{
    Q_OBJECT
public:
    ClipShareRunner(QApplication* a, QObject* parent = 0);
    ~ClipShareRunner() {}
    void initialize();

signals:
    void writeToSocket(QString);

public slots:
    void processClipboardChange();
    void readFromSocket(QString);

private:
    QApplication * app;
    TcpClient * tcpclient;
    QMimeData * mimeData;

    void processConfigFile();

    QStringList supportedTypes {"text/plain","text/html"};
    const QString configFilename = "config.cfg";
};

#endif // CLIPSHARERUNNER_H
