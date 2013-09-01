#ifndef _VIDEOFILE_HPP
#define _VIDEOFILE_HPP

#include "libav.hpp"
#include "RawFrame.hpp"

class VideoFile {
	public:
		VideoFile(const char *filename);
		~VideoFile();

		RawFrame *fetchRawFrame();

	private:
		AVFormatContext *ctx;
		int vstream;
};

#endif
