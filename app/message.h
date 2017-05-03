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
	QString id;
public:

	Message(const QString& from, const QString& to, const QString& message, const bool read, QString id = "0") {
		this->read = read;
		this->from = from;
		this->to = to;
		this->message = message;
		this->id = id;
	}

	Message(const QString& from, const QString& to, const QString& message) {
		this->read = false;
		this->from = from;
		this->to = to;
		this->message = message;
	}

	Message() {
		this->read = false;
		this->from = "";
		this->to = "";
		this->message = "";
	}
	// Отправитель может быть опущен
	Message(const QString& to, const QString& message) {
		this->read = false;
		this->from = "";
		this->to = to;
		this->message = message;
	}

	void setMyID(QString ID) {
		this->id = ID;
	}

	// getters
	bool is_read() { return this->read; }
	QString getSender() { return this->from; }
	QString getRecepient() { return this->to; }
	QString getMessage() { return this->message; }
	QString getId() { return this->id; }

};


enum class UserStatus {
	Inactive,
	Active
};


#endif // !APP_LAYER_MESSAGE
