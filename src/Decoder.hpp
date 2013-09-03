#ifndef _DECODER_HPP
#define _DECODER_HPP

#include "VideoFile.hpp"
#include "Frame.hpp"

class Decoder {
	public:
		Decoder(VideoFile &file);

		Frame *decode(RawFrame *input);

	private:
		AVCodecContext *ctx;
		AVCodec *codec;
};

#endif
