#pragma once
#include "pch.h"

bool waitWindow(sf::RenderWindow* win, int mlSec);
void memoryToTextBigEndian(std::wstringstream* outStream, void* memory, int byteCount);
void memoryToText(std::wstringstream* outStream, void* memory, int byteCount);

int cyclicLeftShift(int integer, int steps);
int cyclicRightShift(int integer, int steps);

int cyclicLeftShiftInOtherEndianness(int integer, int steps);
int cyclicRightShiftInOtherEndianness(int integer, int steps);
