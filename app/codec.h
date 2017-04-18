
#pragma once
#include <bitset>
#include <QBitArray>
#include <string>


#ifndef DATA_LINK_LAYER_CODEC
#define DATA_LINK_LAYER_CODEC

class Codec {
public:
	virtual QByteArray encode(const QByteArray& data) = 0;
	//virtual const QByteArray& decode(const QByteArray& data, QByteArray& decodedData) = 0;
	virtual QByteArray decode(const QByteArray& data) = 0;
};


template <const int bitCount>
class HammingCodec : public Codec {

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

	bitmap decode_full_vectore (bitmap src) {
		int err

		for (int i = 0, o = 0; i < bitCount; ++i)
		{
			if (bit_count(bitmap(i + 1)) <= 1)
				continue;
			ans[i] = src[o++];
		}

	}

	bool check_for_error(QBitArray src) {
		int sum = 0;
		for (int i = 0; i < bitCount; ++i)
		{
			/*if (bit_count(bitmap(i + 1)) == 1 && src[bitCount - i-1]) {
				err = true;
				break;
			}*/
			if (src[bitCount -i-1])
				sum ^= (i + 1);
		}
		bool err = sum>0;
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
	
	QByteArray bitArrayToByteArray(QBitArray bits) {
		// Resulting byte array
			QByteArray bytes;
			int maxSize = bits.count() / 8 + 1;
			bytes.resize(maxSize);
			for (int i = 0; i < maxSize; i++)
				bytes[i] = 0x00;
		// Convert from QBitArray to QByteArray
		for (int b = 0; b<bits.count(); ++b)
			bytes[b / 8] = (bytes.at(b / 8) | ((bits[b] ? 1 : 0) << (7 - (b % 8))));
		return bytes;
	}

	virtual QByteArray encode(const QByteArray& data) {
		QByteArray encodedData;
		QBitArray dataBits = QByteArrayToBitArray(data);

		if (int addingBites = dataBits.size() % baseWord) {
			dataBits.resize(dataBits.size() + addingBites);
		}

		encodedData = "";

		for (int i = 0, length = dataBits.size(); i < length; i+=baseWord) {
			char* word = new char[baseWord+1];
			for (int j = 0; j < baseWord; j++) {
				word[j] = dataBits[i + j] ? '1' : '0';
			}
			word[baseWord] = '\0';
			bitmap myVec = bitmap(word);
			bitmap fullVector = gen_full_vector(myVec);
			QByteArray encodedWord = bitsetToByteArray(fullVector);
			//QByteArray encodedWord = bitsetToByteArray(myVec);
			encodedData.append((char)fullVector.to_ulong());
			delete[] word;
		}
		//data = encodedData;
		return encodedData;
	}

	QBitArray getMeaningBits(QBitArray word) {
		QBitArray meaningBits(baseWord);
		for (int i = 0, o=baseWord-1; i < bitCount; ++i)
		{
			if (bit_count(bitmap(i + 1)) != 1) {
				meaningBits[o--] = word[bitCount - i - 1];
			}
		}
		return meaningBits;
	}
	
	virtual QByteArray decode(const QByteArray& data) {
		QBitArray dataBits = QByteArrayToBitArray(data);
		int length = dataBits.size();

		//all crash
		QBitArray decodedDataInBits(length / bitCount*baseWord);
		int currentWordNumber = 0;
		for (int i = 0; i < length; i += 8) {
			
			QBitArray word(bitCount);
			// works only for 7 bit mode !!!
		
			//bitcount>8 all crash
			for (int j = 8-bitCount, o=0; j <= bitCount; j++) {
				if (dataBits[i + j])
					word.setBit(o);
				o++;
			}

			if (check_for_error(word)) {
				return QByteArray();
			}

			QBitArray decodedBits = getMeaningBits(word);
			//QByteArray encodedWord = bitsetToByteArray(myVec);
			for (int j = 0; j < baseWord; j++)
				decodedDataInBits[baseWord*currentWordNumber + j] = decodedBits[j];
			currentWordNumber++;
		}

		return bitArrayToByteArray(decodedDataInBits);
	}
};


#endif // !DATA_LINK_LAYER_CODEC
