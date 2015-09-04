#include "clipshare.h"

#include "settings.h"
#include "aggregator.h"
#include "networkio.h"
#include "window.h"

#include <QApplication>
#include <QClipboard>
#include <QJsonValue>
#include <QJsonObject>
#include <QDebug>
#include <QFile>

Clipshare::Clipshare(QObject *parent) : QObject(parent) {

  settings = new Settings(this);
  aggregator = new Aggregator(settings, this);
  network = new NetworkIO(settings, this);
  window = new Window(settings, this);

  // Clipboard change
  connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &Clipshare::processClipboardChange);

  // Network connections
  connect(this, &Clipshare::emitNetworkRequest, network,
          &NetworkIO::processNetworkRequest);
  connect(network, &NetworkIO::emitNetworkResponse, this,
          &Clipshare::processNetworkResponse);

  // GUI notifications and commands
  connect(this, &Clipshare::emitMessage, window, &Window::processMessage);
  connect(window, &Window::emitCommand, this, &Clipshare::processCommand);

  settings->load();
  window->fillFields();
  network->checkCredentials();
}
void Clipshare::processClipboardChange() {
  qDebug() << "Clipboard changed!";

  if (ignoreClipboardChange) {
    ignoreClipboardChange = false;
    return;
  }

  // Don't trigger 2 times within 50 milliseconds
  if (!clipboardTriggerList.isEmpty() &&
      clipboardTriggerList.last().elapsed() < 50) {
    return;
  }

  // Filter out the events outside the DoubleCopyPeriod
  clipboardTriggerList.push_back(QTime::currentTime());
  int period = settings->getSetting("triggerperiod").toInt();

  while (clipboardTriggerList.front().elapsed() > period) {
    clipboardTriggerList.removeFirst();
  }

  if (clipboardTriggerList.length() >= 2) {

    QString location = aggregator->aggregateClipboard();


    //emitNetworkRequest(request);
  }
}

void Clipshare::processNetworkResponse(QJsonDocument doc) {
  QJsonObject response = doc.object();

  if (response.contains("loggedin")) {
    if (response["loggedin"].toBool() == true) {
      emitMessage(Login, "Correct");
    } else {
      emitMessage(Login, "Incorrect");
    }
  }

  if (response.contains("url")) {
    QString url = response["url"].toString();
    setClipboardText(url);
    emitMessage(URL, url);
  }
}
void Clipshare::setClipboardText(QString url) {
  ignoreClipboardChange = true;
  QMimeData *mimeData = new QMimeData();
  mimeData->setText(url);
  QApplication::clipboard()->setMimeData(mimeData);
}

void Clipshare::processMessage(MessageType type, QString message) {
  emitMessage(type, message);
}

void Clipshare::processCommand(CommandType type, QString message) {
  switch (type) {
  case Connect:
    if (message == "CheckCredentials") {
      network->checkCredentials();
    }
    break;
  }
}
