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

#define WS 8
#define WS2 64

inline static double window_average(uint8_t data[], int linesize, int w_i, int w_j){
	uint32_t avg = 0;
	for( int i = 0; i < WS; i++ ) {
		for( int j = 0; j < WS; j++ ) {
			avg += data[linesize*(w_i*WS + i) + w_j*WS + i];
		}
	}
	return (double)(avg) / WS2;
}

double Bitmap::SSIM(Bitmap &other) {
	// FIXME: this should be resized
	if( width != other.width || height != other.height ) {
		throw "The bitmaps currently have to be of the same size";
	}

	double coef_1 = 6.5025;   // (0.01 * 255)^2
	double coef_2 = 58.5225;  // (0.03 * 255)^2

	int w_width = width / WS;
	int w_height = height / WS;

	int linesize = picture.linesize[0];
	uint8_t *data_this = picture.data[0];
	uint8_t *data_other = other.picture.data[0];

	double ssim = 0;
	for( int w_i = 0; w_i < w_height; w_i++ ) {
		for( int w_j = 0; w_j < w_width; w_j++ ) {
			int x = 0, xx = 0, y = 0, yy = 0, xy = 0;
			for( int i = 0; i < WS; i++ ) {
				for( int j = 0; j < WS; j++ ) {
					int cur_this  = data_this [linesize*(w_i*WS + i) + w_j*WS + i];
					int cur_other = data_other[linesize*(w_i*WS + i) + w_j*WS + i];
					x += cur_this;
					xx += cur_this * cur_this;
					y += cur_other;
					yy += cur_other * cur_other;
					xy += cur_this * cur_other;
				}
			}

			double avg_this = (double) x / WS2;
			double avg_other = (double) y / WS2;
			double var_this = (double) (xx - x*x) / WS2;
			double var_other = (double) (yy - y*y) / WS2; 
			double covar = (double) (xy - x*y) / WS2;

			ssim += (2 * avg_this * avg_other + coef_1) * (2 * covar + coef_2) / (avg_this * avg_this + avg_other * avg_other + coef_1) / (var_this + var_other + coef_2);
		}
	}

	return ssim / w_height / w_width;
}

