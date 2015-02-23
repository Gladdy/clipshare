#include "clipsharerunner.h"
#include "statuswindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char** argv)
{
    Q_INIT_RESOURCE(clipshare);

    QApplication app(argc, argv);
    //app.setQuitOnLastWindowClosed(false);

    StatusWindow * window = new StatusWindow();
    ClipShareRunner * runner = new ClipShareRunner();

    QObject::connect(window,SIGNAL(emitCommand(QString, QString)),runner,SLOT(processCommand(QString,QString)));
    QObject::connect(runner,SIGNAL(emitNotification(QString, QString)),window,SLOT(processNotification(QString,QString)));

    runner->initialize();
    window->show();

    return app.exec();
}
