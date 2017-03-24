#pragma once


#ifndef DIRECTED_TOKEN_RING
#define DIRECTED_TOKEN_RING


#include <QDataStream>

#include <QMap>
#include "codec.h"
#include <QVector>
#include "wrappers\wrapper.h"
#include "lowlevelclient.h"

class DirectedTokenRing : public LowLevelClient {
	Q_OBJECT
private:
	bool ready = false;
	
	Codec* codec = 0;

	QVector<QByteArray> buffer_in;
	
	QVector<QByteArray> buffer_out;

	QMap<quint8, QString> pa; // physical addresses

	void createPhysicalAddress() {
		// ToDo: динамическое обновление адресов
		pa.insert(1, "Vladislav");
		pa.insert(2, "Nikita");
		pa.insert(3, "Anton");
	}


public:
	DirectedTokenRing(QObject* parent);

	void setCodec(Codec* codec) { this->codec = codec; }

	void send(QByteArray data);
	QList<QString> get_contacts();


private slots:
	void frameReadyHandler(QByteArray);
	void ringErrorHandler(LowLevelClientError);


signals:
	
	void new_message(QByteArray);
	
	
};


#endif // !DIRECTED_TOKEN_RING
