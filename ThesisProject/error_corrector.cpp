#include "error_corrector.hpp"

#include "schifra_galois_field.hpp"
#include "schifra_galois_field_polynomial.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_error_processes.hpp"

ErrorCorrector::ErrorCorrector() {}

int ErrorCorrector::getLengthOfCodeword()
{
	return this->fec_length;
}

std::vector<int> ErrorCorrector::encode(string text)
{
	// Finite Field Parameters 
	const std::size_t field_descriptor = 8;
	const std::size_t generator_polynomial_index = 120;
	const std::size_t generator_polynomial_root_count = 8;

	// Reed Solomon Code Parameters
	const std::size_t code_length = 255;
	const std::size_t fec_length = 8;
	const std::size_t data_length = code_length - fec_length;

	// Instantiate Finite Field and Generator Polynomials
	const schifra::galois::field field
	(
		field_descriptor,
		schifra::galois::primitive_polynomial_size06,
		schifra::galois::primitive_polynomial06
	);

	schifra::galois::field_polynomial generator_polynomial(field);

	if (
		!schifra::make_sequential_root_generator_polynomial
		(
			field,
			generator_polynomial_index,
			generator_polynomial_root_count,
			generator_polynomial
		)
		)
	{
		std::cout << "Error - Failed to create sequential root generator!" << std::endl;
		return {};
	}

	// Instantiate Encoder and Decoder (Codec)
	typedef schifra::reed_solomon::encoder<code_length, fec_length, data_length> encoder_t;
	typedef schifra::reed_solomon::decoder<code_length, fec_length, data_length> decoder_t;

	const encoder_t encoder(field, generator_polynomial);
	const decoder_t decoder(field, generator_polynomial_index);

	// Pad message with nulls up until the code-word length
	text.resize(code_length, 0x00);

	// Instantiate RS Block For Codec 
	schifra::reed_solomon::block<code_length, fec_length> block;

	// Transform message into Reed-Solomon encoded codeword
	if (!encoder.encode(text, block))
	{
		std::cout << "Error - Critical encoding failure!" << std::endl;
		return {};
	}
		
	vector<int> codeword;
	for (int i = code_length - fec_length; i < code_length; i++)
	{
		codeword.push_back(block.data[i]);
	}
	return codeword;
}
bool ErrorCorrector::decode(string message, std::vector<int> codeword)
{
	/* Instantiate Finite Field and Generator Polynomials */
	const schifra::galois::field field
	(
		field_descriptor,
		schifra::galois::primitive_polynomial_size06,
		schifra::galois::primitive_polynomial06
	);

	schifra::galois::field_polynomial generator_polynomial(field);

	if (
		!schifra::make_sequential_root_generator_polynomial
		(
			field,
			generator_polynomial_index,
			generator_polynomial_root_count,
			generator_polynomial
		)
		)
	{
		std::cout << "Error - Failed to create sequential root generator!" << std::endl;
		return "";
	}

	// Instantiate Decoder (Codec)
	typedef schifra::reed_solomon::decoder<code_length, fec_length, data_length> decoder_t;

	const decoder_t decoder(field, generator_polynomial_index);

	// Instantiate RS Block For Codec 
	schifra::reed_solomon::block<code_length, fec_length> block;

	for (int i = 0; i < codeword.size(); i++)
	{
		block.data[code_length - fec_length + i] = codeword[i];
	}

	if (!decoder.decode(block))
	{
		std::cout << "Error - Critical decoding failure!" << std::endl;
		return "";
	}
	else if (!schifra::is_block_equivelent(block, message))
	{
		std::cout << "Error - Error correction failed!" << std::endl;
		return false;
	}

	return true;
}

