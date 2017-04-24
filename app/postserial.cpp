#include "postserial.h"
#include <QtSql/QSql>

#include <QVariant>


void PostSerial::readHandle(QByteArray data) {
	if (data.isEmpty()) return;
	QSqlQuery query;
	// parse what data came etc
	// depending on data came we should decide which signal to emit
	try {
		QStringList chunks = QString(data).split(';');
		if (chunks.length() < 3) return;
		QString from = chunks[0].split('=')[1];
		QString to = chunks[1].split('=')[1];
		QString message = chunks[2].split('=')[1];
		Message mess(from, to, message);
		
		
		QString from_id;
		QString to_id;
		Q_ASSERT(query.exec(QString("SELECT * FROM users WHERE username = \'%1\';").arg(from)));
		if (query.first()) {
			from_id = query.value(0).toString();
		}
		
		Q_ASSERT(query.exec(QString("SELECT * FROM users WHERE username = \'%1\';").arg(to)));
		if (query.first()) {
			to_id = query.value(0).toString();
		}
		query.exec(QString("INSERT INTO postbox(sender, recipient, message, status) VALUES (%1, %2, '%3', %4);").arg(from_id, to_id, message, QString::number(0)));

		emit PostSerial::new_message(mess);
	}
	catch (...)
	{
	}
}


int PostSerial::send_message(Message mess) {
	if (logged_username == QString())
		return false;

	QSqlQuery query;
	QString from_id;
	QString to_id;
	Q_ASSERT(query.exec(QString("SELECT * FROM users WHERE username = \'%1\';").arg(logged_username)));
	if (query.first()) {
		from_id = query.value(0).toString();
	}

	Q_ASSERT(query.exec(QString("SELECT * FROM users WHERE username = \'%1\';").arg(mess.getRecepient())));
	if (query.first()) {
		to_id = query.value(0).toString();
	}
	query.exec(QString("INSERT INTO postbox(sender, recipient, message, status) VALUES (%1, %2, '%3', %4);").arg(from_id, to_id, mess.getMessage(), QString::number(0)));
	QString _id = query.lastInsertId().toString();

	QString data = QString("FROM=%1;TO=%2;MESSAGE=%3").arg(logged_username, mess.getRecepient(), mess.getMessage());
	QVector<QString> recs;
	recs.append(mess.getRecepient());
	this->send(data.toStdString().c_str(), recs);

	return true;
}

bool PostSerial::login(QString username, QString pass) {
    QSqlQuery query;
	Q_ASSERT(query.exec(QString("SELECT * FROM users WHERE username = \'%1\';").arg(username)));
	if (query.first()) {
		// there is a record
		QString _id = query.value(0).toString();
		QString username = query.value(1).toString();
		QString real_pass = query.value(2).toString();
		this->logged_username = username;
		if (pass != real_pass)
			return false;
	}
	else {
		// register him
		query.exec(QString("INSERT INTO users(username, password) VALUES ('%1', '%2');").arg(username, pass));
		this->logged_username = username;
	}
	this->send_user_login(username);
	return true;
}

void PostSerial::logout(QString username) {
	this->logged_username.clear();
	this->send_user_logout(username);
}

QString PostSerial::get_current_logged_user() {
	return this->logged_username;
}
void PostSerial::addressBookRemove(QString username) {
	emit userRemoved(username);
}
void PostSerial::addressBookAdd(QString username) {
	QSqlQuery query;
	Q_ASSERT(query.exec(QString("SELECT * FROM users WHERE username = \'%1\';").arg(username)));
	if (!query.first()) {
		query.exec(QString("INSERT INTO users(username) VALUES ('%1');").arg(username));
	}
	QString _id = query.value(0).toString();
	emit userAdded(username);
}

QVector <Message> PostSerial::getIncomingPostbox() {
	QSqlQuery query;
	QString to = get_user_id(get_current_logged_user());
	QVector <Message> postboxMessages;
	Q_ASSERT(query.exec(QString("SELECT sender, message, status  FROM postbox WHERE recipient = \'%1\';").arg(to)));

	while (query.next()) {
		QString from = query.value(0).toString();
		QString message = query.value(1).toString();
		bool status = query.value(2).toInt();
		postboxMessages.push_back(Message(from, to, message, status));
	}
	return postboxMessages;
}

QVector <Message> PostSerial::getOutcomingPostbox() {
	QSqlQuery query;
	QString from = get_user_id( get_current_logged_user());
	QVector <Message> postboxMessages;
	Q_ASSERT(query.exec(QString("SELECT recipient, message, status  FROM postbox WHERE sender = \'%1\';").arg(from)));

	while (query.next()) {
		QString to = query.value(0).toString();
		QString message = query.value(1).toString();
		bool status = query.value(2).toInt();
		postboxMessages.push_back(Message(from, to, message, status));
	}
	return postboxMessages;
}

QString PostSerial::get_user_id(QString username) {
	QSqlQuery query;
	QString id;
	Q_ASSERT(query.exec(QString("SELECT id FROM users WHERE username = \'%1\';").arg(username)));
	if (query.first()) {
		id = query.value(0).toString();
	}
	return id;
}

int PostSerial::notify_message_read(Message &message) {
	//нужно придумать Проапргейдить  Message, чтобы он хранил ещё и id
	return 0;

}