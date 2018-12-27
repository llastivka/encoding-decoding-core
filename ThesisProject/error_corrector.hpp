#include <cstddef>
#include <iostream>
#include <string>
#include <stdint.h>
#include <vector>

using namespace std;

class ErrorCorrector {
private:
	/* Finite Field Parameters */
	static const std::size_t field_descriptor = 8;
	static const std::size_t generator_polynomial_index = 120;
	static const std::size_t generator_polynomial_root_count = 8;

	/* Reed Solomon Code Parameters */
	static const std::size_t code_length = 255;
	static const std::size_t fec_length = 8;
	static const std::size_t data_length = code_length - fec_length;

public:
	ErrorCorrector();
	int getLengthOfCodeword();
	std::vector<int> encode(string text);
	bool decode(string message, std::vector<int> codeword);
};