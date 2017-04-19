#pragma once


// Now it's the main class of application layer

#ifndef POST_SERIAL
#define POST_SERIAL

#include <QSerialPort>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "message.h"
#include "directed_token_ring.h"


class PostSerial : public DirectedTokenRing {
	Q_OBJECT

	QSqlDatabase db;
	QString logged_username;

private slots:
	void readHandle(QByteArray);

public:

	PostSerial(QObject * parent) : DirectedTokenRing(parent) {
		connect(this, &DirectedTokenRing::new_message, this, &PostSerial::readHandle);

		// Database setup
		db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName("./db.sqlite3");
		db.open();

		QSqlQuery query;
		//query.exec("CREATE DATABASE IF NOT EXISTS postclient;");
		query.exec("CREATE TABLE IF NOT EXISTS users( userid INTEGER PRIMARY KEY AUTOINCREMENT,\
													  username varchar(40) NOT NULL UNIQUE,\
													  password varchar(40) NOT NULL );");
		query.exec("CREATE TABLE IF NOT EXISTS postbox( message_id INTEGER PRIMARY KEY AUTOINCREMENT,\
														sender INTEGER NOT NULL,\
														recipient INTEGER NOT NULL,\
														message TEXT NOT NULL,\
														status INTEGER NOT NULL );");

	}
	
	QString get_current_logged_user();
	int send_message(Message message);
	int notify_message_read(Message& message);

public slots:
	bool login(QString username, QString pass);
	void logout(QString username);

signals:
	void new_message(Message message);
	void message_read(Message message);
	void message_delivered(Message message);
	void user_status(QString username, UserStatus status);
};

#endif // !POST_SERIAL
