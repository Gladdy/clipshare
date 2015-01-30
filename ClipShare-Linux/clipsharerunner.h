#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include "tcpclient.h"

#include <QObject>
#include <QApplication>
#include <QMimeData>
#include <QStringList>
#include <QString>
#include <QMutex>

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

    QMutex clipboardMutex;

    void processConfigFile();

    QStringList supportedTypes {"text/plain","text/html"};
    const QString configFilename = "config.cfg";

	bool updatingClipboard = false;
};

#endif // CLIPSHARERUNNER_H
