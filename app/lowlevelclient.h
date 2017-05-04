#pragma once


#ifndef LOW_LEVEL_CLIENT
#define LOW_LEVEL_CLIENT

#include <QObject>
#include <QtSerialPort\QSerialPort>
#include "codec.h"


#define START_BYTE 0x7E
#define STOP_BYTE 0x7F
#define ESCAPE_BYTE '\\'


// Операции обертки в старт-стоповые байты,
//  реализация интерфейса нижнего уровня
class LowLevelClient : public QObject {
	Q_OBJECT

private:
	QSerialPort* in = 0;
	QSerialPort* out = 0;
	
	bool data_started = false;
	bool esc_char = false;
	QByteArray buffer;

	// кодеки для шифровки и расшифровки сообщений
	Codec * codec = 0;
		
public:
	enum class LowLevelClientError {
		NoError,
		OpenError,
		ConnectionClosed
	};

	enum class ConnectionState {
		Disconnected,
		Connected
	};

private:
	ConnectionState conn_state = ConnectionState::Disconnected;
	void setConnectionState(ConnectionState state);

public:
	LowLevelClient(QObject* parent = nullptr);
	int network_connect(const QString& port_in, const QString& port_out);
	int network_disconnect();
	void send(QByteArray data);
	ConnectionState getConnectionState();

	void setCodec(Codec* codec) { this->codec = codec; }

private slots:
	void qserialreadHandler();
	void qserialDTRChanged(bool);
	void errorHandler(QSerialPort::SerialPortError);

signals:
	void connectionOpen();
	void connectionClosed();
	void errorOccured(LowLevelClientError);
	void frame_ready(QByteArray data);
};


#endif // !LOW_LEVEL_CLIENT
