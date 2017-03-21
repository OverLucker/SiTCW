#include "directed_token_ring.h"
#//include <QMessageBox>

DirectedTokenRing::DirectedTokenRing(QObject* parent) : QObject(parent) {

	createPhysicalAddress();
}

int DirectedTokenRing::network_connect(const QString& port_in, const QString& port_out) {
	if (!this->in && !this->out) {
		this->in = new QSerialPort(port_in, this);
		this->out = new QSerialPort(port_out, this);

		connect(in, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(ringErrorHandler(QSerialPort::SerialPortError)));
		connect(out, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(ringErrorHandler(QSerialPort::SerialPortError)));
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
	if (data.length() > MAX_DATA_SIZE) {
	}

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

	// assuming it working
	while (data.length() > 0) {

		// Пытаемся найти начало кадра
		// Кадр полный и есть начало кадра
		int st_byte_pos = data.indexOf(START_BYTE);
		if (full_frame_in && st_byte_pos >= 0) {
			// записываем как есть
			int stop_byte_pos = data.indexOf(STOP_BYTE, st_byte_pos);
			buffer_in.append(data.mid(st_byte_pos + 1, stop_byte_pos - st_byte_pos));
			data = data.right(data.length() - stop_byte_pos - 1);
			// Нет конца кадра, но было начало
			if (stop_byte_pos < 0)
				full_frame_in = false;
			continue;
		}

		int stop_byte_pos = data.indexOf(STOP_BYTE);
		if (!full_frame_in && stop_byte_pos >= 0) {
			buffer_in.last().append(data.left(stop_byte_pos));
			data = data.right(data.length() - stop_byte_pos - 1);
			full_frame_in = true;
			continue;
		}

		data.clear();
	}


	if (!full_frame_in)
		return;

	// decode each frame
	if (this->codec)
		data = this->codec->decode(data);

	// merge all frames
	data.clear();
	for (auto i : buffer_in)
		data.append(i);

	buffer_in.clear();

	// handle inc message

	emit DirectedTokenRing::new_message(data);
	
}

QList<QString> DirectedTokenRing::get_contacts() {
	return pa.values();
}

void DirectedTokenRing::ringErrorHandler(QSerialPort::SerialPortError error) {
	if (error == QSerialPort::SerialPortError::ResourceError) {
		//TODO: After release special frame, send it to "out"
		DirectedTokenRing::network_disconnect();
	}
	//example for debug
	/*QMessageBox msgBox;
	msgBox.setWindowTitle("example");
	msgBox.setText(QString::number((int)error));
	msgBox.exec();
	*/
	//QMessageBox::information(thQMainWindow sd;is, "Connection status", (int)error); 
	//ui.textBrowser->append(err);
}