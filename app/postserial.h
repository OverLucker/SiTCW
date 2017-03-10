#pragma once


// Now it's the main class of application layer

#ifndef POST_SERIAL
#define POST_SERIAL

#include <QSerialPort>

#include "message.h"
#include "directed_token_ring.h"


class PostSerial : public DirectedTokenRing {
	Q_OBJECT

private slots:
	void readHandle(QByteArray);


public:

	PostSerial(QObject * parent) : DirectedTokenRing(parent) {
		connect(this, &DirectedTokenRing::new_message, this, &PostSerial::readHandle);
	}
	
<<<<<<< HEAD
    int send_message(Message message);
=======
	int send_message(Message& message);
>>>>>>> 7640863a3a731e54788944cd573cf613ae831d3c
	int send_user_login(const QString& username);
	int send_user_logout(const QString& username);
	int notify_message_read(Message& message);


signals:
<<<<<<< HEAD
    void new_message(Message message);
	void message_read(Message& message);
	void message_delivered(Message& message);
=======
	void new_message(Message message);
	void message_read(Message message);
	void message_delivered(Message message);
>>>>>>> 7640863a3a731e54788944cd573cf613ae831d3c
	void user_status(QString username, UserStatus status);
};

#endif // !POST_SERIAL
