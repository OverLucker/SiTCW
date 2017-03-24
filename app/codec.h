
#pragma once
#include <bitset>
#include <QBitArray>


#ifndef DATA_LINK_LAYER_CODEC
#define DATA_LINK_LAYER_CODEC

template <const int bitCount >
class Codec {
public:
	virtual const QByteArray& encode(const QByteArray& data) = 0;
	virtual const QByteArray& decode(const QByteArray& data) = 0;
};


template <const int bitCount>
class HammingCodec : public Codec<bitCount> {

public:
	// НАЧАЛЬНЫЕ УСЛОВИЯ!!!
	//#define CORRECT
	//const int bitCount = 15;
	//const char myVector[] = "10101010100";
	// НАЧАЛЬНЫЕ УСЛОВИЯ!!!

	// ВСПОМОГАТЕЛЬНЫЕ РАСЧЕТЫ И ДИРЕКТИВЫ
	const int pow2count = ceil(log(bitCount) / log(2));
	const int baseWord = bitCount - pow2count;
	typedef std::bitset <bitCount> bitmap;

	HammingCodec() {
	}
	// подсчитывает кол-во 1 битов в числе
	int bit_count(bitmap& src)
	{
		int bCount = 0;
		for (int i = 0; i < bitCount; ++i)
			if (src[i])
				bCount++;
		return bCount;
	}

	// считает контрольную сумму 
	int control_sum(bitmap& src) {
		int err = 0;
		for (int i = 0; i < bitCount; ++i)
		{
			if (src[i])
				err ^= (i + 1);
		}

		return err;
	}


	// Располагает биты в правильном порядке в векторе
	bitmap gen_full_vector(bitmap src)
	{
		bitmap ans;

		for (int i = 0, o = 0; i < bitCount; ++i)
		{
			if (bit_count(bitmap(i + 1)) <= 1)
				continue;
			ans[i] = src[o++];
		}

		// расставим биты по местам
		int err = control_sum(ans);
		for (int tmp = 1; tmp < bitCount; tmp = tmp << 1)
			ans[tmp - 1] = err & tmp;


		return ans;
	}

	// исправляет вектор по мере возможностей
	bitmap fix_vector(bitmap src) {
		bitmap i = control_sum(src);
		int err = i.to_ulong();

		if (err > 0)
			src[err - 1] = !src[err - 1];
		return src;
	}

	QBitArray QByteArrayToBitArray(const QByteArray& byteArray) {
		// Create a bit array of the appropriate size
		QBitArray bitArray(byteArray.count() * 8);

		// Convert from QByteArray to QBitArray
		for (int i = 0; i < byteArray.count(); ++i) {
			for (int b = 0; b < 8; b++) {
				bitArray.setBit(i * 8 + b, byteArray.at(i)&(1 << (7 - b)));
			}
		}
		return bitArray;
	}

	QByteArray bitsetToByteArray(bitmap& bitSet) {
		QByteArray byteArray;
		int maxSize = bitCount / 8 + 1;
		byteArray.resize(maxSize);
		for (int i = 0; i < maxSize; i++)
			byteArray[i] = 0x00;
		// Convert from bitset to QByteArray
		for (int b = 0; b<bitCount; ++b) {
			byteArray[b / 8] = (byteArray.at(b / 8) | ((bitSet[b] ? 1 : 0) << (7 - (b % 8))));
		}
		return byteArray;
	}

	virtual const QByteArray& encode(const QByteArray& data) {
		QBitArray dataBits = QByteArrayToBitArray(data);

		if (int addingBites = dataBits.size() % baseWord) {
			dataBits.resize(dataBits.size() + addingBites);
		}

		int length = dataBits.size();
		QByteArray encodedData;


		for (int i = 0; i < length; i+=baseWord) {
			char* word = new char[baseWord+1];
			for (int j = 0; j < baseWord; j++) {
				word[j] = dataBits[i + j] ? '1' : '0';
			}
			word[baseWord] = '\0';
			bitmap myVec = bitmap(word);
			bitmap fullVector = gen_full_vector(myVec);
			QByteArray encodedWord = bitsetToByteArray(fullVector);
			//QByteArray encodedWord = bitsetToByteArray(myVec);
			encodedData.append(encodedWord);
			delete[] word;

		}

		return data;
	}

	virtual const QByteArray& decode(const QByteArray& data) {

		return data;
	}
};


#endif // !DATA_LINK_LAYER_CODEC
