#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include "tcpclient.h"

#include <QObject>
#include <QMimeData>
#include <QStringList>
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
    }
    void initialize();

signals:
    void writeToSocket(QString);
    void error(int, QString);
    void writingToClipboard();
    void readingClipboardUpdate();

public slots:
    void processClipboardChange();
    void readFromSocket(QString);

    void displayError(int, QString);

private:
    TcpClient * tcpclient;
    QMimeData * mimeData; //the current pointer to the mimedata, but others will delete it

    QTime lastUpdated;
    QStringList supportedTypes;

    void readConfigFile();
    void applyConnectionConfig();
    void applyTransmissionConfig();

    QJsonDocument config;
    const QString configFilename = "config.txt";
    int maxTransmitSize = INT_MAX;
};

#endif // CLIPSHARERUNNER_H
