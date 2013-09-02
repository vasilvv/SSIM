#ifndef _BITMAP_HPP
#define _BITMAP_HPP

#include "libav.hpp"

class Bitmap {
	public:
		Bitmap(AVFrame *frame);
		~Bitmap();

		double SSIM(Bitmap &other);
	private:
		AVPicture picture;
		PixelFormat pix_fmt;
		int width, height;
		bool needs_to_free;
};

#endif
