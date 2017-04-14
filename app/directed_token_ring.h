#pragma once


#ifndef DIRECTED_TOKEN_RING
#define DIRECTED_TOKEN_RING


#include <QMap>
#include "codec.h"
#include <QVector>
#include "lowlevelclient.h"

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
	quint8 app_address = 0;
	quint8 second_round = 0;

	// состояния каждого клиента 
	//  1. не подключен
	//  2. подключен, но не готов к передаче
	//  3. подключен и готов к передаче
	//

	enum class ClientState {
		NotConnected,
		Connected,
		Ready
	};

	ClientState client_state = ClientState::NotConnected;

	// буферы
	//  1. буфер входящих кадров
	//  2. буфер исходящих кадров
	//  3. буфер известных физических адресов и ящиков (логическая связь)
	//

	QVector<QByteArray> buffer_in;
	QVector<QByteArray> buffer_out;
	QMap<quint8, QString> pa;

	// кодеки для шифровки и расшифровки сообщений
	Codec * codec = 0;


	void send_frame(QByteArray data);
public:
	DirectedTokenRing(QObject* parent);


	void setCodec(Codec* codec) { this->codec = codec; }

	void send(QByteArray data);
	void send_user_login(const QString& username);
	void send_user_logout(const QString& username);
	QList<QString> get_contacts();



private slots:
	void onNetworkConnectionOpen();
	void frameReadyHandler(QByteArray);
	void ringErrorHandler(LowLevelClientError);


signals:
	void ClientStateChanged(ClientState new_state);
	void new_message(QByteArray);
	void userLoggedIn(QString username);
	void userLoggedOut(QString username);
	
	
};




#endif // !DIRECTED_TOKEN_RING
