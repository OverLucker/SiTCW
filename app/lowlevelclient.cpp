#include "lowlevelclient.h"


LowLevelClient::LowLevelClient(QObject*parent) : QObject(parent) {

}

int LowLevelClient::network_connect(const QString& port_in, const QString& port_out) {
	if (!this->in && !this->out) {
		this->in = new QSerialPort(port_in, this);
		this->out = new QSerialPort(port_out, this);

		// Here you should add connectors if you want to
		connect(in, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(errorHandler(QSerialPort::SerialPortError)));
		connect(in, &QSerialPort::readyRead, this, &LowLevelClient::qserialreadHandler);
		connect(out, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(errorHandler(QSerialPort::SerialPortError)));

		// openning ports
		if (!this->in->open(QIODevice::ReadOnly)) {
			// Clear everything to be accessable again
			delete this->in;
			delete this->out;
			this->in = 0;
			this->out = 0;
			emit errorOccured(LowLevelClientError::OpenError);
			return 0;
		}
		if (!this->out->open(QIODevice::WriteOnly)) {
			// Clear everything to be accessable again
			delete this->in;
			delete this->out;
			this->in = 0;
			this->out = 0;
			emit errorOccured(LowLevelClientError::OpenError);
			return 0;
		}
		emit LowLevelClient::connectionOpen();
		return 1;
	}
	return 0;
}

int LowLevelClient::network_disconnect() {
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

	// ToDo: Отправка кадра о закрытии соединения
	emit connectionClosed();
	return 0;
}

void LowLevelClient::qserialreadHandler() {
	QByteArray& data = this->buffer;
	char c;
	while (true) {
		// case 1 : there is data but we didnt had a data START_BYTE
		while (!this->data_started && this->in->getChar(&c)) {
			if (c == START_BYTE)
				this->data_started = true;
		}

		// case 2 : there is data, we have started a data stream and haven't finished yet
		while (this->data_started && this->in->getChar(&c)) {
			if (this->esc_char) {
				this->esc_char = false;
				data.append(c);
				continue;
			}
			if (c == STOP_BYTE) {
				this->data_started = false;
				// notify everyone that data is read
				emit frame_ready(data);
				// clear the buffer
				data.clear();
				break;
			}
			if (c == ESCAPE_BYTE) {
				this->esc_char = true;
				continue;
			}

			data.append(c);	
		}
		if (this->in->bytesAvailable() <= 0)
			break;
	}
}


void LowLevelClient::send(const QByteArray& data) {
	QByteArray new_data;

	// стартовый байт начала данных
	new_data.append(START_BYTE);

	// экранирование символов
	for (int i = 0, end = data.length(); i < end; ++i) {
		char sym = data[i];
		if (sym == START_BYTE || sym == STOP_BYTE || sym == ESCAPE_BYTE)
			new_data.append(ESCAPE_BYTE);
		new_data.append(sym);
	}

	// стоповый байт
	new_data.append(STOP_BYTE);

	this->out->write(new_data);
}

void LowLevelClient::errorHandler(QSerialPort::SerialPortError error) {
	if (error == QSerialPort::SerialPortError::ResourceError) {
		emit errorOccured(LowLevelClientError::ConnectionClosed);
	}
}
