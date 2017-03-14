#include "directed_token_ring.h"

DirectedTokenRing::DirectedTokenRing(QObject* parent) : QObject(parent) {

	createPhysicalAddress();
}

int DirectedTokenRing::network_connect(const QString& port_in, const QString& port_out) {
	if (!this->in && !this->out) {
		this->in = new QSerialPort(port_in, this);
		this->out = new QSerialPort(port_out, this);
		connect(this->in, &QSerialPort::readyRead, this, &DirectedTokenRing::qserialreadHandler);
		int result = this->openPorts();
		if (result)
			emit DirectedTokenRing::connectionOpen();
		return result;
	}
	return 0;
}

int DirectedTokenRing::network_disconnect() {
	if (this->in) {
		this->in->disconnect();
		this->in->close();
	}
	if (this->out) {
		this->out->disconnect();
		this->out->close();
	}
	this->in = 0;
	this->out = 0;

	emit connectionClosed();
	return 0;
}

void DirectedTokenRing::send(QByteArray data) {
	// split to frames


	// encode each frame
	QByteArray encoded_data;
	if (this->codec)
		encoded_data = this->codec->encode(data);

	data.prepend(START_BYTE);
	data.append(STOP_BYTE);
	this->out->write(data);
}


void DirectedTokenRing::qserialreadHandler() {
	QByteArray data = this->in->readAll();
	// strip from start stop byte
	

	// decode each frame
	if (this->codec)
		data = this->codec->decode(data);


	//QByteArrayList frame_st = data.split(START_BYTE);


	// merge all frames

	// handle inc message

	emit DirectedTokenRing::new_message(data);
}