#include <iostream>
#include <vector>
#include <cstdlib>

#include "VideoFile.hpp"
#include "Decoder.hpp"


void check_frame(RawFrame *frame) {
	// End of file
	if( !frame ) {
		exit(0);
	}

	if( frame->getPos() < 0 ) {
		std::cerr << "Found a frame for which libav fails to identify position\n";
		exit(1);
	}
}

class Reader {
	public:
		Reader(char *filename);
		// ~Reader(); -- FIXME
		std::unique_ptr<Frame> read();

		VideoFile *file = nullptr;
		Decoder *decoder = nullptr;
		int last_keyframe_pos = 0;
		int skipped = 0;
		bool eof = false;
};

Reader::Reader(char *filename) {
	file = new VideoFile(filename);
	decoder = new Decoder(*file);
}

std::unique_ptr<Frame> Reader::read() {
	if( !eof ) {
		std::unique_ptr<RawFrame> raw_frame = file->fetchRawFrame();
		if( !raw_frame ) {
			// End of file
			eof = true;
			return this->read();
		}
		if( raw_frame->getPos() < 0 ) {
			std::cerr << "Found a frame for which libav fails to identify position\n";
			exit(1);
		}

		std::unique_ptr<Frame> frame = decoder->decode(raw_frame.get());
		if( frame == nullptr ) {
			skipped++;
			return this->read();
		} else {
			return frame;
		}
	} else {
		if( skipped ) {
			RawFrame raw_frame{};
			std::unique_ptr<Frame> frame = decoder->decode(&raw_frame);
			return frame;
		} else {
			return nullptr;
		}
	}
}

int main(int argc, char **argv) {
	try {
		if( argc < 3 ) {
			std::cerr << "Syntax: ssim orig_file.mkv stream_file.mp4\n";
			return 1;
		}

		av_register_all();
		avcodec_register_all();

		Reader orig{argv[1]};
		Reader transformed{argv[2]};

		for( int i = 0; ; i++ ) {
			std::unique_ptr<Frame> bmp_orig = orig.read();
			std::unique_ptr<Frame> bmp_transformed = transformed.read();
			std::unique_ptr<Bitmap> bmp3 = nullptr;
			Bitmap *bmp2;

			if( !bmp_orig && !bmp_transformed ) {
				break;
			}

			if( !bmp_orig->hasSameDimensions(*bmp_transformed) ) {
				std::unique_ptr<Bitmap> bmp3_tmp{ bmp_transformed->scale(bmp_orig->getWidth(), bmp_orig->getHeight()) };
				bmp3.swap(bmp3_tmp);
				bmp2 = bmp3.get();
			} else {
				bmp2 = bmp_transformed.get();
			}

			double ssim = bmp_orig->SSIM(*bmp2);
			printf( "%i %.06f %s %s\n", i + 1, ssim, bmp_orig->getTypeStr(), bmp_transformed->getTypeStr() );
			std::cout.flush();
		}

	} catch( LibavError err ) {
		std::cerr << err.getErrorMessage() << "\n";
		return 1;
	} catch( std::string err ) {
		std::cerr << err << "\n";
		return 1;
	} catch( const char* err ) {
		std::cerr << err << "\n";
		return 1;
	}

	return 0;
}

