#include <iostream>
#include <vector>
#include <cstdlib>
#include <regex>

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
		int64_t getFramePosition(int frameno);

	private:
		VideoFile *file = nullptr;
		Decoder *decoder = nullptr;
		int last_keyframe_pos = 0;
		int skipped = 0;
		bool eof = false;
		std::vector<int64_t> positions;
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
			// Possibly get skipped frames
			return this->read();
		}

		// Track the position of frame in file
		if( raw_frame->getPos() < 0 ) {
			std::cerr << "Found a frame for which libav fails to identify position\n";
			exit(1);
		}
		positions.push_back( raw_frame->getPos() );

		std::unique_ptr<Frame> frame = decoder->decode(raw_frame.get());
		if( frame == nullptr ) {
			skipped++;
			return this->read();
		} else {
			return frame;
		}
	} else {
		if( skipped ) {
			// Ask libav for the remaining frames which were skipped
			// due to frame reordering
			RawFrame raw_frame{};
			std::unique_ptr<Frame> frame = decoder->decode(&raw_frame);
			return frame;
		} else {
			return nullptr;
		}
	}
}

int64_t Reader::getFramePosition(int frameno) {
	return positions.at(frameno);
}

void save_frame(AVFrame *p_frame, int width, int height, std::string output_filename) {
	FILE *p_file; 
	p_file = fopen(output_filename.c_str(), "wb");
	if(p_file == NULL) return;
	fprintf(p_file, "P6\n%d %d\n255\n", width, height);
	for(int y = 0; y < height; y++){
		fwrite(p_frame->data[0]+y*p_frame->linesize[0], 1, width, p_file);
	}
	fclose(p_file);
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

		std::string stream_filename = argv[2];
		std::regex resolution_re ("[0-9]+x([0-9]+)");
        std::smatch resolution_sm;
        std::regex_search(stream_filename, resolution_sm, resolution_re);
        std::string resolution = resolution_sm[1];
        bool every_other_frame = false;
        if(resolution == "144") every_other_frame = true;

		for( int i = 0; ; i++ ) {
			std::unique_ptr<Frame> bmp_orig = orig.read();
			std::unique_ptr<Frame> bmp_transformed = transformed.read();

			if(every_other_frame) {
				std::unique_ptr<Frame> extra_frame = orig.read();
			}
			
			std::unique_ptr<Bitmap> bmp3 = nullptr;
			Bitmap *bmp2;

			if( !bmp_orig || !bmp_transformed ) {
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
			
			printf( "%i %.06f %s %i %li\n",
				i,
				ssim,
				bmp_transformed->getTypeStr(),
				bmp_transformed->getCodedPictureNumber(),
				transformed.getFramePosition(bmp_transformed->getCodedPictureNumber()) );	
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

