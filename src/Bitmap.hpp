#ifndef _BITMAP_HPP
#define _BITMAP_HPP

#include "libav.hpp"

class Bitmap {
	public:
		Bitmap(AVFrame *frame);
		Bitmap(int width, int height, PixelFormat pix_fmt);
		~Bitmap();

		Bitmap* scale(int new_width, int new_height);
		double SSIM(Bitmap &other);
		uint32_t CRC32(bool include_chroma);

		inline int getWidth() { return width; }
		inline int getHeight() { return height; }
		inline int hasSameDimensions(Bitmap &bmp2) {
			return width == bmp2.width && height == bmp2.height;
		}
	protected:
		AVPicture picture;
		PixelFormat pix_fmt;
		int width, height;
};

#endif
