#include "coder.hpp"

class DecodingUtil : public Coder {
	private:
		map<int, char> alphanumericValuesMap
		{
			std::make_pair(1, '0'),
			std::make_pair(2, '1'),
			std::make_pair(3, '2'),
			std::make_pair(4, '3'),
			std::make_pair(5, '4'),
			std::make_pair(6, '5'),
			std::make_pair(7, '6'),
			std::make_pair(8, '7'),
			std::make_pair(9, '8'),
			std::make_pair(10, '9'),
			std::make_pair(11, 'A'),
			std::make_pair(12, 'B'),
			std::make_pair(13, 'C'),
			std::make_pair(14, 'D'),
			std::make_pair(15, 'E'),
			std::make_pair(16, 'F'),
			std::make_pair(17, 'G'),
			std::make_pair(18, 'H'),
			std::make_pair(19, 'I'),
			std::make_pair(20, 'J'),
			std::make_pair(21, 'K'),
			std::make_pair(22, 'L'),
			std::make_pair(23, 'M'),
			std::make_pair(24, 'N'),
			std::make_pair(25, 'O'),
			std::make_pair(26, 'P'),
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

	public:
		DecodingUtil();
		DecodingUtil(int modulesNumber);
		string decode(string bitStream);
		bool checkErrorCorrection(string message, vector<int> codeword);
		int getPaletteIndex(std::vector<Coder::color> palette, std::vector<vector<double>> paletteBgrProportions, Coder::color currentColor);
		std::string getBitStreamFrom2DCode(Mat code);
		Mat perspectiveTransform(Mat input, Point2f* inputQuad);
};