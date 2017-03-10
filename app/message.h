#pragma once


#ifndef APP_LAYER_MESSAGE
#define APP_LAYER_MESSAGE

#include <QObject>

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
	Inactive,
	Active
};


#endif // !APP_LAYER_MESSAGE
