#ifndef __FRAME_H
#define __FRAME_H

#include "Bitmap.hpp"

class Frame : public Bitmap {
	public:
		Frame(AVFrame *frame);
		~Frame();

		inline int getPresentationTimestamp() {
			return frame->pkt_pts;
		}
	protected:
		AVFrame *frame;
};

#endif

