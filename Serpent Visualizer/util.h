#pragma once
#include "pch.h"

bool waitWindow(sf::RenderWindow* win, int mlSec);
void memoryToTextBigEndian(std::wstringstream* outStream, void* memory, int byteCount);
void memoryToText(std::wstringstream* outStream, void* memory, int byteCount);

void mempnt(void* memory, int byteCount);
void mempntioe(void* memory, int byteCount);

void checkEndianness();

int cyclicLeftShift(int integer, int steps);
int cyclicRightShift(int integer, int steps);

int cyclicLeftShiftInOtherEndianness(int integer, int steps);
int cyclicRightShiftInOtherEndianness(int integer, int steps);

int leftShiftInOtherEndianness(int integer, int steps);
int rightShiftInOtherEndianness(int integer, int steps);

std::vector<std::string> split(const std::string& s, char delim);
