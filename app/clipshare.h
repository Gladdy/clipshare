#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include "status.h"

#include <QObject>
#include <QMimeData>
#include <QString>
#include <QJsonDocument>
#include <QList>
#include <QTime>

#include <memory>

class NetworkIO;
class Aggregator;
class Window;
class Settings;

class Clipshare : public QObject {
  Q_OBJECT
public:
  Clipshare(QObject *parent = 0);

signals:
  void emitNetworkRequest(QJsonDocument);
  void emitMessage(MessageType, QString message);

public slots:
  void processClipboardChange();

  void processNetworkResponse(QJsonDocument);
  void processCommand(CommandType, QString command);

  /**
   * @brief processMessage
   * Aggregate all messages
   * forward them to the GUI
   */
  void processMessage(MessageType, QString message);

private:
  // bool loggedIn = false;
  void setClipboardText(QString url);
  bool ignoreClipboardChange = false;

  // Keep track of the application settings
  Settings *settings;

  // Aggregate the clipboard data into a single file
  Aggregator *aggregator;

  // Send the single file over the network
  NetworkIO *network;

  // Display the GUI
  Window *window;

  // Times at which the clipboard got triggered
  QList<QTime> clipboardTriggerList;
};

#endif // CLIPSHARERUNNER_H
