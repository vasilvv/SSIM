#include "RawFrame.hpp"

#include <iostream>
RawFrame::~RawFrame() {
    av_free_packet(&packet);
}

