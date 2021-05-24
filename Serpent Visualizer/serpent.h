#pragma once
#include "pch.h"

typedef unsigned char byte;

union IntByteBuffer {
	byte bytes[16];
	int ints[4];

	IntByteBuffer(byte* byteArray) {
		for (int i = 0; i < 16; i++)
			bytes[i] = byteArray[i];
	}
};

// ������� ��������� ������������
extern byte* ipTable;

// ������� �������� ������������
extern byte* fpTable;

// ������� �����
extern byte* s0;
extern byte* s1;
extern byte* s2;
extern byte* s3;
extern byte* s4;
extern byte* s5;
extern byte* s6;
extern byte* s7;
extern byte** sBoxes;

// ��������� (��������) ������� �����
extern byte* is0;
extern byte* is1;
extern byte* is2;
extern byte* is3;
extern byte* is4;
extern byte* is5;
extern byte* is6;
extern byte* is7;
extern byte** isBoxes;

byte* getFullKey(byte* startKey, int startKeyLength);
int* devideKeyIntoBlocks(byte* fullKey);
byte* sBox(byte* data, int round);

void doPermutation(int structureSize, int dataBitNumber, int distBitNumber, byte* data, byte* dist);
byte* doInitPermutation(byte* data);
byte* doFinalPermutation(byte* data);
