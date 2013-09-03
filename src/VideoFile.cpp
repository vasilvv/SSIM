#include "VideoFile.hpp"

VideoFile::VideoFile(const char *filename) {
	int error;

	ctx = avformat_alloc_context();
	error = avformat_open_input(&ctx, filename, NULL, NULL);
	if( error < 0 ) {
		throw LibavError(error);
	}

	vstream = -1; 
	for( int i = 0; i < ctx->nb_streams; i++ ) {
		if( ctx->streams[i]->codec->codec_id == CODEC_ID_H264 ) {
			vstream = i;
			break;
		}
	}
	if( vstream == -1 ) {
		throw "No H.264 stream found";
	}
}

VideoFile::~VideoFile() {
	if( ctx ) {
		avformat_close_input(&ctx);
	}
}

std::unique_ptr<RawFrame> VideoFile::fetchRawFrame() {
	int error;
	std::unique_ptr<RawFrame> frame{ new RawFrame() };

	for(;;) {
		error = av_read_frame(ctx, &frame->packet);
		if( error < 0 ) {
				throw LibavError(error);
		}
		if( frame->packet.stream_index == vstream ) {
			break;
		} else {
			av_free_packet(&frame->packet);
		}
	}

	return frame;
}
