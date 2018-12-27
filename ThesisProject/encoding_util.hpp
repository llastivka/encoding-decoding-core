#include "coder.hpp"

class EncodingUtil : public Coder {
private:

public:
	EncodingUtil();
	EncodingUtil(int modulesNumber);
	string encode(string text);
	string encodeErrorCorrectionCodeword(vector<int> codeword);
	Mat create2DCode(string bitString, int moduleSideSize);
};