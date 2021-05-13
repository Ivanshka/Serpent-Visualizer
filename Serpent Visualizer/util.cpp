#include "pch.h"
#include "util.h"

bool waitWindow(sf::RenderWindow* win, int mlSec) {
	sf::Clock clock;
	clock.restart();
	float sumTime = 0;
	while (clock.getElapsedTime().asMilliseconds() < mlSec) {
		sf::Event event;
		while (win->pollEvent(event))
			if (event.type == sf::Event::Closed)
				win->close();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) // дополнение для возможности ускорить выполнение по нажатию F
			return true;
		Sleep(25);
	}
	return false;
}

void memoryToTextBigEndian(std::wstringstream* outStream, void* memory, int byteCount) {
	char* address = (char*)memory;
	for (int i = byteCount - 1, endLine = 1; i >= 0; i--, endLine++) {	// прямой порядок байт
		*outStream << std::bitset<sizeof(address[i]) * 8>(address[i]) << " ";
		if (endLine % 8 == 0)
			*outStream << L"\n";
	}
}

void memoryToText(std::wstringstream* outStream, void* memory, int byteCount) {
	char* address = (char*)memory;
	for (int i = 0; i < byteCount; i++) {	// обратный порядок байт
		*outStream << std::bitset<sizeof(address[i]) * 8>(address[i]) << " ";
		if ((i + 1) % 8 == 0)
			*outStream << L"\n";
	}
}

void mempnt(void* memory, int byteCount) {
	char* address = (char*)memory;
	for (int i = 0; i < byteCount; i++) {	// обратный порядок байт
		std::cout << std::bitset<sizeof(address[i]) * 8>(address[i]) << " ";
		if ((i + 1) % 8 == 0)
			std::cout << "\n";
	}
}

void mempntioe(void* memory, int byteCount) {
	char* address = (char*)memory;
	for (int i = byteCount - 1, endLine = 1; i >= 0; i--, endLine++) {	// прямой порядок байт
		std::cout << std::bitset<sizeof(address[i]) * 8>(address[i]) << " ";
		if (endLine % 8 == 0)
			std::cout << "\n";
	}
}

void checkEndianness()
{
	uint16_t x = 0x0001;
	printf("%s-endian\n", *((uint8_t*)&x) ? "little" : "big");
}

int cyclicLeftShift(int integer, int steps) {
	std::bitset<32> bit(integer), part1, part2, result;
	part1 = bit << steps;
	part2 = bit >> 32 - steps;
	result = part1 | part2;
	return (int)result.to_ulong();
}

int cyclicRightShift(int integer, int steps) {
	std::bitset<32> bit(integer), part1, part2, result;
	part1 = bit >> steps;
	part2 = bit << 32 - steps;
	result = part1 | part2;
	return (int)result.to_ulong();
}

int cyclicLeftShiftInOtherEndianness(int integer, int steps) {
	byte* byteArray = (byte*)&integer;
	byte buf;
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	integer = cyclicLeftShift(integer, steps);
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	return integer;
}

int cyclicRightShiftInOtherEndianness(int integer, int steps) {
	byte* byteArray = (byte*)&integer;
	byte buf;
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	integer = cyclicRightShift(integer, steps);
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	return integer;
}

int leftShiftInOtherEndianness(int integer, int steps)
{
	byte* byteArray = (byte*)&integer;
	byte buf;
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	integer = integer << steps;
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	return integer;
}

int rightShiftInOtherEndianness(int integer, int steps)
{
	byte* byteArray = (byte*)&integer;
	byte buf;
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	integer = integer >> steps;
	buf = byteArray[0];
	byteArray[0] = byteArray[3];
	byteArray[3] = buf;
	buf = byteArray[1];
	byteArray[1] = byteArray[2];
	byteArray[2] = buf;
	return integer;
}

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}