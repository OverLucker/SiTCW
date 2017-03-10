#pragma once


#ifndef POST_SERIAL
#define POST_SERIAL

#include <QSerialPort>
//#include <QtSerialPort\QSerialPort>
#include "message.h"


class PostSerial : public QSerialPort {
	Q_OBJECT

private slots:
	void readHandle();


public:
	PostSerial(QObject * parent) : QSerialPort(parent) { 
		connect(this, &QSerialPort::readyRead, this, &PostSerial::readHandle);
	}
	
    int send_message(Message message);
	int send_user_login(const QString& username);
	int send_user_logout(const QString& username);
	int notify_message_read(const Message& message);


signals:
    void new_message(Message message);
	void message_read(Message& message);
	void message_delivered(Message& message);
	void user_status(QString username, UserStatus status);
};

#endif // !POST_SERIAL
