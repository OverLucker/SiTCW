#pragma once


#ifndef DIRECTED_TOKEN_RING
#define DIRECTED_TOKEN_RING

#define MAX_DATA_SIZE 256 // in bytes
#define START_BYTE 0x7E
#define STOP_BYTE 0x7F


#include <QDataStream>
#include <QtSerialPort\QSerialPort>
#include <QMap>
#include "codec.h"
#include <QVector>



class DirectedTokenRing : public QObject {
	Q_OBJECT

private:
	QSerialPort* in = 0;
	QSerialPort* out = 0;
	bool ready = false;
	Codec* codec = 0;

	QVector<QByteArray> buffer_in;
	QVector<QByteArray> buffer_out;

	QMap<quint8, QString> pa; // physical addresses

	int openPorts() {
		if (!this->in->open(QIODevice::ReadOnly)) {
			// Handle error
			emit errorOccured(DirectedTokenRingError::OpenError);
			return 0;
		}
		if (!this->out->open(QIODevice::WriteOnly)) {
			// Handle error
			emit errorOccured(DirectedTokenRingError::OpenError);
			return 0;
		}
		return 1;
	}

	void createPhysicalAddress() {
		pa.insert(1, "Vladislav");
		pa.insert(2, "Nikita");
		pa.insert(3, "Anton");
	}





public:
	DirectedTokenRing(QObject* parent);

	enum class DirectedTokenRingError {
		NoError,
		OpenError,
		ConnectionClosed
	};

	int network_connect(const QString& port_in, const QString& port_out);
	int network_disconnect();

	void setCodec(Codec* codec) { this->codec = codec; }

	void send(QByteArray data);


private slots:
    void qserialreadHandler();

signals:
	void connectionOpen();
	void connectionClosed();
	void new_message(QByteArray);
	void frame_ready();
	void errorOccured(DirectedTokenRingError);
};


#endif // !DIRECTED_TOKEN_RING


