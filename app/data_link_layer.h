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
	Meeting,
	Disconnect,
	Login,
	Logout
};



#define FRAME_HEADER_SIZE 12
#define MAX_DATA_SIZE 256 // in bytes
#define MAX_FRAME_SIZE FRAME_HEADER_SIZE + MAX_DATA_SIZE
#define FRAME_SECURE_BYTE 0x7E

enum class FrameType {
	SuperVisor = 1,
	Information = 2,
	Token = 3,
};

struct Frame {
	FrameType frame_type;
	bool last_frame;
	quint8 frame_header_size;
	quint8 frame_secure;
	quint8 sender;
	quint8 recipients_number;
	quint8 recipients[14];
	quint8 data_size;
	char data[MAX_DATA_SIZE];
};

namespace Handlers {
	class FrameHandler {
		public:
			virtual bool handle(const Frame& frame) = 0;

		operator QByteArray()const {

		}
	};

	class SuperVisorHandler {

	};

	class InformationHandler {

	};

	class TokenHandler {

	};
}








/*
typedef unsigned char uchar; 

const int TOKEN_MAX_TIMEOUT = 15;







class Frame {
	// This class is immutable. It just contains gathered info

private:
	

	

	// validators
	bool validate_frame_type(quint8 val){ return val > 0 && val < 4; }

public:
	Frame(
		FrameType frame_type,
		bool last_frame,
		quint8 frame_header_size,
		bool frame_secure,
		quint8 sender,
		std::vector<quint8> recipients,
		quint8 data_size
		// ToDo: need to add data
	) {
		this->frame_type = frame_type;
		this->last_frame = last_frame;
		this->frame_header_size = frame_header_size;
		this->frame_secure = frame_secure;
		this->sender = sender;
		this->recipients = recipients;
		this->data_size = data_size;
		// ToDo: need to add data
	}
	
	static const std::vector<Frame>& Construct(quint8 * data, int length) {
	
	}

	static const std::vector<Frame>& Construct(QDataStream& data) {
		// one Frame Reading
		quint8 tmp;

		// getting 1 byte
		data >> tmp;
		bool last_frame = tmp & 0x20;
		quint8 frame_type = tmp && 0xC0;
		quint8 frame_header_size = tmp && 0x1F;
		
		// getting 2 byte
		data >> tmp;
		quint8 frame_number = tmp;

		// getting 3 byte
		data >> tmp;
		bool frame_secure = tmp == 0xBD;
		if (!frame_secure)
			// return an empty vector
			return std::vector<Frame>();

		// getting 4 byte
		data >> tmp;
		quint8 sender = tmp & 0xF0;
		quint8 recipient_number = tmp & 0x0F;
		quint8 recipient_bytes = recipient_number >> 1;

		// checking
		// static bytes count = 5
		if (frame_header_size != recipient_bytes + 5)
			// return an empty vector
			return std::vector<Frame>();

		// getting recipients
		std::vector<quint8> recipients;
		
		for (int i = 0; i < recipient_bytes; ++i)
		{
			data >> tmp;
			if (tmp & 0xF0)
				recipients.push_back(tmp & 0xF0);

			if (tmp & 0x0F)
				recipients.push_back(tmp & 0x0F);
		}

		// getting data_size
		quint8 data_size;
		data >> data_size;

		// ToDo: need to read data
		char d[256];
		data.readRawData(d, data_size);

	}


	// getters
	FrameType getFrameType() { return this->frame_type; }
	bool isLastFrame() { return this->last_frame; }
	quint8 const getFrameHeaderSize() { return this->frame_header_size; }
	quint8 const getSender() { return this->sender; }
	std::vector<quint8> const getRecipients() { return this->recipients; }
	quint8 const getDataSize() { return this->data_size; }
	char * getData() { return this->data; }

};







//QMap<FrameType, handler> handlers;

void supervisor_handler(char * data) {

}

void information_handler(char * data) {

}

void token_handler(char * data) {

}

void ringdestruction_handler(char * data) {

}
	











char * frame_maker(char * data) {
	return NULL;
}

void dl_api_send() {

}



int frame_checker(char * data) {
	// extract frame info (type, last frame, size of frame)

	// extract frame number

	// extract sender
	// extract recipient_count
	// extract recipients
	// extract user_data_count
	// extract user_data
	return NULL;
}

// dl_api_send
// dl_api_get(void * callback)

*/

#endif // !DATA_LINK_LAYER
