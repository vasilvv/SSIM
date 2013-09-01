#include <iostream>

#include "VideoFile.hpp"
#include "Decoder.hpp"

int main(int argc, char **argv) {
	try {
		int error;
		av_register_all();
		avcodec_register_all();

		VideoFile file(argv[1]);
		Decoder decoder(file);

		for( int i = 0; ; i++ ) {
			RawFrame *frame = file.fetchRawFrame();
			if( frame->getPos() == -1 ) {
				std::cout << "Found a frame for which libav fails to identify position\n";
				return 1;
			}
			Bitmap *bmp = decoder.decode(frame);
			if(bmp) delete bmp;
			printf( "%i %li\n", i, frame->getPos() );
			delete frame;
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

