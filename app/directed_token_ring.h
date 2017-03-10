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
	QSerialPort* in;
	QSerialPort* out;
	bool ready = false;
	Codec* codec;

	QVector<QByteArray> buffer_in;
	QVector<QByteArray> buffer_out;

	QMap<quint8, QString> pa; // physical addresses

	void openPorts() {
		if (!this->in->open(QIODevice::ReadOnly)) {
			// Handle error
		}
		if (!this->out->open(QIODevice::WriteOnly)) {
			// Handle error
		}
	}

	void createPhysicalAddress() {
		pa[1] = "Vladislav";
		pa[2] = "Alexander";
		pa[3] = "Anton";
	}

private slots:
	void qserialreadHandler();


public:
	DirectedTokenRing(QObject * parent) : QObject(parent) {
		connect(this->out, &QSerialPort::readyRead, this, &DirectedTokenRing::qserialreadHandler);
		createPhysicalAddress();
	}

	int network_connect(const QString& port_in, const QString& port_out) {
		if (!this->in && !this->out){
			this->in = new QSerialPort(port_in, this);
			this->out = new QSerialPort(port_out, this);
			emit DirectedTokenRing::connectionOpen();
			return 0;
		}
		return 1;
	}

	int network_disconnect() {
		this->in = 0;
		this->out = 0;
		emit connectionClosed();
		return 0;
	}

	void setCodec(Codec* codec) { this->codec = codec; }

	void send(QByteArray data);


signals:
	void connectionOpen();
	void connectionClosed();
	void new_message(QByteArray);
	void frame_ready();
};


#endif // !DIRECTED_TOKEN_RING


