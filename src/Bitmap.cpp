#include "Bitmap.hpp"

#include <zlib.h>
#include <iostream>
#include <fstream>
#include <x264.h>

Bitmap::Bitmap(AVFrame *frame) {
	pix_fmt = static_cast<PixelFormat>(frame->format);
	if( pix_fmt != PIX_FMT_YUV420P ) {
		throw "Formats other than YUV 4:2:0 are not supported";
	}
	width = frame->width;
	height = frame->height;

	avpicture_alloc(&picture, pix_fmt, frame->width, frame->height);
	av_image_copy(picture.data, picture.linesize, const_cast<const uint8_t**>(frame->data), frame->linesize, pix_fmt, width, height);
}

Bitmap::Bitmap(int width, int height, PixelFormat pix_fmt) {
	if( pix_fmt != PIX_FMT_YUV420P ) {
		throw "Formats other than YUV 4:2:0 are not supported";
	}

	this->width = width;
	this->height = height;
	this->pix_fmt = pix_fmt;

	avpicture_alloc(&picture, pix_fmt, width, height);
}

Bitmap::~Bitmap() {
	avpicture_free(&picture);
}

Bitmap* Bitmap::scale(int new_width, int new_height) {
	Bitmap *result = new Bitmap(new_width, new_height, pix_fmt);
	struct SwsContext *context;

	if( new_width % 2 != 0 || new_height % 2 != 0 ) {
		throw "Dimensions of the image must be even";
	}

	result->picture.linesize[0] = new_width;
	result->picture.linesize[1] = new_width / 2;
	result->picture.linesize[2] = new_width / 2;

	context = sws_getCachedContext(NULL, width, height, pix_fmt,
	                                     new_width, new_height, pix_fmt,
	                                     SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(context, picture.data, picture.linesize, 0, height,
	                           result->picture.data, result->picture.linesize);
	sws_freeContext(context);

	return result;
}

/*  Window sizes  */
#define WS 8
#define WS2 64

/*
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
			int w_offset = linesize * w_i * WS + w_j * WS;
			int x = 0, xx = 0, y = 0, yy = 0, xy = 0;
			for( int i = 0; i < WS; i++ ) {
				int l_offset = w_offset + linesize*i;
				for( int j = 0; j < WS; j++ ) {
					int cur_this  = data_this [l_offset + j];
					int cur_other = data_other[l_offset + j];
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
*/

uint32_t Bitmap::CRC32(bool include_chroma) {
	uint32_t crc = crc32(0, Z_NULL, 0);

	crc = crc32(crc, picture.data[0], picture.linesize[0] * height);
	
	if( include_chroma ) {
		crc = crc32(crc, picture.data[1], width * height / 4);
		crc = crc32(crc, picture.data[2], width * height / 4);
	}

	return crc;
}

void Bitmap::dumpPGM(const char *filename) {
	std::ofstream file(filename);

	file << "P5\n";
	file << width << " " << height << "\n";
	file << "255\n";

	int linesize = picture.linesize[0];
	for( int i = 0; i < height; i++ ) {
		for( int j = 0; j < width; j++ ) {
			file << (int)picture.data[0][linesize*i + j];
			file << ((j == width - 1) ? "\n" : " ");
		}
	}

	file.close();
}

struct x264_pixel_function_t
{
  // 158 is the number of pointers in this internal x264 struct
  // this is obviously a bit fragile since x264 could add more
  void * ptrs[ 158 ];
};

// These are internal libx264 instructions so they aren't included in x264.h
extern "C"
{
  float x264_pixel_ssim_wxh( const x264_pixel_function_t * func, const uint8_t *pix1,
                             uintptr_t stride1, const uint8_t *pix2, uintptr_t stride2,
                             int width, int height, void *buf, int *cnt );

  void x264_pixel_init( int cpu, x264_pixel_function_t *pixf );

  uint32_t x264_cpu_detect( void );
}

x264_pixel_function_t init_pixel_function( void )
{
  x264_pixel_function_t pix_func;
  x264_pixel_init( x264_cpu_detect(), &pix_func );

  return pix_func;
}

x264_pixel_function_t x264_funcs = init_pixel_function();

double Bitmap::SSIM(Bitmap &other) {
	uint8_t *tmpbuf = new uint8_t[8192*1024];
	int count;
	double ssim;

	ssim = x264_pixel_ssim_wxh(&x264_funcs, picture.data[0], picture.linesize[0],
	                           other.picture.data[0], other.picture.linesize[0],
	                           width, height, tmpbuf+16, &count);

	delete tmpbuf;
	return ssim/count;
}

