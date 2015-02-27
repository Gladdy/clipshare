#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include <QObject>
#include <QMimeData>
#include <QString>
#include <QJsonDocument>
#include <QList>
#include <QTime>

class NetworkManager;
class ApplicationSettings;
class ClipboardJSONFormatter;

class ClipShareRunner : public QObject
{
    Q_OBJECT
public:
    ClipShareRunner(QObject* parent = 0);
    void initialize();
    void attemptLogin();
    ApplicationSettings * settings;

signals:
    void emitNetworkRequest(QJsonDocument);
    void emitNotification(QString, QString);

public slots:
    void processClipboardChange();

    void processNetworkResponse(QJsonDocument);
    void processCommand(QString, QString);
    void processNotification(QString, QString);

private:
    bool loggedIn = false;

    ClipboardJSONFormatter * formatter;
    NetworkManager * manager;

    QMimeData * mimeData;

    QList<QTime> clipboardTriggerList;
};

#endif // CLIPSHARERUNNER_H
