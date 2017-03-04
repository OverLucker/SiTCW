#pragma once

// � ������ ����� ����������� ������� ��� ������ � ���������� �������
//  ������ ���� �������� ������������, ��� �������������� ������������
//   �������� ������� api
// ������������ ������ ������� ���������� � al_api_

#ifndef APP_LAYER
#define APP_LAYER

#include "data_link_layer.h"


class Message {
protected:
	bool read;
	std::string from;
	std::string to;
	std::string message;

public:
	Message(std::string from, std::string to, std::string message) {
		this->read = false;
		this->from = from;
		this->to = to;
		this->message = message;
	}

	// getters
	bool is_read() { return this->read; }
	std::string getSender() { return this->from; }
	std::string getRecepient() { return this->to; }
	std::string getMessage() { return this->message; }
};

enum class UserStatus {
	InActive,
	Active
};

// ��������� ��� ��������
class OutMessage : public Message {

};


// ��������� ������
class InMessage : public Message {
public:
	bool make_read() {
		this->read = true;
		return true;
	}

};



void new_message(Message& message) {

}
//
//
//


#endif // !APP_LAYER
