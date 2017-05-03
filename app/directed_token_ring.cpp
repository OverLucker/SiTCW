#include "directed_token_ring.h"
#include <QMessageBox>
#include <algorithm>
#include <time.h>


#define LOG_OUT_DELAY 500
#define SINGLE_FRAME_AWAIT_TIMER 3000


DirectedTokenRing::DirectedTokenRing(QObject* parent) : LowLevelClient(parent) {
	connect(this, &LowLevelClient::frame_ready, this, &DirectedTokenRing::frameReadyHandler);
	connect(this, &LowLevelClient::connectionOpen, this, &DirectedTokenRing::onNetworkConnectionOpen);
	connect(this, &LowLevelClient::errorOccured, this, &DirectedTokenRing::ringErrorHandler);
	connect(timer, &QTimer::timeout, this, &DirectedTokenRing::frameTimeout);

	qsrand(time(NULL));
	qsrand(qrand());
	def_number = qrand() % 254 + 1; // 1 - 255
	known_numbers.append(def_number);
	timer->setSingleShot(true);
	timer->setInterval(SINGLE_FRAME_AWAIT_TIMER);
}

QList<QString> DirectedTokenRing::get_contacts() { return pa.values(); }

void DirectedTokenRing::ringErrorHandler(LowLevelClientError error) {
	if (error == LowLevelClientError::ConnectionClosed) {
		QByteArray data;
		data.append((quint8)SuperVisorFrameTypes::Disconnect);
		send_frame(FrameBuilder::makeSupervisorFrame(local_address, data));
		// обновим статус клиента
		setClientState(ClientState::Offline);
		timer->stop();
		known_numbers.clear();
		known_numbers.append(def_number);
	}

}

void DirectedTokenRing::send_frame(QByteArray data) {
	// encode with codec
	if (this->codec)
		data = codec->encode(data);

	LowLevelClient::send(data);
}

void DirectedTokenRing::frameTimeout() {
	send_frame(last_frame);
	timer->start();
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
	data.append(username);
	this->last_frame = FrameBuilder::makeSupervisorFrame(local_address, data);
	send_frame(this->last_frame);
	// timer->start();
}

void DirectedTokenRing::send_user_login(const QString& username) {
	QByteArray data;
	data.append((quint8)SuperVisorFrameTypes::Login);
	data.append(username);
	this->last_frame = FrameBuilder::makeSupervisorFrame(local_address, data);
	send_frame(this->last_frame);
	timer->start();
}

void DirectedTokenRing::frameReadyHandler(QByteArray data) {
	// decode with codec
	if (this->codec)
		data = codec->decode(data);

	Frame f = FrameBuilder::fromByteArray(data);
	if (f.getFrameType() == FrameType::Null)
		return;

	QByteArray fr_data = f.getData();

	if (f.getFrameType() == FrameType::SuperVisor) {
		
		SuperVisorFrameTypes fr_type = static_cast<SuperVisorFrameTypes>((quint8)fr_data[0]);
		if (fr_type == SuperVisorFrameTypes::Meeting) {
			quint8 inc_num = fr_data[1];

			if (known_numbers.indexOf(inc_num) < 0) {
				known_numbers.append(inc_num);
				std::sort(known_numbers.begin(), known_numbers.end());
			}

			local_address = known_numbers.indexOf(def_number) + 1;

			if (inc_num == def_number) {
				timer->stop();
				// обновим статус клиента
				setClientState(ClientState::Online);
				return;
			}

			QByteArray data;
			data.append((quint8)SuperVisorFrameTypes::Meeting);
			data.append(inc_num);
			send_frame(FrameBuilder::makeSupervisorFrame(0, data));
			return;
		}

		if (fr_type == SuperVisorFrameTypes::Login) {
			QString username = fr_data.mid(1);
			
			if (!pa.contains(f.getSender())) {
				pa[f.getSender()] = username;
				emit userLoggedIn(username);
			}
		}

		if (fr_type == SuperVisorFrameTypes::Logout) {
			pa.remove(f.getSender());
			QString username = fr_data.mid(1);
			timer->stop();
			emit userLoggedOut(username);
		}
		
		if (fr_type == SuperVisorFrameTypes::Disconnect) {
			setClientState(ClientState::Offline);
			this->onNetworkConnectionOpen();
		}
	}
	if (f.getFrameType() == FrameType::Information) {
		if (f.getRecipients().indexOf(local_address) >= 0)
				emit new_message(f.getData());
	}

	if (f.getSender() != local_address)
		send_frame(data);
	else {
		timer->stop();
	}
}

void DirectedTokenRing::onNetworkConnectionOpen() {
	QByteArray data;
	data.append((quint8)SuperVisorFrameTypes::Meeting);
	data.append(def_number);
	this->last_frame = FrameBuilder::makeSupervisorFrame(0, data);
	send_frame(last_frame);
	timer->start();
}
