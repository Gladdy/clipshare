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

#include <iostream>

Clipshare::~Clipshare() {}

Clipshare::Clipshare(QObject *parent) : QObject(parent) {

  settings = new Settings(this);
  aggregator = new Aggregator(settings, this);
  network = new NetworkIO(settings, this);
  window = new Window(settings, this);

  // Clipboard change
  connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &Clipshare::processClipboardChange);

  // Network connections
  connect(this, &Clipshare::upload, network, &NetworkIO::upload);
  connect(network, &NetworkIO::emitResult, this, &Clipshare::processResult);

  // GUI notifications and commands
  connect(this, &Clipshare::emitMessage, window, &Window::processMessage);
  connect(window, &Window::emitCommand, this, &Clipshare::processCommand);

  // Crisply close the app by hiding the sqlite save going on
  connect(settings, &Settings::triggerHide, window, &Window::hide);

  settings->load();
  window->fillFields();

  if(settings->getSetting("email").toString().length() > 5 &&
     settings->getSetting("password").toString().length() > 3) {
    network->login();
  }
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

  // Trigger whenever there are at least 2 matching events.
  if (clipboardTriggerList.length() >= 2) {

    QString location = aggregator->aggregateClipboard();

    if(location != "")
    {
      emit upload(location);
    }
    else
    {
      emit emitMessage(Notification, "something went wrong in uploading this");
    }
  }
}

void Clipshare::processResult(QString str) {

  qDebug() << "Processing network response in Clipshare: " << str;

  QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());

  if(doc.isNull()) {
    emitMessage(Notification, str);
    return;
  }

  QJsonObject response = doc.object();

  if (response.contains("authenticated")) {
    if (response["authenticated"].toBool() == true) {
      emitMessage(Login, "true");
    } else {
      emitMessage(Login, "false");
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
    if (message == "login") {
      network->login();
    }
    break;
  }
}
