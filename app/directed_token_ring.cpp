#include "directed_token_ring.h"
#include "data_link_layer.h"
#include <QMessageBox>

#include <time.h>


DirectedTokenRing::DirectedTokenRing(QObject* parent) : LowLevelClient(parent) {
	connect(this, &LowLevelClient::frame_ready, this, &DirectedTokenRing::frameReadyHandler);
	connect(this, &LowLevelClient::connectionOpen, this, &DirectedTokenRing::onNetworkConnectionOpen);
}

QList<QString> DirectedTokenRing::get_contacts() {
	return pa.values();
}

void DirectedTokenRing::ringErrorHandler(LowLevelClientError error) {
	if (error == LowLevelClientError::ConnectionClosed) {
		//TODO: After release special frame, send it to "out"
		DirectedTokenRing::network_disconnect();
	}

}

void DirectedTokenRing::send_frame(QByteArray data) {
	// encode with codec
	if (this->codec)
		data = codec->encode(data);

	LowLevelClient::send(data);
}

void DirectedTokenRing::send(QByteArray data) {
	// Функционал этой процедуры был перемещен в DirectedTokenRing::send_frame
	//  в связи с количеством вызовов и общностью использования
	//  теперь эта процедура отвечает за отправку информационных кадров
	QVector<quint8> rec;
	rec.append(1);
	send_frame(FrameBuilder::makeInformationFrame(local_address, rec, data));
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

	// ToDo: нужен обработчик кадров
	if (data.length() <= 0)
		return;

	Frame f = FrameBuilder::fromByteArray(data);

	if (f.getFrameType() == FrameType::SuperVisor) {
		QByteArray fr_data = f.getData();
		if ((quint8)fr_data[0] == (quint8)SuperVisorFrameTypes::Meeting) {
			QVector<quint8> def_numbers;
			if (app_address && def_number) {
				for (int i = 1; fr_data.at(i) != 0; i += 2) {
					if (fr_data.at(i) == app_address && fr_data.at(i + 1) == def_number)
						if (second_round)
							return;
						local_address = app_address;
						send_frame(FrameBuilder::makeSupervisorFrame(local_address, fr_data));
						second_round = true;
						return;
					def_numbers.append(fr_data.at(i + 1));
				}

			}
			
			app_address = fr_data.at(fr_data.length() - 2) + 1;
			fr_data.append(app_address);
			def_number = rand() % 254 + 1;
			while(def_numbers.indexOf(def_number))
				def_number = rand() % 254 + 1;

			fr_data.append(def_number);
			send_frame(FrameBuilder::makeSupervisorFrame(local_address, fr_data));
			
		}
		if ((quint8)f.getData()[0] == (quint8)SuperVisorFrameTypes::Login) {
			QString username = f.getData().mid(2);
			pa[f.getData()[1]] = username;
			emit userLoggedIn(username.append(48+f.getData()[1]));
		}
	}
	else
		if (f.getFrameType() == FrameType::Information)
			emit new_message(f.getData());
}

void DirectedTokenRing::onNetworkConnectionOpen() {
	// обновим статус клиента
	client_state = ClientState::Connected;
	emit ClientStateChanged(client_state);

	// ToDo: отправка кадра для знакомства
	QByteArray data;
	data.append((quint8)SuperVisorFrameTypes::Meeting);
	app_address = 1;
	data.append(app_address);

	qsrand(time(NULL));
	def_number = qrand() % 254 + 1; // 1 - 255
	data.append(def_number);
	send_frame(FrameBuilder::makeSupervisorFrame(local_address, data));
}
