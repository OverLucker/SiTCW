#include "sitcw.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SiTCW w;
	w.show();
	return a.exec();
}
