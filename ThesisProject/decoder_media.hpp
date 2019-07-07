#pragma once
#include "decoder.hpp"
#include <string.h>

namespace decoder {
	class DecoderMedia : public decoder::Decoder {
	public:
		DecoderMedia();
		std::string decode(int8_t* image);
	};
}