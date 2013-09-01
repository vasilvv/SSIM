#include "Bitmap.hpp"

#include <iostream>

Bitmap::Bitmap(AVFrame *frame) {
	pix_fmt = static_cast<PixelFormat>(frame->format);
	width = frame->width;
	height = frame->height;

	avpicture_alloc(&picture, pix_fmt, frame->width, frame->height);
	av_image_copy(picture.data, picture.linesize, const_cast<const uint8_t**>(frame->data), frame->linesize, pix_fmt, width, height);
}

Bitmap::~Bitmap() {
	avpicture_free(&picture);
}

