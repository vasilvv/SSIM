#include <iostream>

#include "VideoFile.hpp"
#include "Decoder.hpp"

int main(int argc, char **argv) {
	try {
		av_register_all();
		avcodec_register_all();

		VideoFile file1(argv[1]);
		VideoFile file2(argv[2]);
		Decoder decoder1(file1);
		Decoder decoder2(file2);

		double ssim = 0;
		int frame_no = 0;
		for( int i = 0; ; i++ ) {
			RawFrame *frame1 = file1.fetchRawFrame();
			RawFrame *frame2 = file2.fetchRawFrame();
			if( frame1->getPos() == -1 || frame2->getPos() == -1 ) {
				std::cout << "Found a frame for which libav fails to identify position\n";
				return 1;
			}
			Bitmap *bmp1 = decoder1.decode(frame1);
			Bitmap *bmp2 = decoder2.decode(frame2);
			for(;;) {
				if( !bmp1 && !bmp2 ) {
					frame1 = file1.fetchRawFrame();
					frame2 = file2.fetchRawFrame();
					bmp1 = decoder1.decode(frame1);
					bmp2 = decoder2.decode(frame2);
				} else if( bmp1 && !bmp2 ) {
					frame2 = file2.fetchRawFrame();
					bmp2 = decoder2.decode(frame2);
				} else if( !bmp1 && bmp2 ) {
					frame1 = file1.fetchRawFrame();
					bmp1 = decoder1.decode(frame1);
				} else {
					frame_no += 1;
					break;
				}
			}
			printf( "%i %li\n", frame_no, frame1->getPos() );
			if( !bmp1->hasSameDimensions(*bmp2) ) {
				Bitmap *bmp3 = bmp2->scale(bmp1->getWidth(), bmp1->getHeight());
				delete bmp2;
				bmp2 = bmp3;
			}
			ssim += bmp1->SSIM(*bmp2);
			printf( "SSIM: %.05f\n", ssim / frame_no );
			delete bmp1;
			delete bmp2;
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

