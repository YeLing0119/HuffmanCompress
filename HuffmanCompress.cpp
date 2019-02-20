#define _CRT_SECURE_NO_WARNINGS
#include "HuffmanCompress.hpp"

int main(int argc, char* argv[]){

    if(argc != 4){
        std::cout << "Usage: Huffman [-c/-x] [sourse file] [destion file]" 
           << std::endl << "        -c Compress" << "       -x  Decompress" << std::endl;

        return -1;
    }

	std::string type = argv[1];
	std::string in_file_path = argv[2];
	std::string out_file_path = argv[3];
    
    HuffmanCompress hc;
	if(type == "-c"){
		hc.FileCompress(in_file_path, out_file_path);
	}else if(type == "-x"){
		hc.FileDeCompress(in_file_path, out_file_path);
	}

	return 0;
}
