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
			std::unique_ptr<RawFrame> frame1 = file1.fetchRawFrame();
			std::unique_ptr<RawFrame> frame2 = file2.fetchRawFrame();
			if( frame1->getPos() == -1 || frame2->getPos() == -1 ) {
				std::cout << "Found a frame for which libav fails to identify position\n";
				return 1;
			}
			std::unique_ptr<Bitmap> bmp1 = decoder1.decode(frame1.get());
			std::unique_ptr<Bitmap> bmp2 = decoder2.decode(frame2.get());
			for(;;) {
				if( !bmp1 && !bmp2 ) {
					frame1 = file1.fetchRawFrame();
					frame2 = file2.fetchRawFrame();
					bmp1 = decoder1.decode(frame1.get());
					bmp2 = decoder2.decode(frame2.get());
				} else if( bmp1 && !bmp2 ) {
					frame2 = file2.fetchRawFrame();
					bmp2 = decoder2.decode(frame2.get());
				} else if( !bmp1 && bmp2 ) {
					frame1 = file1.fetchRawFrame();
					bmp1 = decoder1.decode(frame1.get());
				} else {
					frame_no += 1;
					break;
				}
			}
			printf( "%i %li\n", frame_no, frame1->getPos() );
			if( !bmp1->hasSameDimensions(*bmp2) ) {
				std::unique_ptr<Bitmap> bmp3{ bmp2->scale(bmp1->getWidth(), bmp1->getHeight()) };
				bmp2.swap(bmp3);
			}
			ssim += bmp1->SSIM(*bmp2);
			printf( "SSIM: %.05f\n", ssim / frame_no );
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

