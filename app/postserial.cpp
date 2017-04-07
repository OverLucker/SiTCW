#include "postserial.h"


void PostSerial::readHandle(QByteArray data) {
	if (data.isEmpty()) return;
	// parse what data came etc
	// depending on data came we should decide which signal to emit
	try {
		QStringList chunks = QString(data).split(';');
		if (chunks.length() < 3) return;
		QString from = chunks[0].split('=')[1];
		QString to = chunks[1].split('=')[1];
		QString message = chunks[2].split('=')[1];
		Message mess(from, to, message);
	emit PostSerial::new_message(mess);
	}
	catch (...)
	{
	}
}


int PostSerial::send_message(Message mess) {
	QString data = QString("FROM=%1;TO=%2;MESSAGE=%3").arg(mess.getSender(), mess.getRecepient(), mess.getMessage());
	//QString data = QString("1");
	this->send(data.toStdString().c_str());
	return true;
}
