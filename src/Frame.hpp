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

		inline bool isKeyframe() {
			return frame->key_frame;
		}

		inline enum AVPictureType getType() {
			return frame->pict_type;
		}

		inline const char* getTypeStr() {
			switch( getType() ) {
				case AV_PICTURE_TYPE_I:  return "I";
				case AV_PICTURE_TYPE_P:  return "P";
				case AV_PICTURE_TYPE_B:  return "B";
				case AV_PICTURE_TYPE_S:  return "S";
				case AV_PICTURE_TYPE_SI: return "SI";
				case AV_PICTURE_TYPE_SP: return "SP";
				case AV_PICTURE_TYPE_BI: return "BI";
				default:                 return "???";
			}
		}
	protected:
		AVFrame *frame;
};

#endif

