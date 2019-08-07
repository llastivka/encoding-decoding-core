#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstddef>
#include <stdint.h>
#include <math.h>
#include <fstream>
#include <string>
#include <bitset>
//#include "error_corrector.hpp"

using namespace std;

class Coder {
private:
	int modulesNumber;
	//static ErrorCorrector errorCorrector;

	map<char, int> encodingAlphanumericValuesMap
	{
		std::make_pair('0', 0),
		std::make_pair('1', 1),
		std::make_pair('2', 2),
		std::make_pair('3', 3),
		std::make_pair('4', 4),
		std::make_pair('5', 5),
		std::make_pair('6', 6),
		std::make_pair('7', 7),
		std::make_pair('8', 8),
		std::make_pair('9', 9),
		std::make_pair('A', 10),
		std::make_pair('B', 11),
		std::make_pair('C', 12),
		std::make_pair('D', 13),
		std::make_pair('E', 14),
		std::make_pair('F', 15),
		std::make_pair('G', 16),
		std::make_pair('H', 17),
		std::make_pair('I', 18),
		std::make_pair('J', 19),
		std::make_pair('K', 20),
		std::make_pair('L', 21),
		std::make_pair('M', 22),
		std::make_pair('N', 23),
		std::make_pair('O', 24),
		std::make_pair('P', 25),
		std::make_pair('Q', 26),
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

	map<int, char> decodingAlphanumericValuesMap
	{
		std::make_pair(0, '0'),
		std::make_pair(1, '1'),
		std::make_pair(2, '2'),
		std::make_pair(3, '3'),
		std::make_pair(4, '4'),
		std::make_pair(5, '5'),
		std::make_pair(6, '6'),
		std::make_pair(7, '7'),
		std::make_pair(8, '8'),
		std::make_pair(9, '9'),
		std::make_pair(10, 'A'),
		std::make_pair(11, 'B'),
		std::make_pair(12, 'C'),
		std::make_pair(13, 'D'),
		std::make_pair(14, 'E'),
		std::make_pair(15, 'F'),
		std::make_pair(16, 'G'),
		std::make_pair(17, 'H'),
		std::make_pair(18, 'I'),
		std::make_pair(19, 'J'),
		std::make_pair(20, 'K'),
		std::make_pair(21, 'L'),
		std::make_pair(22, 'M'),
		std::make_pair(23, 'N'),
		std::make_pair(24, 'O'),
		std::make_pair(25, 'P'),
		std::make_pair(26, 'Q'),
		std::make_pair(27, 'R'),
		std::make_pair(28, 'S'),
		std::make_pair(29, 'T'),
		std::make_pair(30, 'U'),
		std::make_pair(31, 'V'),
		std::make_pair(32, 'W'),
		std::make_pair(33, 'X'),
		std::make_pair(34, 'Y'),
		std::make_pair(35, 'Z'),
		std::make_pair(36, ' '),
		std::make_pair(37, '$'),
		std::make_pair(38, '%'),
		std::make_pair(39, '*'),
		std::make_pair(40, '+'),
		std::make_pair(41, '-'),
		std::make_pair(42, '.'),
		std::make_pair(43, '/'),
		std::make_pair(44, ':'),
	};

	static const std::string HTTP_PROTOCOL;
	static const std::string HTTPS_PROTOCOL;
	//later add possibility to configure url shortener
	static const std::string DEFAULT_URL_SHORTENER;
	static const std::string DEFAULT_LINK_MARKER;

	static const int BITS_IN_MODULE = 3;

	static const int BIT_NUM = 11;
	static const int SHORT_BIT_NUM = 6;
	static const int BYTE_BIT_NUM = 8;

	static const int PALETTE_NUM = 8;
	static const int ANGLES_NUM = 4;
	struct color;
	static const int COLOR_NUMBER = 8;
	static color palette[COLOR_NUMBER];
	static const color WHITE;

	int codeSideSize; //amount of pixels in code side
	int codeSide; //amount of modules in code side
	int moduleSideSize = 100; //amount of pixels in module side

							  //void setErrorCorrector(ErrorCorrector errorCorrector);
							  //ErrorCorrector getErrorCorrector();
	string getBinaryAsString(int value);
	int getDecimalFromBinaryString(string value);

	//decoding specific methods
	string decode(string bitStream);
	bool checkErrorCorrection(string message, vector<int> codeword);
	int getPaletteIndex(std::vector<Coder::color> palette, std::vector<vector<double>> paletteBgrProportions, Coder::color currentColor);
	int getPaletteIndex(std::vector<cv::Vec3b> palette, cv::Vec3b currentPixelHSV);
	std::string getBitStreamFrom2DCode(cv::Mat code);
	cv::Mat perspectiveTransform(cv::Mat input, vector<cv::Point2i> inputQuad);
	cv::Mat perspectiveTransform(cv::Mat input);
	cv::Mat threasholdImage(cv::Mat img);
    vector<cv::Point2i> getAnglesFromImage(cv::Mat image);
	cv::Mat createMat(int8_t* image, int32_t rows, int32_t cols);
	std::string decodeMessageFromImage(int8_t* image, int32_t rows, int32_t cols, vector<int32_t> xInputQuad, vector<int32_t> yInputQuad);
	bool isLink(string decoded);


	//encoding specific methods
	std::string encode(std::string text);
	string encodeErrorCorrectionCodeword(vector<int> codeword);
	cv::Mat create2DCode(string bitString);
	bool isURL(string text);
	
public:
	Coder();
	Coder(int modulesNumber);
	void setModulesNumber(int modulessNumber);
	int getModulesNumber();

	cv::Mat encodeStringToMat(std::string text);
    std::string decodeStringFromMat(cv::Mat mat);

	vector<int> getCorners(cv::Mat mat);
	std::string decodeStringFromMatWithCorners(cv::Mat mat, vector<int> corners);
	
};