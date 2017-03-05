#include "postserial.h"

void PostSerial::readHandle() {
	QByteArray data = this->readAll();


	// parse what data came etc
	// depending on data came we should decide which signal to emit
	emit this->readyRead();
}