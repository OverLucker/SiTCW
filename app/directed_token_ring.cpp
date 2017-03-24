#include "directed_token_ring.h"
#//include <QMessageBox>

DirectedTokenRing::DirectedTokenRing(QObject* parent) : LowLevelClient(parent) {
	connect(this, &LowLevelClient::frame_ready, this, &DirectedTokenRing::frameReadyHandler);
	connect(this, &LowLevelClient::connectionOpen, this, &DirectedTokenRing::onNetworkConnectionOpen);
	createPhysicalAddress();
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

void DirectedTokenRing::send(QByteArray data) {
	// encode with codec
	if (this->codec)
		data = codec->encode(data);

	LowLevelClient::send(data);
}

void DirectedTokenRing::frameReadyHandler(QByteArray data) {
	// decode with codec
	if (this->codec)
		data = codec->decode(data);

	// ToDo: нужен обработчик кадров

	emit new_message(data);
}

void DirectedTokenRing::onNetworkConnectionOpen() {
	// обновим статус клиента
	client_state = ClientState::Connected;
	emit ClientStateChanged(client_state);

	// ToDo: отправка кадра для знакомства
}
