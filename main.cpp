#include "sitcw.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SiTCW w;
	w.show();
	return a.exec();
}
