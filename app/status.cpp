#include "status.h"

StatusReporter::StatusReporter(QObject *parent) : QObject(parent) {
  connect(this, SIGNAL(emitMessage(MessageType, QString)), parent,
          SLOT(processMessage(MessageType, QString)));
}
