#include "clipsharerunner.h"
#include "statuswindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char** argv)
{
    Q_INIT_RESOURCE(clipshare);

    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(
            0,
            QObject::tr("Systray"),
            QObject::tr("I couldn't detect any system tray on this system.")
        );

        return 1;
    }

    ClipShareRunner * runner = new ClipShareRunner();
    StatusWindow * window = new StatusWindow(runner->settings);

    QObject::connect(window,SIGNAL(emitCommand(CommandType, QString)),runner,SLOT(processCommand(CommandType,QString)));
    QObject::connect(runner,SIGNAL(emitMessage(MessageType, QString)),window,SLOT(processMessage(MessageType,QString)));

    return app.exec();
}
