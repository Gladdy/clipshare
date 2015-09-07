#include "clipshare.h"
#include "window.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char **argv) {
  Q_INIT_RESOURCE(clipshare);

  QApplication app(argc, argv);
  QApplication::setQuitOnLastWindowClosed(false);

  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    QMessageBox::critical(
        0, QObject::tr("Systray"),
        QObject::tr("I couldn't detect any system tray on this system."));

    return 1;
  }

  Clipshare application;

  return app.exec();
}
