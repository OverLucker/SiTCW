#pragma once


#ifndef DIRECTED_TOKEN_RING
#define DIRECTED_TOKEN_RING

#define LOG_OUT_DELAY 500

#include <QMap>
#include <QVector>
#include "lowlevelclient.h"
#include <QTimer>
#include "data_link_layer.h"

class DirectedTokenRing;

typedef void(DirectedTokenRing::*handler)(Frame&);

class DirectedTokenRing : public LowLevelClient {
	Q_OBJECT
private:

	// свойства каждого клиента
	//  1. адрес
	//  2. число для определения первенства
	//  3. первоначальное предположение адреса
	//

	quint8 local_address = 0;
	quint8 def_number = 0;
	QVector<quint8> known_numbers;
	QTimer* timer = new QTimer(this);

	// состояния каждого клиента
	//  1. не подключен
	//  2. подключен, но не готов к передаче
	//  3. подключен и готов к передаче
	//

	QMap<SuperVisorFrameTypes, handler> handlers;
	void handlerMeeting(Frame&);
	void handlerDisconnect(Frame&);
	void handlerLogin(Frame&);
	void handlerLogout(Frame&);

public:
	enum class ClientState {
		Offline,
		Online
	};

private:
	ClientState client_state = ClientState::Offline;

	// буферы
	//  1. буфер входящих кадров
	//  2. буфер исходящих кадров
	//  3. буфер известных физических адресов и ящиков (логическая связь)
	//

	QVector<QByteArray> buffer_in;
	QVector<QByteArray> buffer_out;
	QMap<quint8, QString> pa;



	QByteArray last_frame;

	void send_frame(QByteArray data);

protected:
	void setClientState(ClientState state) {
		if (client_state != state) {
			client_state = state;
			emit ClientStateChanged(client_state);
		}
	}

public:
	DirectedTokenRing(QObject* parent);



	void send(QByteArray data, QVector<QString> recipients);
	void send_user_login(const QString& username);
	void send_user_logout(const QString& username);
	// for test only
	int get_local_address() {
		return this->local_address;
	}
	int get_def_num() {
		return this->def_number;
	}
	QList<QString> get_contacts();



private slots:
	void onNetworkConnectionOpen();
	void frameReadyHandler(QByteArray);
	void ringErrorHandler(LowLevelClientError);
	void frameTimeout();


signals:
	void ClientStateChanged(ClientState new_state);
	void new_message(QByteArray);
	void userLoggedIn(QString username);
	void userLoggedOut(QString username);
	
	
};




#endif // !DIRECTED_TOKEN_RING
