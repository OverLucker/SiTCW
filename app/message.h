#pragma once


#ifndef APP_LAYER_MESSAGE
#define APP_LAYER_MESSAGE
#include <QString>

class Message {
protected:
	bool read;
	QString from;
	QString to;
	QString message;

public:
	Message(const QString& from, const QString& to, const QString& message) {
		this->read = false;
		this->from = from;
		this->to = to;
		this->message = message;
	}

	// getters
	bool is_read() { return this->read; }
	QString getSender() { return this->from; }
	QString getRecepient() { return this->to; }
	QString getMessage() { return this->message; }
};


enum class UserStatus {
	Inactive,
	Active
};


#endif // !APP_LAYER_MESSAGE
