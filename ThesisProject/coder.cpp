#include "coder.hpp"

ErrorCorrector Coder::errorCorrector = ErrorCorrector();

Coder::color Coder::palette[COLOR_NUMBER] = {
	{ 255, 0, 0 }, //red
	{ 0, 255, 0 }, //green
	{ 0, 0, 255 }, //blue
	{ 240, 50, 230 }, //magenta
	{ 70, 240, 240 }, //cyan
	{ 255, 225, 25 }, //yellow
	{ 145, 30, 180 }, //purple
	{ 245, 130, 49 } //orange
};

const Coder::color Coder::WHITE = { 255, 255, 255 };

Coder::Coder() {}

Coder::Coder(int modulesNumber)
{
	this->modulesNumber = modulesNumber;
	codeSide = sqrt(modulesNumber); //amount of modules in code side
	moduleSideSize = codeSideSize / codeSide; //amount of pixels in module side
}

void Coder::setModulesNumber(int modulesNumber)
{
	this->modulesNumber = modulesNumber;
	codeSide = sqrt(modulesNumber); //amount of modules in code side
	moduleSideSize = codeSideSize / codeSide; //amount of pixels in module side
}

int Coder::getModulesNumber()
{
	return this->modulesNumber;
}

void Coder::setErrorCorrector(ErrorCorrector errorCorrector)
{
	Coder::errorCorrector = errorCorrector;
}

ErrorCorrector Coder::getErrorCorrector()
{
	return Coder::errorCorrector;
}

string Coder::getBinaryAsString(int value)
{
	string binary = std::bitset<BIT_NUM>(value).to_string();
	return binary;
}

int Coder::getDecimalFromBinaryString(string value)
{
	int dec = 0, mask = 1;

	for (int i = value.length() - 1; i >= 0; i--) {
		if (value[i] == '1') {
			dec |= mask;
		}
		mask <<= 1;
	}

	return dec;
}