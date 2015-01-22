#include "clipsharerunner.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	ClipShareRunner csr (a.clipboard());

	return a.exec();
}
