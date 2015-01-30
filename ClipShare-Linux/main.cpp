#include "clipsharerunner.h"
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication * app = new QApplication(argc, argv);
    ClipShareRunner * csr = new ClipShareRunner(app);
    csr->initialize();
    return app->exec();
}

