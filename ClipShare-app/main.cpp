#include "clipsharerunner.h"
#include "statuswindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char** argv)
{
    Q_INIT_RESOURCE(clipshare);

    QApplication app(argc, argv);
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(
            0,
            QObject::tr("Systray"),
            QObject::tr("I couldn't detect any system tray on this system.")
        );

        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    ClipShareRunner * runner = new ClipShareRunner();
    runner->initialize();

    StatusWindow * window = new StatusWindow(runner->settings);
    window->show();

    QObject::connect(window,SIGNAL(emitCommand(QString, QString)),runner,SLOT(processCommand(QString,QString)));
    QObject::connect(runner,SIGNAL(emitNotification(QString, QString)),window,SLOT(processNotification(QString,QString)));

    runner->attemptLogin();

    return app.exec();
}
