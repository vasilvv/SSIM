#ifndef _RAWFRAME_HPP
#define _RAWFRAME_HPP

#include "libav.hpp"

class VideoFile;
class Decoder;

class RawFrame {
    friend class VideoFile;
    friend class Decoder;

	public:
        ~RawFrame();

        inline uint8_t *getData() {
            return packet.data;
        }

        inline size_t getSize() {
            return packet.size;
        }

		inline int64_t getPos() {
			return packet.pos;
		}

	private:
		AVPacket packet;
};

#endif
