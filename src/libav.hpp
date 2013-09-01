#ifndef _LIBAV_HPP
#define _LIBAV_HPP

extern "C" {
    #define __STDC_CONSTANT_MACROS
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
}

#include <string>

class LibavError {
    public:
        int err;

        inline LibavError(int err) {
            this->err = err;
        }

        inline std::string getErrorMessage() {
            char buffer[1024] = {};

            av_strerror(err, buffer, sizeof(buffer));
            return std::string(buffer);
        }
};

#endif
