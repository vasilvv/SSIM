#ifndef _DECODER_HPP
#define _DECODER_HPP

#include "VideoFile.hpp"
#include "Bitmap.hpp"

class Decoder {
	public:
		Decoder(VideoFile &file);

		Bitmap *decode(RawFrame *input);

	private:
		AVCodecContext *ctx;
		AVCodec *codec;
};

#endif
