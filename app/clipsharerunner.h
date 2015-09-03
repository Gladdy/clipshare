#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include "messagetype.h"

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
    ApplicationSettings * settings;

signals:
    void emitNetworkRequest(QJsonDocument);
    void emitMessage(MessageType, QString message);

public slots:
    void processClipboardChange();

    void processNetworkResponse(QJsonDocument);
    void processCommand(CommandType, QString command);

    /**
     * @brief processMessage
     * Aggregate all messages from the ApplicationSettings, ClipboardJSONFormatter and NetworkManager
     * forward them to the StatusWindow
     */
    void processMessage(MessageType, QString message);

private:
    bool loggedIn = false;
    void setClipboardText(QString url);
    bool ignoreClipboardChange = false;

    ClipboardJSONFormatter * formatter;
    NetworkManager * manager;

    QList<QTime> clipboardTriggerList;
};

#endif // CLIPSHARERUNNER_H
