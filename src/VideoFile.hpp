#ifndef _VIDEOFILE_HPP
#define _VIDEOFILE_HPP

#include "libav.hpp"
#include "RawFrame.hpp"

#include <memory>

class Decoder;

class VideoFile {
	friend class Decoder;

	public:
		VideoFile(const char *filename);
		~VideoFile();

		std::unique_ptr<RawFrame> fetchRawFrame();

	private:
		AVFormatContext *ctx;
		int vstream;
};

#endif
