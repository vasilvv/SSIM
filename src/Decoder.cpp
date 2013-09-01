#include "Decoder.hpp"

Decoder::Decoder(VideoFile &file) {
	int error;
	AVDictionary *opts = NULL;

	codec = avcodec_find_decoder(CODEC_ID_H264);
	ctx = file.ctx->streams[file.vstream]->codec;

	error = avcodec_open2(ctx, codec, &opts);
	if( error < 0 ) {
		throw LibavError(error);
	}
}

Bitmap *Decoder::decode(RawFrame *input) {
	AVFrame *frame = avcodec_alloc_frame();
	int got_picture;
	int error;
	Bitmap *result = NULL;

	error = avcodec_decode_video2(ctx, frame, &got_picture, &input->packet);
	if( error < 0 ) {
		throw LibavError(error);
	}

	if( !got_picture ) {
		goto exit;
	}

	result = new Bitmap(frame);
exit:
	av_free(frame);
	return result;
}
