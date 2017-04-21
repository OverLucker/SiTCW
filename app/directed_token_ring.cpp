#include "directed_token_ring.h"
#include "data_link_layer.h"
#include <QMessageBox>
#include <algorithm>
#include <time.h>
#include <QTimer>


#define LOG_OUT_DELAY 500
#define SINGLE_FRAME_AWAIT_TIMER 1000


DirectedTokenRing::DirectedTokenRing(QObject* parent) : LowLevelClient(parent) {
	connect(this, &LowLevelClient::frame_ready, this, &DirectedTokenRing::frameReadyHandler);
	connect(this, &LowLevelClient::connectionOpen, this, &DirectedTokenRing::onNetworkConnectionOpen);
	connect(this, &LowLevelClient::errorOccured, this, &DirectedTokenRing::ringErrorHandler);
}

QList<QString> DirectedTokenRing::get_contacts() {
	return pa.values();
}

void DirectedTokenRing::ringErrorHandler(LowLevelClientError error) {
	if (error == LowLevelClientError::ConnectionClosed) {
		QByteArray data;
		data.append((quint8)SuperVisorFrameTypes::Disconnect);
		send_frame(FrameBuilder::makeSupervisorFrame(local_address, data));
		QTimer::singleShot(LOG_OUT_DELAY, this, SLOT(network_disconnect()));
	}

}

void DirectedTokenRing::send_frame(QByteArray data) {
	// encode with codec
	if (this->codec)
		data = codec->encode(data);

	LowLevelClient::send(data);
}

void DirectedTokenRing::send(QByteArray data, QVector<QString> recipients) {
	QVector<quint8> rec;
	for (auto it : recipients)
		rec.append(pa.key(it));
	
	send_frame(FrameBuilder::makeInformationFrame(local_address, rec, data));
}

void DirectedTokenRing::send_user_logout(const QString& username) {
	QByteArray data;
	data.append((quint8)SuperVisorFrameTypes::Logout);
	data.append(local_address);
	data.append(username);
	send_frame(FrameBuilder::makeSupervisorFrame(local_address, data));
}

void DirectedTokenRing::send_user_login(const QString& username) {
	QByteArray data;
	data.append((quint8)SuperVisorFrameTypes::Login);
	data.append(local_address);
	data.append(username);
	send_frame(FrameBuilder::makeSupervisorFrame(local_address, data));
}

void DirectedTokenRing::frameReadyHandler(QByteArray data) {
	// decode with codec
	if (this->codec)
		data = codec->decode(data);
	
	if (data.length() <= 0)
		return;

	Frame f = FrameBuilder::fromByteArray(data);
	QByteArray fr_data = f.getData();

	if (f.getFrameType() == FrameType::SuperVisor) {
		
		SuperVisorFrameTypes fr_type = static_cast<SuperVisorFrameTypes>((quint8)fr_data[0]);
		if (fr_type == SuperVisorFrameTypes::Meeting) {
			quint8 inc_num = fr_data[1];
			if (def_number == 0) {
				qsrand(time(NULL));
				qsrand(qrand());
				def_number = qrand() % 254 + 1; // 1 - 255
			}
			if (inc_num == def_number)
				return;

			if (known_numbers.indexOf(inc_num) < 0)
				known_numbers.append(inc_num);

			std::sort(known_numbers.begin(), known_numbers.end());
			local_address = known_numbers.indexOf(def_number) + 1;

			send_frame(f);
			return;
		}

		if (fr_type == SuperVisorFrameTypes::Login) {
			QString username = fr_data.mid(2);
			pa[fr_data[1]] = username;
			emit userLoggedIn(username);
		}

		if (fr_type == SuperVisorFrameTypes::Logout) {
			pa.remove(fr_data[1]);
			QString username = fr_data.mid(2);
			emit userLoggedOut(username);
		}
		
		if (fr_type == SuperVisorFrameTypes::Disconnect) {
			emit LowLevelClient::errorOccured(LowLevelClientError::ConnectionClosed);
			return;
		}
	}
	if (f.getFrameType() == FrameType::Information) {
		if (f.getRecipients().indexOf(local_address) >= 0)
				emit new_message(f.getData());
	}

	if (f.getSender() != local_address)
		send_frame(data);
}

void DirectedTokenRing::onNetworkConnectionOpen() {
	// обновим статус клиента
	client_state = ClientState::Connected;
	emit ClientStateChanged(client_state);

	// ToDo: отправка кадра для знакомства
	QByteArray data;
	data.append((quint8)SuperVisorFrameTypes::Meeting);

	if (def_number == 0) {
		qsrand(time(NULL));
		qsrand(qrand());
		def_number = qrand() % 254 + 1; // 1 - 255
	}
	data.append(def_number);
	known_numbers.append(def_number);
	send_frame(FrameBuilder::makeSupervisorFrame(local_address, data));
}
