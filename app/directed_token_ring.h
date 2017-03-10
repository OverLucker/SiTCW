#pragma once


#ifndef DIRECTED_TOKEN_RING
#define DIRECTED_TOKEN_RING

#include <QDataStream>
#include <QtSerialPort\QSerialPort>
#include <QMap>
#include "codec.h"


class DirectedTokenRing : public QObject {
	Q_OBJECT

private:
	QSerialPort* in;
	QSerialPort* out;
	bool ready = false;
	Codec* codec;

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

	}

private slots:
	void readyRead();
	void qserialreadHandler();

public:
	DirectedTokenRing(QObject * parent) : QObject(parent) {
		connect(this->out, &QSerialPort::readyRead, this, &DirectedTokenRing::qserialreadHandler);
	}

	DirectedTokenRing(QSerialPort * in, QSerialPort * out, QObject* parent) : QObject(parent) {
		this->in = in;
		this->out = out;
		connect(this->out, &QSerialPort::readyRead, this, &DirectedTokenRing::qserialreadHandler);
	}

	DirectedTokenRing(const QString& port_in, const QString& port_out, QObject* parent) : QObject(parent) {
		this->in = new QSerialPort(port_in, this);
		this->out = new QSerialPort(port_out, this);
		connect(this->out, &QSerialPort::readyRead, this, &DirectedTokenRing::qserialreadHandler);
	}

	void setCodec(Codec* codec) { this->codec = codec; }

	void send(const QByteArray& data);


signals:
	void connectionOpen();
	void connectionClosed();
	void readyRead();
};


#endif // !DIRECTED_TOKEN_RING


