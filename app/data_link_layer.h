#pragma once

// В данном файле реализованы функции для работы с канальным уровнем
//  Данный файл является подключаемым, все взаимодействия определяются
//   вызовами функций api
// Наименование данных функций начинается с dl_api_


#ifndef DATA_LINK_LAYER
#define DATA_LINK_LAYER

#include "physical_layer.h"
#include <vector>
#include <QDataStream>
#include <QMap>


enum class SuperVisorFrameTypes {
	Meeting = 1,
	Disconnect = 2,
	Login = 3,
	Logout = 4
};



#define FRAME_HEADER_SIZE 12
#define MAX_DATA_SIZE 256 // in bytes
#define MAX_FRAME_SIZE FRAME_HEADER_SIZE + MAX_DATA_SIZE
#define FRAME_SECURE_BYTE 0x7E

enum class FrameType {
	Null = 0,
	SuperVisor = 1,
	Information = 2,
	Token = 3,
};

class Frame {
private:
	FrameType frame_type;
	quint8 last_frame;
	quint8 frame_header_size;
	quint8 frame_secure;
	quint8 sender;
	quint8 recipients_number;
	QVector<quint8> recipients;
	quint8 data_size;
	QByteArray data;

	const int address_size = 4; // in bits
	const int pure_header_size = 4; // in bytes

public:

	// ToDo: forgot about frame number
	Frame(FrameType frame_type,
		  bool last_frame,
		  const quint8& sender, 
		  const QVector<quint8>& recipients,
		  const QByteArray& data) {

		this->frame_type = frame_type;
		this->last_frame = last_frame ? 0xFF : 0x00;

		this->frame_secure = FRAME_SECURE_BYTE;

		this->sender = sender;
		this->recipients_number = recipients.length();

		this->recipients = recipients;
		this->data_size = data.length();
		this->data = data;

		this->frame_header_size = (recipients_number*address_size%8 ? 1 : 0) + (recipients_number * address_size / 8) + pure_header_size;
	}

	operator QByteArray() {
		QByteArray buffer;

		quint8 f = (quint8)frame_type << 6;
		quint8 byte = f | (last_frame & 0x20) | (frame_header_size & 0x1F);
		buffer.append(byte);

		buffer.append(frame_secure);

		buffer.append((sender << 4) | (recipients_number & 0x0F));
		for (quint8 i = 0; i < recipients_number; i += 2) {
			if (i + 1 < recipients_number)
				buffer.append((recipients[i] << 4) | (recipients[i + 1] & 0x0F));
			else
				buffer.append(recipients[i] << 4);
		}

		buffer.append(data_size);

		buffer.append(data);
		return buffer;
	}

	FrameType getFrameType() { return this->frame_type; }
	bool isLastFrame() { return this->last_frame; }
	quint8 const getFrameHeaderSize() { return this->frame_header_size; }
	quint8 const getSender() { return this->sender; }
	QVector<quint8> const getRecipients() { return this->recipients; }
	QByteArray getData() { return this->data; }
};


class FrameBuilder {

public:
	static Frame makeInformationFrame(const quint8& sender, const QVector<quint8>& recipients, const QByteArray& data) {
		// ToDo: split to frames
		return Frame (
			FrameType::Information,
			true,
			sender,
			recipients,
			data
		);
	}

	static Frame makeSupervisorFrame(const quint8& sender, const QByteArray& data) {
		// ToDo: split to frames
		QVector<quint8> recipients;
		for (int i = 1; i < 16; ++i) {
			recipients.append(i);
		}

		return Frame(
			FrameType::SuperVisor,
			true,
			sender,
			recipients,
			data
		);
	}

	static Frame makeNullFrame() {
		return Frame(
			FrameType::Null,
			true,
			0,
			QVector<quint8>(),
			QByteArray()
		);
	}

	static Frame fromByteArray(const QByteArray& data) {
		if (data.length() <= FRAME_HEADER_SIZE)
			return makeNullFrame();

		if (data[1] != FRAME_SECURE_BYTE)
			return makeNullFrame();

		QVector<quint8> recipients;
		quint8 rec_count = (data[2] & 0x0F);
		for (quint8 i = 0, all = rec_count / 2; i < all; ++i) {
			if ((data[i + 3] & 0xF0 >> 4) > 0)
				recipients.append((data[i + 3] & 0xF0) >> 4);
			if((data[i + 3] & 0x0F) > 0)
				recipients.append((data[i + 3] & 0x0F));
		}
		if (rec_count % 2)
			recipients.append(data[rec_count / 2 + 3] & 0xF0 >> 4);

		return Frame(
			FrameType((quint8)data[0] >> 6), // frame_type
			data[0] & 0x20, // last_frame
			(data[2] & 0xF0) >> 4, // sender
			recipients,
			data.mid(data[0] & 0x1F)
		);
	}

};

#endif
