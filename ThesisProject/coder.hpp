#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstddef>
#include <stdint.h>
#include <math.h>
#include <fstream>
#include <string>
#include <bitset>
#include "error_corrector.hpp"

using namespace std;
using namespace cv;

class Coder {
private:
	int modulesNumber;
	static ErrorCorrector errorCorrector;

public:
	map<char, int> alphanumericValuesMap
	{
		std::make_pair('0', 1),
		std::make_pair('1', 2),
		std::make_pair('2', 3),
		std::make_pair('3', 4),
		std::make_pair('4', 5),
		std::make_pair('5', 6),
		std::make_pair('6', 7),
		std::make_pair('7', 8),
		std::make_pair('8', 9),
		std::make_pair('9', 10),
		std::make_pair('A', 11),
		std::make_pair('B', 12),
		std::make_pair('C', 13),
		std::make_pair('D', 14),
		std::make_pair('E', 15),
		std::make_pair('F', 16),
		std::make_pair('G', 17),
		std::make_pair('H', 18),
		std::make_pair('I', 19),
		std::make_pair('J', 20),
		std::make_pair('K', 21),
		std::make_pair('L', 22),
		std::make_pair('M', 23),
		std::make_pair('N', 24),
		std::make_pair('O', 25),
		std::make_pair('P', 26),
		std::make_pair('R', 27),
		std::make_pair('S', 28),
		std::make_pair('T', 29),
		std::make_pair('U', 30),
		std::make_pair('V', 31),
		std::make_pair('W', 32),
		std::make_pair('X', 33),
		std::make_pair('Y', 34),
		std::make_pair('Z', 35),
		std::make_pair(' ', 36),
		std::make_pair('$', 37),
		std::make_pair('%', 38),
		std::make_pair('*', 39),
		std::make_pair('+', 40),
		std::make_pair('-', 41),
		std::make_pair('.', 42),
		std::make_pair('/', 43),
		std::make_pair(':', 44),
	};

	static const int BITS_IN_MODULE = 3;

	static const int BIT_NUM = 11;
	static const int SHORT_BIT_NUM = 6;
	static const int BYTE_BIT_NUM = 8;

	static const int PALETTE_NUM = 8;
	static const int ANGLES_NUM = 4;
	const struct color
	{
		float r, g, b;
	};
	static const int COLOR_NUMBER = 8;
	static color palette[COLOR_NUMBER];
	static const color WHITE;

	int codeSideSize = 1000; //amount of pixels in code side
	int codeSide; //amount of modules in code side
	int moduleSideSize; //amount of pixels in module side

	Coder();
	Coder(int modulesNumber);
	void setModulesNumber(int modulessNumber);
	int getModulesNumber();
	void setErrorCorrector(ErrorCorrector errorCorrector);
	ErrorCorrector getErrorCorrector();
	string getBinaryAsString(int value);
	int getDecimalFromBinaryString(string value);
};