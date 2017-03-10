#include "directed_token_ring.h"

void DirectedTokenRing::send(QByteArray data) {
	// split to frames


	// encode each frame
	QByteArray encoded_data;
	if (this->codec)
		encoded_data = this->codec->encode(data);

	data.prepend(START_BYTE);
	data.append(STOP_BYTE);
	this->out->write(data);
}


void DirectedTokenRing::qserialreadHandler(void) {
	QByteArray data = this->in->readAll();
	// decode each frame
	if (this->codec)
		data = this->codec->decode(data);


	//QByteArrayList frame_st = data.split(START_BYTE);


	// merge all frames

	// handle inc message

	emit DirectedTokenRing::new_message(data);
}