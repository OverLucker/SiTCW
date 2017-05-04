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
		QString external_id = chunks[3].split('=')[1];

		if (external_id == "-1") {
			//"open convert"
			Q_ASSERT(query.exec(QString("UPDATE postbox SET status = 1 WHERE message_id = \'%1\';").arg(message)));
			Message mess(from, to, message);
			emit message_read(mess);
			return;
		}
		
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

		query.exec(QString("INSERT INTO postbox(sender, recipient, message, status, external_id) VALUES (%1, %2, '%3', %4, %5);").arg(from_id, to_id, message, QString::number(0), external_id));
		QString message_id = query.lastInsertId().toString();
		Message mess(from, to, message, false, external_id);
		emit PostSerial::new_message(mess);
	}
	catch (...)
	{
	}
}


int PostSerial::send_message(Message& mess) {
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
	mess.setMyID(_id);

	QString data = QString("FROM=%1;TO=%2;MESSAGE=%3;ID=%4").arg(logged_username, mess.getRecepient(), mess.getMessage(), _id);
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

	Q_ASSERT(query.exec(QString("SELECT p.message_id, u.username, p.message, p.status FROM postbox as p \
								 JOIN users AS u ON u.userid = p.sender \
								 WHERE p.recipient = \'%1\';").arg(to)));
	while (query.next()) {
		QString id = query.value(0).toString();
		QString from = query.value(1).toString();
		QString message = query.value(2).toString();
		bool status = query.value(3).toInt();
		postboxMessages.push_back(Message(from, to, message, status, id));
	}
	return postboxMessages;
}

QVector <Message> PostSerial::getOutcomingPostbox() {
	QSqlQuery query;
	QString from = get_user_id( get_current_logged_user());
	QVector <Message> postboxMessages;
	Q_ASSERT(query.exec(QString("SELECT p.message_id, u.username, p.message, p.status FROM postbox as p \
								 JOIN users AS u ON u.userid = p.recipient \
								 WHERE p.sender = \'%1\';").arg(from)));

	while (query.next()) {
		QString id = query.value(0).toString();
		QString to = query.value(1).toString();
		QString message = query.value(2).toString();
		bool status = query.value(3).toInt();
		postboxMessages.push_back(Message(from, to, message, status, id));
	}
	return postboxMessages;
}

QString PostSerial::get_user_id(QString username) {
	QSqlQuery query;
	QString id;
	Q_ASSERT(query.exec(QString("SELECT userid FROM users WHERE username = \'%1\';").arg(username)));
	if (query.first()) {
		id = query.value(0).toString();
	}
	return id;
}

//¬€«¬¿“‹ ¬À¿ƒ” ƒÀﬂ Œ“ –€“»ﬂ  ŒÕ¬≈–“¿
int PostSerial::notify_message_read(QString external_id) {
	QSqlQuery query;
	Q_ASSERT(query.exec(QString("SELECT u.username, u2.username FROM postbox as p \
								JOIN users AS u ON u.userid = p.sender \
								JOIN users as u2 ON u2.userid = p.recipient\
								WHERE p.external_id = \'%1\';").arg(external_id)));
	QString from, to;
	if (query.first()) {
		to = query.value(0).toString();
		from = query.value(1).toString();
	}

	QString data = QString("FROM=%1;TO=%2;MESSAGE=%3;ID=%4").arg(from, to, external_id, QString("-1"));
	QVector<QString> recs;
	recs.append(to);
	this->send(data.toStdString().c_str(), recs);

	return 0;

}