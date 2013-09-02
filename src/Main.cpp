#include <iostream>

#include "VideoFile.hpp"
#include "Decoder.hpp"

int main(int argc, char **argv) {
	try {
		int error;
		av_register_all();
		avcodec_register_all();

		VideoFile file1(argv[1]);
		VideoFile file2(argv[2]);
		Decoder decoder1(file1);
		Decoder decoder2(file2);

		double ssim = 0;
		for( int i = 0; ; i++ ) {
			RawFrame *frame1 = file1.fetchRawFrame();
			RawFrame *frame2 = file2.fetchRawFrame();
			if( frame1->getPos() == -1 || frame2->getPos() == -1 ) {
				std::cout << "Found a frame for which libav fails to identify position\n";
				return 1;
			}
			Bitmap *bmp1 = decoder1.decode(frame1);
			Bitmap *bmp2 = decoder2.decode(frame2);
			printf( "%i %li\n", i, frame1->getPos() );
			if( bmp1 && bmp2 ) {
				ssim += bmp1->SSIM(*bmp2);
				printf( "SSIM: %.05f\n", ssim / i );
				delete bmp1;
				delete bmp2;
			}
			delete frame1;
			delete frame2;
		}

	} catch( LibavError err ) {
		std::cout << err.getErrorMessage() << "\n";
	} catch( std::string err ) {
		std::cout << err << "\n";
	} catch( const char* err ) {
		std::cout << err << "\n";
	}

	return 0;
}

