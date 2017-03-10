#include "postserial.h"


void PostSerial::readHandle(QByteArray data) {

	// parse what data came etc
	// depending on data came we should decide which signal to emit
<<<<<<< HEAD
	emit this->readyRead();
}


int PostSerial::send_message(Message message){
    //

}
=======
	QStringList chunks = QString(data).split(';');
	std::string from = chunks[0].split('=')[1].toStdString();
	std::string to = chunks[1].split('=')[1].toStdString();
	std::string message = chunks[2].split('=')[1].toStdString();
	Message mess(from, to, message);
	emit new_message(mess);
}


int PostSerial::send_message(Message& mess) {
	QString data = QString("FROM=%1;TO=%2;MESSAGE=%3").arg(mess.getSender().c_str(), mess.getRecepient().c_str(), mess.getMessage().c_str());
	this->send(data.toStdString().c_str());
	return true;
}
>>>>>>> 7640863a3a731e54788944cd573cf613ae831d3c
