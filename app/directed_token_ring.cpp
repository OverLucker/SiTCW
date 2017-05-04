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

	handlers[SuperVisorFrameTypes::Meeting] = &DirectedTokenRing::handlerMeeting;
	handlers[SuperVisorFrameTypes::Disconnect] = &DirectedTokenRing::handlerDisconnect;
	handlers[SuperVisorFrameTypes::Login] = &DirectedTokenRing::handlerLogin;
	handlers[SuperVisorFrameTypes::Logout] = &DirectedTokenRing::handlerLogout;
}

QList<QString> DirectedTokenRing::get_contacts() { return pa.values(); }

void DirectedTokenRing::ringErrorHandler(LowLevelClientError error) {
	if (error == LowLevelClientError::ConnectionClosed) {
		QByteArray data;
		data.append((quint8)SuperVisorFrameTypes::Disconnect);
		send_frame(FrameBuilder::makeSupervisorFrame(local_address, data));
		timer->stop();
		known_numbers.clear();
		known_numbers.append(def_number);
		// обновим статус клиента
		setClientState(ClientState::Offline);
	}

}

void DirectedTokenRing::send_frame(QByteArray data) {
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
	timer->start();
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
	Frame f = FrameBuilder::fromByteArray(data);
	FrameType frame_type = f.getFrameType();
	if (frame_type == FrameType::Null)
		return;

	if (frame_type == FrameType::SuperVisor) {
		SuperVisorFrameTypes fr_type = static_cast<SuperVisorFrameTypes>((quint8)f.getData()[0]);
		(this->*handlers[fr_type])(f);
	}

	if (frame_type == FrameType::Information) {
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

void DirectedTokenRing::handlerMeeting(Frame& f) {
	QByteArray fr_data = f.getData();
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

	QByteArray new_data;
	new_data.append((quint8)SuperVisorFrameTypes::Meeting);
	new_data.append(inc_num);
	send_frame(FrameBuilder::makeSupervisorFrame(0, new_data));
}

void DirectedTokenRing::handlerDisconnect(Frame&) {
	setClientState(ClientState::Offline);
	this->onNetworkConnectionOpen();
}

void DirectedTokenRing::handlerLogin(Frame& f) {
	QString username = f.getData().mid(1);

	if (!pa.contains(f.getSender())) {
		pa[f.getSender()] = username;
		emit userLoggedIn(username);
	}
}

void DirectedTokenRing::handlerLogout(Frame& f) {
	pa.remove(f.getSender());
	QString username = f.getData().mid(1);
	timer->stop();
	emit userLoggedOut(username);
}
