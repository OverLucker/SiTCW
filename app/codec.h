#pragma once


#ifndef DATA_LINK_LAYER_CODEC
#define DATA_LINK_LAYER_CODEC

class Codec {
public:
	virtual const QByteArray& encode(QByteArray& data) = 0;
	virtual const QByteArray& decode(QByteArray& data) = 0;
};


class HammingCodec : public Codec {

public:
	virtual const QByteArray& encode(QByteArray& data) {
		// ToDo: write encoder
		return data;
	}

	virtual const QByteArray& decode(QByteArray& data) {
		// ToDo: write decoder
		return data;
	}
};


#endif // !DATA_LINK_LAYER_CODEC
