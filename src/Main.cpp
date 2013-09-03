#include <iostream>
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

int main(int argc, char **argv) {
	try {
		if( argc < 3 ) {
			std::cerr << "Syntax: ssim orig_file.mkv stream_file.mp4\n";
			return 1;
		}

		av_register_all();
		avcodec_register_all();

		VideoFile file_orig(argv[1]);
		VideoFile file_transformed(argv[2]);
		Decoder decoder_orig(file_orig);
		Decoder decoder_transformed(file_transformed);

		int frame_no = 0;
		for( int i = 0; ; i++ ) {
			std::unique_ptr<RawFrame> frame_orig = file_orig.fetchRawFrame();
			std::unique_ptr<RawFrame> frame_transformed = file_transformed.fetchRawFrame();

			check_frame(frame_orig.get());
			check_frame(frame_transformed.get());

			std::unique_ptr<Bitmap> bmp_orig = decoder_orig.decode(frame_orig.get());
			std::unique_ptr<Bitmap> bmp_transformed = decoder_transformed.decode(frame_transformed.get());
			for(;;) {
				if( !bmp_orig && !bmp_transformed ) {
					frame_orig = file_orig.fetchRawFrame();
					frame_transformed = file_transformed.fetchRawFrame();

					check_frame(frame_orig.get());
					check_frame(frame_transformed.get());

					bmp_orig = decoder_orig.decode(frame_orig.get());
					bmp_transformed = decoder_transformed.decode(frame_transformed.get());
				} else if( bmp_orig && !bmp_transformed ) {
					frame_transformed = file_transformed.fetchRawFrame();
					check_frame(frame_transformed.get());
					bmp_transformed = decoder_transformed.decode(frame_transformed.get());
				} else if( !bmp_orig && bmp_transformed ) {
					frame_orig = file_orig.fetchRawFrame();
					check_frame(frame_orig.get());
					bmp_orig = decoder_orig.decode(frame_orig.get());
				} else {
					frame_no += 1;
					break;
				}
			}
			if( !bmp_orig->hasSameDimensions(*bmp_transformed) ) {
				std::unique_ptr<Bitmap> bmp3{ bmp_transformed->scale(bmp_orig->getWidth(), bmp_orig->getHeight()) };
				bmp_transformed.swap(bmp3);
			}

			double ssim = bmp_orig->SSIM(*bmp_transformed);
			printf( "%i %li %.06f\n", frame_no, frame_transformed->getPos(), ssim );
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

