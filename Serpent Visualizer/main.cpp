#include "pch.h"

//#define DEBUG

bool fast = false;

inline void showAlgorithmFlowchart(sf::RenderWindow* win, sf::Sprite algorithmSprite, byte stage, short spriteWidth, short spriteHeight, int msDelay) {
	algorithmSprite.setTextureRect(sf::IntRect(stage * spriteWidth, 0, spriteWidth, spriteHeight));
	algorithmSprite.setPosition((1280 - spriteWidth) / 2, (720 - spriteHeight) / 2);
	win->clear(sf::Color::White);
	win->draw(algorithmSprite);
	win->display();
	if (!fast)
		fast = waitWindow(win, msDelay);
}

inline byte* makeAndShowPermutation(sf::RenderWindow* win, sf::Sprite permutationTableSprite, std::wstring textString, sf::Font textFont, byte* data, byte* permataionTable) {
	sf::Text text(textString, textFont, 18);
	text.setPosition(10, 10);
	text.setFillColor(sf::Color::Black);
	win->clear(sf::Color::White);
	win->draw(text);
	win->draw(permutationTableSprite);
	win->display();

	if (!fast)
		fast = waitWindow(win, 1000);

	sf::RectangleShape cellRect(sf::Vector2f(34, 34));
	sf::RectangleShape dataBitRect(sf::Vector2f(10, 23));
	sf::RectangleShape distBitRect(sf::Vector2f(10, 23));
	cellRect.setFillColor(sf::Color(255, 0, 255, 127));
	dataBitRect.setFillColor(sf::Color(32, 32, 255, 127));
	distBitRect.setFillColor(sf::Color(255, 0, 0, 127));
	int cellRectX;
	int cellRectY;
	sf::Text keyBefore("", textFont, 18);
	sf::Text keyAfter("", textFont, 18);
	keyBefore.setFillColor(sf::Color::Black);
	keyAfter.setFillColor(sf::Color::Black);
	keyBefore.setPosition(10, 55);
	keyAfter.setPosition(10, 145);
	std::wstringstream keyBeforeTextStream;
	std::wstringstream keyAfterTextStream;

	std::wstringstream textStream;
	byte* temp = new byte[16];
	memset(temp, 0, 16);
	for (int i = 0; i < 128; i++) {
		// сначала выделяем нужный байт данных
		int selectedDataByteIndex = permataionTable[i] / 8;
		byte selectedDataByte = data[selectedDataByteIndex];
		int newDataBitNumber = 7 - permataionTable[i] % 8;
		bool bitValue = data[selectedDataByteIndex] & 1 << newDataBitNumber;

		// сначала выделяем нужный байт данных
		int selectedDistByteIndex = i / 8;
		byte newDistBitNumber = 7 - i % 8;
		temp[selectedDistByteIndex] |= bitValue << newDistBitNumber;

		// 710 и 11 - смещение от краев окна
		cellRectX = 710 + 35 * (i % 16);
		cellRectY = 11 + 35 * (i / 16);
		cellRect.setPosition(cellRectX, cellRectY);
		keyBeforeTextStream.str(L"");
		keyAfterTextStream.str(L"");
		memoryToText(&keyBeforeTextStream, data, 16);
		memoryToText(&keyAfterTextStream, temp, 16);

		keyBefore.setString(keyBeforeTextStream.str());
		keyAfter.setString(keyAfterTextStream.str());

		textStream.str(L"");
		textStream << textString << L"\nДо перестановки:\n\n\n\nПосле перестановки:\n\n\nБит "
			<< permataionTable[i] << L" переходит на позицию " << i;
		text.setString(textStream.str());

		// добавляем i / 8, т.к. после каждых 8 бит стоит пробел
		// после 64-го знака добавляем еще 1, чтобы учесть знак перехода на новую строку
		dataBitRect.setPosition(keyBefore.findCharacterPos(permataionTable[i] < 64 ? permataionTable[i] + permataionTable[i] / 8 : permataionTable[i] + permataionTable[i] / 8 + 1));
		distBitRect.setPosition(keyAfter.findCharacterPos(i < 64 ? i + i / 8 : i + i / 8 + 1));

		win->clear(sf::Color::White);
		win->draw(cellRect);
		win->draw(dataBitRect);
		win->draw(distBitRect);
		win->draw(permutationTableSprite);
		win->draw(text);
		win->draw(keyBefore);
		win->draw(keyAfter);
		win->display();
		if (!fast)
			fast = waitWindow(win, 200);
	}
	delete[] data;
	return temp;
}

inline byte* replaceBySboxInRound(sf::RenderWindow* win, int round, sf::Font font, sf::Sprite sBoxSprite, byte* data, byte** sBoxes, bool inverse = false) {
	std::wstringstream textStream;
	textStream.str(L"Текущий раунд: " + std::to_wstring(round) + L"\nЗамена бит данных с помощью S-блоков\n\nФиолетовый - номер таблицы\nКрасный - 4 старших бита выбранного байта\nЗеленый - 4 младших бита выбранного байта\nКаждые 4 бита - индексы ячейки таблицы. Они заменяются на значение ячейки.");
	sf::Text text(textStream.str(), font, 18);
	text.setFillColor(sf::Color::Black);
	text.setPosition(10, 10);
	win->clear(sf::Color::White);
	win->draw(text);
	win->draw(sBoxSprite);
	win->display();

	// текстура и спрайт картинки 
	sf::Texture ltTexture;
	ltTexture.loadFromFile("lt.png");
	sf::Sprite ltSprite(ltTexture);

	sf::RectangleShape highBitsRect(sf::Vector2f(34, 34));
	highBitsRect.setFillColor(sf::Color(255, 0, 0, 127));
	sf::RectangleShape lowBitsRect(sf::Vector2f(34, 34));
	lowBitsRect.setFillColor(sf::Color(0, 255, 0, 127));
	sf::RectangleShape tableNumberRect;

	// адаптация под более широкий спрайт таблицы обратной замены
	if (inverse)
		tableNumberRect.setSize(sf::Vector2f(64, 34));
	else
		tableNumberRect.setSize(sf::Vector2f(34, 34));

	tableNumberRect.setFillColor(sf::Color(255, 0, 255, 127));

	int Si = round % 8; // номер таблицы для отрисовки подсвечивания номера нужной таблицы
	byte* toUse = sBoxes[Si];
	byte* temp = new byte[16];
	for (int j = 0; j < 16; j++) {
		// выделяем отдельный байт
		int curr = data[j] & 0xFF;
		// получаем старшие 4 бита
		byte high4 = (byte)cyclicRightShiftInOtherEndianness(curr, 4);
		// получаем младшие 4 бита
		byte low4 = (byte)(curr & 0x0F);
		temp[j] = (byte)((toUse[high4] << 4) ^ (toUse[low4]));

		// адаптация под более широкий спрайт таблицы обратной замены
		if (inverse)
			tableNumberRect.setPosition(sf::Vector2f(640, 10 + Si * 35 + 1));
		else
			tableNumberRect.setPosition(sf::Vector2f(669, 10 + Si * 35 + 1));
		highBitsRect.setPosition(sf::Vector2f(669 + (high4 + 1) * 35, 11 + Si * 35));
		lowBitsRect.setPosition(sf::Vector2f(669 + (low4 + 1) * 35, 11 + Si * 35));

		textStream.str(L"");
		textStream << L"Текущий раунд: " + std::to_wstring(round) + L"\nВычисление блоков раундового ключа: замена с помощью S-блоков\n\nФиолетовый - номер таблицы\nКрасный - 4 старших бита выбранного байта\nЗеленый - 4 младших бита выбранного байта\nКаждые 4 бита - индексы ячейки таблицы. Они заменяются на значение ячейки.\n";
		textStream << L"Обрабатываемый ключ:\n";
		memoryToTextBigEndian(&textStream, data, 8);
		memoryToTextBigEndian(&textStream, data + 8, 8);
		textStream << L"Обрабатываемый байт:\t\t";
		memoryToTextBigEndian(&textStream, &curr, 1);
		textStream << L"\nРезультат замены:\t\t\t\t  ";
		memoryToTextBigEndian(&textStream, (void*)&temp[j], 1);
		textStream << L"\nНомер таблицы: ";
		textStream << Si;
		text.setString(textStream.str());

		win->clear(sf::Color::White);
		win->draw(text);
		win->draw(highBitsRect);
		win->draw(lowBitsRect);
		win->draw(sBoxSprite);
		win->draw(tableNumberRect);
		win->display();

		if (!fast)
			fast = waitWindow(win, 100);
	}
	delete[] data;
	return temp;
}

inline bool loadKeyFromFile(byte* key, int* startKeyLength) {
	std::ifstream keyFile("key.txt", std::ios::in);
	// Проверяем, открылся ли файл. Если нет - создаем его и пишем туда СМС об ошибке
	if (!keyFile.is_open()) {
		return false;
	}
	// Грузим ключ
	else {
		std::string line;
		std::getline(keyFile, line);
		if (line.size() > 32) {
			const char* cLine = line.c_str();
			*startKeyLength = 32;
			for (int i = 0; i < *startKeyLength; i++)
			{
				key[i] = cLine[i];
			}
		}
		else {
			const char* cLine = line.c_str();
			*startKeyLength = line.size();
			for (int i = 0; i < *startKeyLength; i++)
			{
				key[i] = cLine[i];
			}
		}

#ifdef DEBUG
		std::cout << "Key:\n";
		for (int i = 0; i < *startKeyLength; i++)
			std::cout << key[i];
		std::cout << "\n";
#endif // !DEBUG
	}
	return true;
}

inline bool loadDataFromFile(byte* data) {
	std::ifstream dataFile("data.txt");
	// Проверяем, открылся ли файл. Если нет - создаем его и пишем туда СМС об ошибке
	if (!dataFile.is_open()) {
		return false;
	}
	// Грузим данные для шифрования
	else {
		dataFile.read((char*)data, 16);
		dataFile.close();
#ifdef DEBUG
		std::cout << "\ndata:\n";
		for (int i = 0; i < 16; i++)
			std::cout << data[i];
		std::cout << "|END|\n";
#endif // !DEBUG
	}
	return true;
}

int main() {
#ifdef DEBUG
	system("chcp 1251");
#endif // DEBUG
#pragma region Инициализация и загрузка ресурсов
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(1280, 720), L"Визуализатор алгоритма Serpent", sf::Style::Close, settings);

	// длина ключа
	int startKeyLength;
	// ключ
	byte* key = new byte[32];
	// рабочие данные
	byte* data = new byte[16];
	memset(data, 0, 16);

	// исходные данные, для сравнение в самом конце
	byte* rawData = new byte[16];
	memset(rawData, 0, 16);

	// зашифрованные данные, для отображения
	byte* encryptedData = new byte[16];
	memset(encryptedData, 0, 16);

	sf::Font font = sf::Font();
	font.loadFromFile("calibri.ttf");

	std::wstringstream textStream; // поток для записи текста в юникоде
	sf::Text text("", font, 18);
	text.setFillColor(sf::Color::Black);

	sf::Texture encryptAlgorithmTexture;
	encryptAlgorithmTexture.loadFromFile("encryptAlgorithm.png");
	sf::Texture decryptAlgorithmTexture;
	decryptAlgorithmTexture.loadFromFile("decryptAlgorithm.png");
	sf::Texture ipTableTexture;
	ipTableTexture.loadFromFile("IP.png");
	sf::Texture fpTableTexture;
	fpTableTexture.loadFromFile("FP.png");
	sf::Texture siTableTexture;
	siTableTexture.loadFromFile("Si.png");
	sf::Texture invSiTableTexture;
	invSiTableTexture.loadFromFile("InvSi.png");
	sf::Texture effectiveLtTexture;
	effectiveLtTexture.loadFromFile("effectiveLT.png");
	sf::Texture invEffectiveLtTexture;
	invEffectiveLtTexture.loadFromFile("invEffectiveLT.png");
	sf::Texture keyTexture;
	keyTexture.loadFromFile("keySprite.png");

	sf::Sprite encryptAlgorithmSprite(encryptAlgorithmTexture);
	short encryptAlgorithmSpriteFrameWidth = 320;
	short encryptAlgorithmSpriteFrameHeight = 538;
	sf::Sprite decryptAlgorithmSprite(decryptAlgorithmTexture);
	short decryptAlgorithmSpriteFrameWidth = 274;
	short decryptAlgorithmSpriteFrameHeight = 618;
	sf::Sprite IPermTableSprite(ipTableTexture);
	IPermTableSprite.setPosition(709, 10);
	sf::Sprite FPermTableSprite(fpTableTexture);
	FPermTableSprite.setPosition(709, 10);
	sf::Sprite SiTableSprite(siTableTexture);
	SiTableSprite.setPosition(668, 10);
	sf::Sprite invSiTableSprite(invSiTableTexture);
	invSiTableSprite.setPosition(639, 10);
	sf::Sprite effectiveLtSprite(effectiveLtTexture);
	effectiveLtSprite.setPosition(919, 20);
	sf::Sprite invEffectiveLtSprite(invEffectiveLtTexture);
	invEffectiveLtSprite.setPosition(919, 20);
	sf::Sprite keySprite(keyTexture);
	keySprite.setPosition(492, 418);

	sf::Image icon;
	if (icon.loadFromFile("icon.png"))
	{
		win->setIcon(32, 32, icon.getPixelsPtr());
	}
#pragma endregion

#pragma region Загрузка ключа и данных для шифрования
	if (!loadKeyFromFile(key, &startKeyLength)) {
		delete[] key;
		delete[] data;
		std::ofstream key("key.txt");
		key << "Введите сюда ключ для шифрования (128/192/256 бит, 16/24/32 байт соответственно).\n";
		key.close();

		text.setString(L"Неверный запуск! Проверьте файл key.txt!");
		text.setPosition(500, 350);
		win->clear(sf::Color::White);
		win->draw(text);
		win->display();
		while (win->isOpen())
			fast = waitWindow(win, 1000);
		return 0;
	}
	
	if (!loadDataFromFile(data)) {
		std::ofstream dataFileError("data.txt");
		dataFileError << "Введите сюда данные для шифрования. Первые 128 бит (16 байт) будут зашифрованы.\n";
		dataFileError.close();

		text.setString(L"Неверный запуск! Проверьте файл data.txt!");
		text.setPosition(500, 350);
		win->clear(sf::Color::White);
		win->draw(text);
		win->display();
		delete[] key;
		delete[] data;
		while (win->isOpen())
			fast = waitWindow(win, 1000);
		return 0;
	}

	// копируем начальные входные данные в другой массив, чтобы наглядно сравнить после расшифрования
	memcpy(rawData, data, 16);
#pragma endregion

#pragma region Визуализация: проверка и расширение входного ключа
	if (startKeyLength > 32) {
		delete[] key;
		delete[] data;
		textStream.str(L"Ключ слишком длинный! Максимальная длина ключа - 256 бит.\n\t\t\t\t\tИзмените ключ и перезапустите программу");
		text.setString(textStream.str());
		text.setPosition(400, 320);
		win->clear(sf::Color::White);
		win->draw(text);
		while (win->isOpen())
			fast = waitWindow(win, 1000);
		delete win;
		return 0;
	}

	textStream << L"Исходный ключ, " << startKeyLength * 8 << L" бит\n";
	text.setPosition(10, 10);
	text.setString(textStream.str());

	// отрисовываем строку
	win->clear(sf::Color::White);
	win->draw(text);
	win->display();

	if (!fast)
		fast = waitWindow(win, 500);

	// анимация вывода исходного ключа
	for (int i = 0; i < startKeyLength; i++) {
		//memoryToTextBigEndian(&textStream, &key[i], 1);
		memoryToTextBigEndian(&textStream, &key[i], 1);
		if ((i + 1) % 8 == 0)
			textStream << L"\n";
		text.setString(textStream.str());
		win->clear(sf::Color::White);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 50);
	}

	std::wstringstream sFullKey;
	sf::Text tFullKey("", font, 18);
	tFullKey.setFillColor(sf::Color::Black);
	tFullKey.setPosition(640, 10);

	if (startKeyLength != 32) { // если длина ключа != 32 байта, т.е. 256 бит - расширяем!
		sFullKey << L"Ключ не полный, расширяем до 256 бит...\n";

		key = getFullKey(key, startKeyLength);

		for (int i = 0; i < 32; i++) {
			sFullKey << std::bitset<sizeof(key[i]) * 8>(key[i]) << " ";
			if ((i + 1) % 8 == 0)
				sFullKey << L"\n";
			tFullKey.setString(sFullKey.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(tFullKey);
			win->display();
			if (!fast)
				fast = waitWindow(win, 50);
		}

		if (!fast)
			fast = waitWindow(win, 1500);
	}
	else {
		sFullKey << L"Длина ключа равна 256 бит и в расширении не нуждается - начинаем шифрование";
		tFullKey.setString(sFullKey.str());
		win->draw(tFullKey);
		win->display();
		if (!fast)
			fast = waitWindow(win, 1500);
	}
#pragma endregion
	fast = false;
#pragma region Вывод текста об этапах
	{
		textStream.str(L"НАЧАЛО ШИФРОВАНИЯ");
		text.setString(textStream.str());
		text.setPosition(550, 320);
		win->clear(sf::Color::White);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 1000);

		textStream.str(L"ЭТАП 1 - ПОЛУЧЕНИЕ РАУНДОВЫХ КЛЮЧЕЙ");
		text.setString(textStream.str());
		text.setPosition(470, 320);
		win->clear(sf::Color::White);
		for (int i = 0; i < 24; i++)
		{
			// ширина кадра спрайта - 296, всего 12 кадров
			keySprite.setTextureRect(sf::IntRect(296 * (i % 12), 0, 296, 296));
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(keySprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 200);
		}
	}
#pragma endregion

	// разбитие ключа на 8 слов по 32 бита
	int* keyBlocks = devideKeyIntoBlocks(key);
	fast = false;
#pragma region Визуализация: разбитие ключа на 8 слов по 32 бита
	// вывод слов разными цветами
	{
		win->clear(sf::Color::White);
		textStream.str(L"Разбиваем ключ из 256 бит на 8 слов по 32 бита");
		text.setString(textStream.str());
		text.setPosition(10, 10);
		win->draw(text);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[0], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(0, 154, 87, 255));
		text.setPosition(15, 30);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[1], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(0, 154, 87, 255));
		text.setPosition(333, 30);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[2], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(0, 88, 153, 255));
		text.setPosition(649, 30);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[3], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(153, 88, 0, 255));
		text.setPosition(965, 30);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[4], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(153, 153, 0, 255));
		text.setPosition(15, 50);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[5], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(153, 0, 0, 255));
		text.setPosition(333, 50);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[6], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(153, 0, 153, 255));
		text.setPosition(649, 50);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[7], 4);
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Color(0, 0, 153, 255));
		text.setPosition(965, 50);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 500);
	}
#pragma endregion

#pragma region Вычисление промежуточных ключей
	// вычисляем промежуточные ключи
	int intermediateKeys[132];
	// дробная часть золотого сечения
	int phi = 0x9e3779b9;

	for (int i = 8; i < 140; i++) {
		intermediateKeys[i - 8] = (keyBlocks[(i - 8) % 8] ^ keyBlocks[(i - 5) % 8] ^ keyBlocks[(i - 3) % 8] ^ keyBlocks[(i - 1) % 8] ^ phi ^ (i - 8));
		cyclicLeftShiftInOtherEndianness(intermediateKeys[i - 8], 11); // циклический сдвиг слево на 11
	}
#pragma endregion
	fast = false;
#pragma region Визуализация: вычисление 132-х промежуточных ключей из 32-битных слов ( XOR(Wi) ^ f ^ i )
	// вывод вычисления промежуточного ключа
	{
		textStream.str(L"Формирование 132-х промежуточных ключей из 32-битных слов...");
		text.setString(textStream.str());
		text.setFillColor(sf::Color::Black);
		text.setPosition(10, 70);
		win->draw(text);

		sf::Texture intermediateKeyFormulaTexture;
		intermediateKeyFormulaTexture.loadFromFile("intermediateKeyFormula.png");
		sf::Sprite intermediateKeyFormulaSprite;
		intermediateKeyFormulaSprite.setTexture(intermediateKeyFormulaTexture);
		intermediateKeyFormulaSprite.setPosition(400, 93);
		win->draw(intermediateKeyFormulaSprite);

		win->display();
		if (!fast)
			fast = waitWindow(win, 500);

		textStream.str(L"Пример: вычисление 1-го промежуточного ключа");
		text.setString(textStream.str());
		text.setPosition(10, 120);
		win->draw(text);
		win->display();

		// выводим текст
		textStream.str(L"W(i-8) = \nW(i-5) = \nW(i-3) = \nW(i-1) = \nphi = \ni = \nResult = \nResult << 11 = ");
		text.setString(textStream.str());
		text.setPosition(10, 140);
		win->draw(text);

		// выводим блоки, phi и i
		textStream.str(L"");
		memoryToTextBigEndian(&textStream, &keyBlocks[0], 4);
		textStream << L"\n";
		memoryToTextBigEndian(&textStream, &keyBlocks[3], 4);
		textStream << L"\n";
		memoryToTextBigEndian(&textStream, &keyBlocks[5], 4);
		textStream << L"\n";
		memoryToTextBigEndian(&textStream, &keyBlocks[7], 4);
		textStream << L"\n";
		memoryToTextBigEndian(&textStream, &phi, 4);
		textStream << L"\n";
		// добавляем номер цикла, в примере равен 8, т.к. i = 8..140
		int data = 8;
		memoryToTextBigEndian(&textStream, &data, 4);
		textStream << L"\n";
		// result
		data = keyBlocks[0] ^ keyBlocks[3] ^ keyBlocks[5] ^ keyBlocks[7] ^ phi ^ data;
		memoryToTextBigEndian(&textStream, &data, 4);
		// result << 11
		data = cyclicLeftShiftInOtherEndianness(data, 11); // циклический сдвиг слево на 11
		textStream << L"\n";
		memoryToTextBigEndian(&textStream, &data, 4);

		text.setString(textStream.str());
		text.setPosition(115, 140);
		win->draw(text);
		win->display();
	}
#pragma endregion

	// чистим ненужное
	delete[] keyBlocks;

#pragma region Вычисление раундовых ключей
	// Предварительные раундовые ключи - обычные раундовые ключи без выполненной IP-перестановки
	byte* preRoundKeys[33];

	for (int i = 0, sCounter = 3; i < 33; i++, sCounter--)
	{
		if (sCounter == -1)
			sCounter = 7;
		preRoundKeys[i] = sBox((byte*)(intermediateKeys + 4 * i), sCounter);
	}
#pragma endregion

	if (!fast)
		fast = waitWindow(win, 2500);

	fast = false;
#pragma region Визуализация: вычисление раундовых ключей (замена с помощью S-блоков)
	// вывод вычисления блоков раундового ключа
	{
		win->clear(sf::Color::White);
		textStream.str(L"Вычисление блоков раундового ключа: замена с помощью S-блоков\n\nФиолетовый - номер таблицы\nКрасный - 4 старших бита выбранного байта\nЗеленый - 4 младших бита выбранного байта\nКаждые 4 бита - индексы ячейки таблицы. Они заменяются на значение ячейки.");
		text.setString(textStream.str());
		text.setPosition(10, 10);
		win->draw(text);
		win->display();

		if (!fast)
			fast = waitWindow(win, 500);

		SiTableSprite.setPosition(668, 10);
		win->draw(SiTableSprite);
		win->display();

		if (!fast)
			fast = waitWindow(win, 3000);

		sf::RectangleShape highBitsRect(sf::Vector2f(34, 34));
		highBitsRect.setFillColor(sf::Color(255, 0, 0, 127));
		sf::RectangleShape lowBitsRect(sf::Vector2f(34, 34));
		lowBitsRect.setFillColor(sf::Color(0, 255, 0, 127));
		sf::RectangleShape tableNumberRect(sf::Vector2f(34, 34));
		tableNumberRect.setFillColor(sf::Color(255, 0, 255, 127));

		for (int i = 0, sCounter = 3; i < 33; i++, sCounter--)
		{
			if (sCounter == -1)
				sCounter = 7;
			byte* data = (byte*)(intermediateKeys + 4 * i);
			{
				byte* toUse = sBoxes[sCounter % 8];
				byte* preRoundKey = new byte[16];
				int Si = sCounter % 8; // номер таблицы для отрисовки подсвечивания номера нужной таблицы
				for (int j = 0; j < 16; j++) {
					// выделяем отдельный байт
					int curr = data[j] & 0xFF;
					// получаем старшие 4 бита
					byte high4 = (byte)cyclicRightShiftInOtherEndianness(curr, 4);
					// получаем младшие 4 бита
					byte low4 = (byte)(curr & 0x0F);
					preRoundKey[j] = (byte)((toUse[high4] << 4) ^ (toUse[low4]));

					tableNumberRect.setPosition(sf::Vector2f(669, 10 + Si * 35 + 1));
					highBitsRect.setPosition(sf::Vector2f(669 + (high4 + 1) * 35, 11 + Si * 35));
					lowBitsRect.setPosition(sf::Vector2f(669 + (low4 + 1) * 35, 11 + Si * 35));

					textStream.str(L"");
					textStream << L"Вычисление блоков раундового ключа: замена с помощью S-блоков\n\nФиолетовый - номер таблицы\nКрасный - 4 старших бита выбранного байта\nЗеленый - 4 младших бита выбранного байта\nКаждые 4 бита - индексы ячейки таблицы. Они заменяются на значение ячейки.\n";
					textStream << L"Обрабатываемый ключ:\n";
					memoryToTextBigEndian(&textStream, data, 8);
					memoryToTextBigEndian(&textStream, data + 8, 8);
					textStream << L"Обрабатываемый байт:\t\t";
					memoryToTextBigEndian(&textStream, &curr, 1);
					textStream << L"\nРезультат замены:\t\t\t\t  ";
					memoryToTextBigEndian(&textStream, (void*)&preRoundKey[j], 1);
					textStream << L"\nНомер таблицы: ";
					textStream << Si;
					text.setString(textStream.str());

					win->clear(sf::Color::White);
					win->draw(text);
					win->draw(highBitsRect);
					win->draw(lowBitsRect);
					win->draw(SiTableSprite);
					win->draw(tableNumberRect);
					win->display();

					if (!fast)
						fast = waitWindow(win, 100);
				}
				delete[] preRoundKey;
			}
		}
	}
#pragma endregion

#pragma region Вычисление финальных раундовых ключей с помощью IP
	byte** roundKeys = new byte * [33];
	for (int i = 0; i < 33; i++)
		roundKeys[i] = doInitPermutation((byte*)(preRoundKeys + 4 * i));
#pragma endregion
	fast = false;
#pragma region Визуализация: начальная перестановка (IP) бит раундовых ключей
	byte* tempKey = new byte[16];
	memcpy(tempKey, preRoundKeys[0], 16);
	tempKey = makeAndShowPermutation(win, IPermTableSprite, L"Начальная перестановка бит ключа на примере 1-го раундового ключа", font, tempKey, ipTable);
	delete[] tempKey;
#pragma endregion

	// удаляем уже ненужный массив
	for (int i = 0; i < 33; i++)
		delete[] preRoundKeys[i];

	fast = false;
#pragma region Вывод текста об этапах (шифрование)
	textStream.str(L"ЭТАП 2 - ШИФРОВАНИЕ ИНФОРМАЦИИ");
	text.setString(textStream.str());
	text.setPosition(488, 320);
	win->clear(sf::Color::White);
	win->draw(text);
	win->display();
	if (!fast)
		fast = waitWindow(win, 1500);
#pragma endregion

	fast = false;
#pragma region Визуализация: начальная перестановка (IP) бит данных
	showAlgorithmFlowchart(win, encryptAlgorithmSprite, 0, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 1000);
	showAlgorithmFlowchart(win, encryptAlgorithmSprite, 1, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 1000);

#ifdef DEBUG
	std::cout << "\nДанные перед начальной перестановкой:\n";
	mempnt(data, 16);
#endif // DEBUG

	data = makeAndShowPermutation(win, IPermTableSprite, L"Начальная перестановка бит данных", font, data, ipTable);
#pragma endregion

#ifdef DEBUG
	std::cout << "\nДанные после начальной перестановки и перед первым раундовым ключом:\n";
	mempnt(data, 16);
#endif // DEBUG

	fast = false;
#pragma region Визуализация: 32 раунда
	byte* temp = nullptr;

	for (int round = 0; round < 32; round++)
	{
#pragma region Визуализация: раунд, наложение ключа
		// отображение блок-схемы алгоритма
		showAlgorithmFlowchart(win, encryptAlgorithmSprite, 1, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
		showAlgorithmFlowchart(win, encryptAlgorithmSprite, 2, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);

#ifdef DEBUG
		std::cout << "\nРаунд " << round + 1 << ". Данные перед наложением ключа:\n";
		mempnt(data, 16);
#endif // DEBUG

		temp = new byte[16];
		memcpy(temp, data, 16);
		byte* roundKey = roundKeys[round];

#ifdef DEBUG
		std::cout << "\nКлюч:\n";
		mempnt(roundKey, 16);
#endif // DEBUG
		text.setPosition(340, 250);

		for (int n = 0; n < 16; n++) {
			temp[n] = (byte)(data[n] ^ roundKey[n]);
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nНаложение ключа на данные...\n";
			memoryToText(&textStream, data, 16);
			textStream << L"+\n";
			memoryToText(&textStream, roundKey, 16);
			textStream << L"=\n";
			memoryToText(&textStream, temp, n + 1);
			text.setString(textStream.str());

			win->clear(sf::Color::White);
			win->draw(text);
			win->display();
			if (!fast)
				fast = waitWindow(win, 100);
		}

		delete[] data;
		data = temp;
		temp = nullptr;
#pragma endregion

#ifdef DEBUG
		std::cout << "\nРаунд " << round + 1 << ". Данные после наложением ключа (перед S-блоком):\n";
		mempnt(data, 16);
#endif // DEBUG

#pragma region Визуализация: раунд, замена с помощью S-блоков
		showAlgorithmFlowchart(win, encryptAlgorithmSprite, 2, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
		showAlgorithmFlowchart(win, encryptAlgorithmSprite, 3, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
		data = replaceBySboxInRound(win, round, font, SiTableSprite, data, sBoxes);
#pragma endregion

#ifdef DEBUG
		std::cout << "\nРаунд " << round + 1 << ". Данные после S-блока:\n";
		mempnt(data, 16);
#endif // DEBUG

		if (round == 31) {
#pragma region Отдельная обработка последнего раунда
			// накладываем ключ
			temp = new byte[16];
			memset(temp, 0, 16);
			showAlgorithmFlowchart(win, encryptAlgorithmSprite, 4, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
			showAlgorithmFlowchart(win, encryptAlgorithmSprite, 6, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
			text.setPosition(337, 255);
			roundKey = roundKeys[32];

#ifdef DEBUG
			std::cout << "\nРаунд " << round + 1 << ". Данные перед наложением последнего ключа:\n";
			mempnt(data, 16);
#endif // DEBUG

			for (int n = 0; n < 16; n++) {
				temp[n] = (byte)(data[n] ^ roundKey[n]);
				textStream.str(L"");
				textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nНаложение последнего ключа на данные...\n";
				memoryToText(&textStream, data, 16);
				textStream << L"+\n";
				memoryToText(&textStream, roundKey, 16);
				textStream << L"=\n";
				memoryToText(&textStream, temp, n + 1);
				text.setString(textStream.str());

				win->clear(sf::Color::White);
				win->draw(text);
				win->display();
				if (!fast)
					fast = waitWindow(win, 100);
			}
			delete[] data;
			data = temp;
			temp = nullptr;
#ifdef DEBUG
			std::cout << "\nРаунд " << round + 1 << ". Данные после наложения последнего ключа и перед конечной перестановкой:\n";
			mempnt(data, 16);
#endif // DEBUG
#pragma endregion
		}
		else {
		// иначе делаем линейное преобразование: IP(LT(FP(x))), где LT и есть линейное преобразование
			showAlgorithmFlowchart(win, encryptAlgorithmSprite, 4, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
			showAlgorithmFlowchart(win, encryptAlgorithmSprite, 5, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);

			// (FP(x)
#ifdef DEBUG
			std::cout << "\nРаунд " << round + 1 << ". Данные после S-блока и перед LT FP:\n";
			mempnt(data, 16);
#endif // DEBUG

			data = makeAndShowPermutation(win, FPermTableSprite, L"Текущий раунд: " + std::to_wstring(round + 1) + L". Линейное преобразование: конечная перестановка бит данных", font, data, fpTable);

#ifdef DEBUG
			std::cout << "\nРаунд " << round + 1 << ". Данные после LT FP и перед main LT:\n";
			mempnt(data, 16);
#endif // DEBUG

			// LT(FP(x))
			text.setPosition(10, 10);
			
			IntByteBuffer buffer(data);
			
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"Начальное состояние:\n";
			memoryToText(&textStream, data, 16);
			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 1) X0 = X0 <<< 13
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X0 до X0 = X0 <<< 13:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);
			buffer.ints[0] = ((buffer.ints[0] << 13) | (cyclicRightShift(buffer.ints[0], (32 - 13))));
			textStream << L"\nX0 после X0 = X0 <<< 13:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 2) X2 = X2 <<< 3
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X2 до X2 = X2 <<< 3:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);
			buffer.ints[2] = ((buffer.ints[2] << 3) | (cyclicRightShift(buffer.ints[2], (32 - 3))));
			textStream << L"\nX2 после X2 = X2 <<< 3:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 3) X1 = X1 + X0 + X2
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X1 до X1 = X1 + X0 + X2:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);
			buffer.ints[1] = buffer.ints[1] ^ buffer.ints[0] ^ buffer.ints[2];
			textStream << L"\nX1 после X1 = X1 + X0 + X2:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 4) X3 = X3 + X2 + (X0 << 3)
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X3 до X3 = X3 + X2 + (X0 << 3):\n";
			memoryToText(&textStream, &buffer.ints[3], 4);
			buffer.ints[3] = buffer.ints[3] ^ buffer.ints[2] ^ (buffer.ints[0] << 3);
			textStream << L"\nX3 после X3 = X3 + X2 + (X0 << 3):\n";
			memoryToText(&textStream, &buffer.ints[3], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 5) X1 = X1 <<< 1
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X1 до X1 = X1 <<< 1:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);
			buffer.ints[1] = (buffer.ints[1] << 1) | (cyclicRightShift(buffer.ints[1], (32 - 1)));
			textStream << L"\nX1 после X1 = X1 <<< 1:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 6) X3 = X3 <<< 7
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X3 до X3 = X3 <<< 7:\n";
			memoryToText(&textStream, &buffer.ints[3], 4);
			buffer.ints[3] = (buffer.ints[3] << 7) | (cyclicRightShift(buffer.ints[3], (32 - 7)));
			textStream << L"\nX3 после X3 = X3 <<< 7:\n";
			memoryToText(&textStream, &buffer.ints[3], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 7) X0 = X0 + X1 + X3
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X0 до X0 = X0 + X1 + X3:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);
			buffer.ints[0] = buffer.ints[0] ^ buffer.ints[1] ^ buffer.ints[3];
			textStream << L"\nX0 после X0 = X0 + X1 + X3:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 8) X2 = X2 + X3 + (X1 << 7)
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X2 до X2 = X2 + X3 + (X1 << 7):\n";
			memoryToText(&textStream, &buffer.ints[2], 4);
			buffer.ints[2] = buffer.ints[2] ^ buffer.ints[3] ^ (buffer.ints[1] << 7);
			textStream << L"\nX2 после X2 = X2 + X3 + (X1 << 7):\n";
			memoryToText(&textStream, &buffer.ints[2], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 9) X0 = X0 <<< 5
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X0 до X0 = X0 <<< 5:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);
			buffer.ints[0] = (buffer.ints[0] << 5) | (cyclicRightShift(buffer.ints[0], (32 - 5)));
			textStream << L"\nX0 после X0 = X0 <<< 5:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 10) X2 = X2 <<< 22
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X2 до X2 = X2 <<< 22:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);
			buffer.ints[2] = (buffer.ints[2] << 22) | (cyclicRightShift(buffer.ints[2], (32 - 22)));
			textStream << L"\nX2 после X2 = X2 <<< 22:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(effectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 3000);

			memcpy(data, buffer.bytes, 16);

#ifdef DEBUG
			std::cout << "\nРаунд " << round + 1 << ". Данные после main LT и перед LT IP:\n";
			mempnt(data, 16);
#endif // DEBUG

			// IP(LT(FP(x)))
			data = makeAndShowPermutation(win, IPermTableSprite, L"Текущий раунд: " + std::to_wstring(round + 1) + L". Линейное преобразование: начальная перестановка бит данных", font, data, ipTable);

#ifdef DEBUG
			std::cout << "\nРаунд " << round + 1 << ". Данные после LT IP и перед раундовым ключом:\n";
			mempnt(data, 16);
#endif // DEBUG
		}
	}
#pragma endregion

	fast = false;

#pragma region Визуализация: конечная перестановка (FP) бит данных
	// отображение блок-схемы алгоритма
	showAlgorithmFlowchart(win, encryptAlgorithmSprite, 6, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
	showAlgorithmFlowchart(win, encryptAlgorithmSprite, 7, encryptAlgorithmSpriteFrameWidth, encryptAlgorithmSpriteFrameHeight, 500);
	data = makeAndShowPermutation(win, FPermTableSprite, L"Конечная перестановка бит данных", font, data, fpTable);
#pragma endregion

#ifdef DEBUG
	std::cout << "\nШифртекст после конечной перестановки:\n";
	mempnt(data, 16);
#endif // DEBUG

#pragma region Запись зашифрованного блока в файл и другой массив
	{
		std::ofstream encrypted("encrypted.bin");
		if (!encrypted.is_open()) {
#ifdef DEBUG
			std::cout << "encrypted block writing: file not opened!\n";
#endif // DEBUG
		}
		else {
			for (int i = 0; i < 16; i++)
			{
				encrypted << data[i];
			}
			encrypted.close();
#ifdef DEBUG
			std::cout << "encrypted block writing: written! :-)\n";
#endif // DEBUG
		}
	}
	// копируем зашифрованный блок данных в другой массив, чтобы отобразить его после расшифрования
	memcpy(encryptedData, data, 16);
#pragma endregion

	fast = false;

#pragma region Вывод текста об этапах (расшифрование)
	textStream.str(L"ЭТАП 3 - РАСШИФРОВКА ШИФРТЕКСТА");
	text.setString(textStream.str());
	text.setPosition(488, 320);
	win->clear(sf::Color::White);
	win->draw(text);
	win->display();
	if (!fast)
		fast = waitWindow(win, 1500);
#pragma endregion

	fast = false;

#ifdef DEBUG
	std::cout << "\n\n\nРАСШИФРОВАНИЕ\nШифртекст перед начальной перестановкой:\n";
	mempnt(data, 16);
#endif // DEBUG

#pragma region Начальная перестановка шифртекста (отмена конечной перестановки)
	showAlgorithmFlowchart(win, decryptAlgorithmSprite, 0, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
	showAlgorithmFlowchart(win, decryptAlgorithmSprite, 1, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
	data = makeAndShowPermutation(win, IPermTableSprite, L"Начальная перестановка шифртекста", font, data, ipTable);
#pragma endregion

	fast = false;

#ifdef DEBUG
	std::cout << "\nШифртекст после начальной перестановки и перед наложением последнего ключа\n";
	mempnt(data, 16);
#endif // DEBUG

#pragma region Наложение 32-го ключа для его отмены
	showAlgorithmFlowchart(win, decryptAlgorithmSprite, 1, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
	showAlgorithmFlowchart(win, decryptAlgorithmSprite, 2, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);

	text.setPosition(340, 250);
	byte* roundKey = roundKeys[32];
	temp = new byte[16];
	memset(temp, 0, 16);
	for (int n = 0; n < 16; n++) {
		temp[n] = (byte)(data[n] ^ roundKey[n]);
		textStream.str(L"");
		textStream << L"Наложение 32-го ключа на шифртекст\n";
		memoryToText(&textStream, data, 16);
		textStream << L"+\n";
		memoryToText(&textStream, roundKey, 16);
		textStream << L"=\n";
		memoryToText(&textStream, temp, n + 1);
		text.setString(textStream.str());

		win->clear(sf::Color::White);
		win->draw(text);
		win->display();
		if (!fast)
			fast = waitWindow(win, 100);
	}

	delete[] data;
	data = temp;
	temp = nullptr;
#pragma endregion

#ifdef DEBUG
	std::cout << "\nШифртекст после наложения последнего ключа и перед iS-блоком:\n";
	mempnt(data, 16);
#endif // DEBUG
#pragma region 32 раунда в обратном порядке
	for (int round = 31; round >= 0; round--) {
		showAlgorithmFlowchart(win, decryptAlgorithmSprite, 3, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
		// обратное линейное преобразование, которое пропускается на 31-м раунде
		if (round != 31) {
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 7, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
			data = makeAndShowPermutation(win, FPermTableSprite, L"Текущий раунд: " + std::to_wstring(round + 1) + L". Обратное лин. преобразование: конечная перестановка шифртекста", font, data, fpTable);

#ifdef DEBUG
			std::cout << "\nОтмена раунда " << round + 1 << ". Шифртекст после LT FP и перед main LT:\n";
			mempnt(data, 16);
#endif // DEBUG

			IntByteBuffer buffer(data);
			text.setPosition(10, 10);

			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"Начальное состояние:\n";
			memoryToText(&textStream, data, 16);
			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 1) X2 = X2 >>> 22
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nОбратное линейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X2 до X2 = X2 >>> 22:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);
			buffer.ints[2] = cyclicRightShift(buffer.ints[2], 22) | (buffer.ints[2] << (32 - 22));
			textStream << L"\nX2 после X2 = X2 >>> 22:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 2) X0 = X0 >>> 5
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X0 до X0 = X0 >>> 5:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);
			buffer.ints[0] = cyclicRightShift(buffer.ints[0], 5) | (buffer.ints[0] << (32 - 5));
			textStream << L"\nX0 после X0 = X0 >>> 5:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 3) X2 = X2 + X3 + (X1 << 7)
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X2 до X2 + X3 + (X1 << 7):\n";
			memoryToText(&textStream, &buffer.ints[2], 4);
			buffer.ints[2] = buffer.ints[2] ^ buffer.ints[3] ^ (buffer.ints[1] << 7);
			textStream << L"\nX2 после X2 + X3 + (X1 << 7):\n";
			memoryToText(&textStream, &buffer.ints[2], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 4) X0 = X0 + X1 + X3
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X0 до X0 = X0 + X1 + X3:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);
			buffer.ints[0] = buffer.ints[0] ^ buffer.ints[1] ^ buffer.ints[3];
			textStream << L"\nX0 после X0 = X0 + X1 + X3:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 5) X3 = X3 >>> 7
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X3 до X3 = X3 >>> 7:\n";
			memoryToText(&textStream, &buffer.ints[3], 4);
			buffer.ints[3] = cyclicRightShift(buffer.ints[3], 7) | (buffer.ints[3] << (32 - 7));
			textStream << L"\nX3 после X3 = X3 >>> 7:\n";
			memoryToText(&textStream, &buffer.ints[3], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 6) X1 = X1 >>> 1
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X1 до X1 = X1 >>> 1:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);
			buffer.ints[1] = cyclicRightShift(buffer.ints[1], 1) | (buffer.ints[1] << (32 - 1));
			textStream << L"\nX1 после X1 = X1 >>> 1:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 7) X3 = X3 + X2 + (X0 << 3)
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X3 до X3 = X3 + X2 + (X0 << 3):\n";
			memoryToText(&textStream, &buffer.ints[3], 4);
			buffer.ints[3] = buffer.ints[3] ^ buffer.ints[2] ^ (buffer.ints[0] << 3);
			textStream << L"\nX3 после X3 = X3 + X2 + (X0 << 3):\n";
			memoryToText(&textStream, &buffer.ints[3], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 8) X1 = X1 + X0 + X2
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X1 до X1 = X1 + X0 + X2:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);
			buffer.ints[1] = buffer.ints[1] ^ buffer.ints[0] ^ buffer.ints[2];
			textStream << L"\nX1 после X1 = X1 + X0 + X2:\n";
			memoryToText(&textStream, &buffer.ints[1], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 9) X2 = X2 >>> 3
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X2 до X2 = X2 >>> 3:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);
			buffer.ints[2] = cyclicRightShift(buffer.ints[2], 3) | (buffer.ints[2] << (32 - 3));
			textStream << L"\nX2 после X2 = X2 >>> 3:\n";
			memoryToText(&textStream, &buffer.ints[2], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 500);

			// 10) X0 = X0 >>> 13
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nЛинейное преобразование: эффективная реализация трансформации блока данных\n";
			textStream << L"X0 до X0 = X0 >>> 13:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);
			buffer.ints[0] = cyclicRightShift(buffer.ints[0], 13) | (buffer.ints[0] << (32 - 13));
			textStream << L"\nX0 после X0 = X0 >>> 13:\n";
			memoryToText(&textStream, &buffer.ints[0], 4);

			text.setString(textStream.str());
			win->clear(sf::Color::White);
			win->draw(text);
			win->draw(invEffectiveLtSprite);
			win->display();
			if (!fast)
				fast = waitWindow(win, 3000);
			
			//buffer.ints[2] = cyclicRightShift(buffer.ints[2], 22) | (buffer.ints[2] << (32 - 22));
			//buffer.ints[0] = cyclicRightShift(buffer.ints[0], 5) | (buffer.ints[0] << (32 - 5));
			//buffer.ints[2] = buffer.ints[2] ^ buffer.ints[3] ^ (buffer.ints[1] << 7);
			//buffer.ints[0] = buffer.ints[0] ^ buffer.ints[1] ^ buffer.ints[3];
			//buffer.ints[3] = cyclicRightShift(buffer.ints[3], 7) | (buffer.ints[3] << (32 - 7));
			//buffer.ints[1] = cyclicRightShift(buffer.ints[1], 1) | (buffer.ints[1] << (32 - 1));
			//buffer.ints[3] = buffer.ints[3] ^ buffer.ints[2] ^ (buffer.ints[0] << 3);
			//buffer.ints[1] = buffer.ints[1] ^ buffer.ints[0] ^ buffer.ints[2];
			//buffer.ints[2] = cyclicRightShift(buffer.ints[2], 3) | (buffer.ints[2] << (32 - 3));
			//buffer.ints[0] = cyclicRightShift(buffer.ints[0], 13) | (buffer.ints[0] << (32 - 13));

			memcpy(data, buffer.bytes, 16);

#ifdef DEBUG
			std::cout << "\nОтмена раунда " << round + 1 << ". Шифртекст после main LT и перед LT IP:\n";
			mempnt(data, 16);
#endif // DEBUG

			data = makeAndShowPermutation(win, IPermTableSprite, L"Текущий раунд: " + std::to_wstring(round + 1) + L". Обратное лин. преобразование: начальная перестановка шифртекста", font, data, ipTable);

#ifdef DEBUG
			std::cout << "\nОтмена раунда " << round + 1 << ". Шифртекст после LT IP и перед iS-блоком:\n";
			mempnt(data, 16);
#endif // DEBUG
		}

#ifdef DEBUG
		std::cout << "\nОтмена раунда " << round + 1 << ". Шифртекст перед iS-блоком:\n";
		mempnt(data, 16);
#endif // DEBUG

		if (round == 31) {
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 4, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
		}
		else {
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 7, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 8, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
		}

		// замена с помощью обратных S-блоков
		data = replaceBySboxInRound(win, round, font, invSiTableSprite, data, isBoxes, true);

#ifdef DEBUG
		std::cout << "\nОтмена раунда " << round + 1 << ". Шифртекст после iS-блока и перед наложением ключа:\n";
		mempnt(data, 16);
#endif // DEBUG

		if (round == 31) {
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 4, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 5, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
		}
		else {
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 8, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 9, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
		}

		
		// накладываем ключ
		text.setPosition(340, 250);
		roundKey = roundKeys[round];
#ifdef DEBUG
		std::cout << "\nКлюч:\n";
		mempnt(roundKey, 16);
#endif // DEBUG
		temp = new byte[16];
		for (int n = 0; n < 16; n++) {
			temp[n] = (byte)(data[n] ^ roundKey[n]);
			textStream.str(L"");
			textStream << L"Текущий раунд: " + std::to_wstring(round + 1) + L"\nНаложение ключа на данные...\n";
			memoryToText(&textStream, data, 16);
			textStream << L"+\n";
			memoryToText(&textStream, roundKey, 16);
			textStream << L"=\n";
			memoryToText(&textStream, temp, n + 1);
			text.setString(textStream.str());

			win->clear(sf::Color::White);
			win->draw(text);
			win->display();
			if (!fast)
				fast = waitWindow(win, 100);
		}

		delete[] data;
		data = temp;
		temp = nullptr;

#ifdef DEBUG
		std::cout << "\nОтмена раунда " << round + 1 << ". Шифртекст после наложения ключа и перед LT FP:\n";
		mempnt(data, 16);
#endif // DEBUG

		if (round == 31) {
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 5, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 6, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
		}
		else {
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 9, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
			showAlgorithmFlowchart(win, decryptAlgorithmSprite, 10, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
		}
	}
#pragma endregion

	// конечная перестановка шифртекста для отмены самой первой начальной перестановки
	showAlgorithmFlowchart(win, decryptAlgorithmSprite, 10, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
	showAlgorithmFlowchart(win, decryptAlgorithmSprite, 11, decryptAlgorithmSpriteFrameWidth, decryptAlgorithmSpriteFrameHeight, 500);
	data = makeAndShowPermutation(win, FPermTableSprite, L"Конечная перестановка шифртекста", font, data, fpTable);

#pragma region Запись расшифрованного блока в файл
	{
		std::ofstream encrypted("decrypted.txt");
		if (!encrypted.is_open()) {
#ifdef DEBUG
			std::cout << "not opened!\n";
#endif // DEBUG
		}
		else {
			for (int i = 0; i < 16; i++)
			{
				encrypted << data[i];
			}
			encrypted.close();
#ifdef DEBUG
			std::cout << "File maybe baby was writed! :-)\n";
#endif // DEBUG
		}
	}
#pragma endregion

	textStream.str(L"");
	textStream << L"Процессы шифрования и расшифрования окончены.\nИсходный блок данных:\n";
	memoryToText(&textStream, rawData, 16);
	textStream << L"\nЗашифрованный блок данных:\n";
	memoryToText(&textStream, encryptedData, 16);
	textStream << L"\nРасшифрованный блок данных:\n";
	memoryToText(&textStream, data, 16);

	// освобождение ресурсов
	delete[] key;
	for (int i = 0; i < 33; i++)
	{
		delete[] roundKeys[i];
	}
	delete[] roundKeys;
	delete[] data;
	delete[] rawData;
	delete[] encryptedData;

	text.setString(textStream.str());
	text.setPosition(340, 230);

	win->clear(sf::Color::White);
	win->draw(text);

	while (win->isOpen()) {
		sf::Event event;
		while (win->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				win->close();
			}
		}
		win->display();
		Sleep(250);
	}
	delete win;
}
