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
		connect(in, &QSerialPort::dataTerminalReadyChanged, this, &LowLevelClient::qserialDTRChanged);
		connect(out, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(errorHandler(QSerialPort::SerialPortError)));
		connect(out, &QSerialPort::readyRead, this, &LowLevelClient::qserialreadHandler);

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
		this->out->setDataTerminalReady(true);
		this->in->setDataTerminalReady(true);
		setConnectionState(ConnectionState::Connected);
		emit LowLevelClient::connectionOpen();
		return 1;
	}
	return 0;
}

int LowLevelClient::network_disconnect() {
	if (!this->in && !this->out)
		return 1;

	if (this->in) {
		if (this->in->isDataTerminalReady())
			this->in->setDataTerminalReady(false);
		this->in->disconnect();
		this->in->close();
	}
	if (this->out) {
		if (this->out->isDataTerminalReady())
			this->out->setDataTerminalReady(false);
		this->out->disconnect();
		this->out->close();
	}
	this->in = 0;
	this->out = 0;

	setConnectionState(ConnectionState::Connected);
	// ToDo: Отправка кадра о закрытии соединения
	// emit connectionClosed();
	return 0;
}

void LowLevelClient::qserialreadHandler() {
	QByteArray& data = this->buffer;
	char c;
	while (conn_state == ConnectionState::Connected && this->in->bytesAvailable() > 0) {
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
				// decode with codec
				if (this->codec)
					data = codec->decode(data);
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
	}
}

void LowLevelClient::send(QByteArray data) {
	if (this->codec)
		data = codec->encode(data);
	
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

	if (this->conn_state == ConnectionState::Connected)
		this->out->write(new_data);
}

void LowLevelClient::errorHandler(QSerialPort::SerialPortError error) {
	if (error == QSerialPort::SerialPortError::NoError)
		return;

	if (conn_state != ConnectionState::Disconnected) {
		setConnectionState(ConnectionState::Disconnected);
		emit errorOccured(LowLevelClientError::ConnectionClosed);
	}
}

void LowLevelClient::qserialDTRChanged(bool set) {
	if (set) {
		setConnectionState(ConnectionState::Connected);
		emit connectionOpen();
	}
	else {
		setConnectionState(ConnectionState::Disconnected);
		emit errorOccured(LowLevelClientError::ConnectionClosed);
	}
}

void LowLevelClient::setConnectionState(LowLevelClient::ConnectionState state) {
	if (this->conn_state != state) {
		this->conn_state = state;
	}
}

LowLevelClient::ConnectionState LowLevelClient::getConnectionState() { 
	return conn_state; 
}
