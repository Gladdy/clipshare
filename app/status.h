#ifndef STATUS_H
#define STATUS_H

#include <QObject>

enum MessageType { URL, Error, Progress, Login, Notification };
enum CommandType { Connect };

class StatusReporter : public QObject {
  Q_OBJECT

public:
  StatusReporter(QObject *parent = 0);

signals:
  void emitMessage(MessageType, QString message);
};

#endif // STATUS_H
