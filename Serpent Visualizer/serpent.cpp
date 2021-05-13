#include "pch.h"
#include "serpent.h"

// таблица начальной перестановки
byte* ipTable = new byte[]{
	 0, 32, 64,  96,  1, 33, 65,  97,  2, 34, 66,  98,  3, 35, 67,  99,
	 4, 36, 68, 100,  5, 37, 69, 101,  6, 38, 70, 102,  7, 39, 71, 103,
	 8, 40, 72, 104,  9, 41, 73, 105, 10, 42, 74, 106, 11, 43, 75, 107,
	12, 44, 76, 108, 13, 45, 77, 109, 14, 46, 78, 110, 15, 47, 79, 111,
	16, 48, 80, 112, 17, 49, 81, 113, 18, 50, 82, 114, 19, 51, 83, 115,
	20, 52, 84, 116, 21, 53, 85, 117, 22, 54, 86, 118, 23, 55, 87, 119,
	24, 56, 88, 120, 25, 57, 89, 121, 26, 58, 90, 122, 27, 59, 91, 123,
	28, 60, 92, 124, 29, 61, 93, 125, 30, 62, 94, 126, 31, 63, 95, 127
};

// таблица конечной перестановки
byte* fpTable = new byte[]{
	 0,  4,  8, 12, 16, 20, 24, 28, 32,  36,  40,  44,  48,  52,  56,  60,
	64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
	 1,  5,  9, 13, 17, 21, 25, 29, 33,  37,  41,  45,  49,  53,  57,  61,
	65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125,
	 2,  6, 10, 14, 18, 22, 26, 30, 34,  38,  42,  46,  50,  54,  58,  62,
	66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126,
	 3,  7, 11, 15, 19, 23, 27, 31, 35,  39,  43,  47,  51,  55,  59,  63,
	67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127
};

// Таблицы замен
byte* s0 = new byte[]{ 3, 8, 15, 1, 10, 6, 5, 11, 14, 13, 4, 2, 7, 0, 9, 12 };
byte* s1 = new byte[]{ 15, 12, 2, 7, 9, 0, 5, 10, 1, 11, 14, 8, 6, 13, 3, 4 };
byte* s2 = new byte[]{ 8, 6, 7, 9, 3, 12, 10, 15, 13, 1, 14, 4, 0, 11, 5, 2 };
byte* s3 = new byte[]{ 0, 15, 11, 8, 12, 9, 6, 3, 13, 1, 2, 4, 10, 7, 5, 14 };
byte* s4 = new byte[]{ 1, 15, 8, 3, 12, 0, 11, 6, 2, 5, 4, 10, 9, 14, 7, 13 };
byte* s5 = new byte[]{ 15, 5, 2, 11, 4, 10, 9, 12, 0, 3, 14, 8, 13, 6, 7, 1 };
byte* s6 = new byte[]{ 7, 2, 12, 5, 8, 4, 6, 11, 14, 9, 1, 15, 13, 3, 10, 0 };
byte* s7 = new byte[]{ 1, 13, 15, 0, 14, 8, 2, 11, 7, 4, 12, 10, 9, 3, 5, 6 };
byte** sBoxes = new byte * [] { s0, s1, s2, s3, s4, s5, s6, s7 };

// Инверсные (обратные) таблицы замен
byte* is0 = new byte[]{ 13, 3, 11, 0, 10, 6, 5, 12, 1, 14, 4, 7, 15, 9, 8, 2 };
byte* is1 = new byte[]{ 5, 8, 2, 14, 15, 6, 12, 3, 11, 4, 7, 9, 1, 13, 10, 0 };
byte* is2 = new byte[]{ 12, 9, 15, 4, 11, 14, 1, 2, 0, 3, 6, 13, 5, 8, 10, 7 };
byte* is3 = new byte[]{ 0, 9, 10, 7, 11, 14, 6, 13, 3, 5, 12, 2, 4, 8, 15, 1 };
byte* is4 = new byte[]{ 5, 0, 8, 3, 10, 9, 7, 14, 2, 12, 11, 6, 4, 15, 13, 1 };
byte* is5 = new byte[]{ 8, 15, 2, 9, 4, 1, 13, 14, 11, 6, 5, 3, 7, 12, 10, 0 };
byte* is6 = new byte[]{ 15, 10, 1, 13, 5, 3, 6, 0, 4, 9, 14, 7, 2, 12, 8, 11 };
byte* is7 = new byte[]{ 3, 0, 6, 13, 9, 14, 15, 8, 5, 12, 11, 7, 10, 1, 4, 2 };
byte** isBoxes = new byte * [] { is0, is1, is2, is3, is4, is5, is6, is7 };

byte* getFullKey(byte* startKey, int startKeyLength) {

	byte* fullKey = startKey;

	// проверка длины ключа и расширение ключа до 256 бит (32 байта)
	if (startKeyLength < 32) {
		fullKey = new byte[32]; // новый ключ
		memcpy(fullKey, startKey, startKeyLength); // копируем старый в новый и дополняем
		for (int i = startKeyLength; i < 32; i++) {
			if (i == startKeyLength) {
				fullKey[i] = (byte)0x80; // начало дополнения
			}
			else {
				fullKey[i] = (byte)0x00;
			}
		}
	}

	delete[] startKey;
	return fullKey;
}

int* devideKeyIntoBlocks(byte* fullKey) {
	int* keyBlocks = new int[8];
	for (int i = 0; i < 8; i++)
		memcpy(keyBlocks + i, fullKey + 4 * i, 4);
	return keyBlocks;
}

// Заменяет 16 байт (4 int) по таблице замен и возвращает указатель на новое число
byte* sBox(byte* data, int round) {
	byte* toUse = sBoxes[round % 8];
	byte* output = new byte[16];
	for (int i = 0; i < 16; i++) {
		//Разрыв со знаком
		int curr = data[i] & 0xFF;
		byte high4 = (byte)cyclicRightShift(curr, 4);
		byte low4 = (byte)(curr & 0x0F);
		output[i] = (byte)((toUse[high4] << 4) ^ (toUse[low4]));
	}
	return output;
}

byte* sBoxInv(byte* data, int round) {
	byte* toUse = isBoxes[round % 8];
	byte* output = new byte[16];
	for (int i = 0; i < 16; i++) {
		//Break signed-ness
		int curr = data[i] & 0xFF;
		byte high4 = (byte)cyclicRightShift(curr, 4);
		byte low4 = (byte)(curr & 0x0F);
		output[i] = (byte)((toUse[high4] << 4) ^ (toUse[low4]));
	}
	return output;
}

// Функция начальной перестановки
byte* initPermutation(byte* data)
{
	byte* output = new byte[16];
	memset(output, 0, 16);
	for (int i = 0; i < 128; i++) {
		//Bit permutation based on IP lookup table
		int bit = cyclicRightShift(data[(ipTable[i]) / 8], ((ipTable[i]) % 8)) & 0x01;
		if ((bit & 0x01) == 1)
			output[15 - (i / 8)] |= 1 << (i % 8);
		else
			output[15 - (i / 8)] &= ~(1 << (i % 8));
	}
	return output;
}

//Функция конечной перестановки
byte* finalPermutation(byte* data) {
	byte* output = new byte[16];
	memset(output, 0, 16);
	for (int i = 0; i < 128; i++) {
		//Bit permutation based on FP lookup table
		int bit = cyclicRightShift(data[15 - fpTable[i] / 8], (fpTable[i] % 8)) & 0x01;
		if ((bit & 0x01) == 1)
			output[(i / 8)] |= 1 << (i % 8);
		else
			output[(i / 8)] &= ~(1 << (i % 8));
	}
	return output;
}

void doPermutation(int structureSize, int dataBitNumber, int distBitNumber, byte* data, byte* dist) {
	if (dataBitNumber > structureSize * 8 || distBitNumber > structureSize * 8)
		throw "dataBitNumber > structureSize * 8 || distBitNumber > structureSize * 8";
	// сначала выделяем нужный байт данных
	int selectedDataByteIndex = dataBitNumber / 8;
	byte selectedDataByte = data[selectedDataByteIndex];
	//printf("\nselected byte: %d", selectedDataByte);
	int newDataBitNumber = 7 - dataBitNumber % 8;
	bool bitValue = data[selectedDataByteIndex] & 1 << newDataBitNumber;
	//printf("\nbit %d is %d, dist bit is %d\n", dataBitNumber, bitValue, distBitNumber);

	// сначала выделяем нужный байт данных
	int selectedDistByteIndex = distBitNumber / 8;
	byte newDistBitNumber = 7 - distBitNumber % 8;
	dist[selectedDistByteIndex] |= bitValue << newDistBitNumber;

	//memprtle(dist, 4);
}

byte* doInitPermutation(byte* data) {
	byte* dist = new byte[16];
	memset(dist, 0, 16);
	for (int i = 0; i < 128; i++) {
		doPermutation(16, ipTable[i], i, data, dist);
	}
	return dist;
}

byte* doFinalPermutation(byte* data) {
	byte* dist = new byte[16];
	memset(dist, 0, 16);
	for (int i = 0; i < 128; i++) {
		doPermutation(16, fpTable[i], i, data, dist);
	}
	return dist;
}
