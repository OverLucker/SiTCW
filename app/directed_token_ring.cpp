#include "directed_token_ring.h"
#//include <QMessageBox>

DirectedTokenRing::DirectedTokenRing(QObject* parent) : LowLevelClient(parent) {
	connect(this, &LowLevelClient::frame_ready, this, &DirectedTokenRing::frameReadyHandler);
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
	//example for debug
	/*QMessageBox msgBox;
	msgBox.setWindowTitle("example");
	msgBox.setText(QString::number((int)error));
	msgBox.exec();
	*/
	//QMessageBox::information(thQMainWindow sd;is, "Connection status", (int)error); 
	//ui.textBrowser->append(err);
}

void DirectedTokenRing::send(QByteArray data) {
	// encode with codec
	if (this->codec)
		data = codec->encode(data);

	LowLevelClient::send(data);
}

void DirectedTokenRing::frameReadyHandler(QByteArray data) {
	emit new_message(data);
}