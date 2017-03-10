#include "directed_token_ring.h"

void DirectedTokenRing::send(const QByteArray& data) {
	// split to frames

	// encode each frame
	
}

void DirectedTokenRing::qserialreadHandler() {
	// decode each frame

	// merge all frames

	// handle inc message

	emit DirectedTokenRing::readyRead();
}