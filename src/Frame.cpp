#include "Frame.hpp"

Frame::Frame(AVFrame *frame) : Bitmap(frame) {
	this->frame = frame;
}

Frame::~Frame() {
	av_free(frame);
}

