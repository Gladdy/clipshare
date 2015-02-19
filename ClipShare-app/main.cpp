#include "clipsharerunner.h"
#include "statuswindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char** argv)
{
    Q_INIT_RESOURCE(clipshare);

    QApplication app(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("No systray detected"));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    StatusWindow window;
    window.hide();

    ClipShareRunner csr;
    csr.initialize();

    return app.exec();
}
