#ifndef _RAWFRAME_HPP
#define _RAWFRAME_HPP

#include "libav.hpp"

class VideoFile;
class Decoder;

class RawFrame {
    friend class VideoFile;
    friend class Decoder;

	public:
		inline RawFrame() {
			memset(&packet, 0, sizeof(AVPacket));
		}
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

		inline bool isKeyframe() {
			return (packet.flags & AV_PKT_FLAG_KEY) != 0;
		}

	private:
		AVPacket packet;
};

#endif
