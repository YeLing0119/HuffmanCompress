#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "HuffmanTree.hpp"

struct CharInfo{
  
	unsigned char _char;
    unsigned long long count;
    std::string trans_string;

	CharInfo(unsigned long long _count = 0):count(_count){}

    CharInfo operator+(const CharInfo &Info){
       return CharInfo(count + Info.count);
    }

    bool operator > (const CharInfo &Info){
        return count > Info.count;
    }
    bool operator != (const CharInfo &Info)const{
        return count != Info.count;
    }
};

class HuffmanCompress{
    private:
        std::vector<CharInfo> trans_info;
    public:
        HuffmanCompress(){
            trans_info.resize(256);
            for(int i = 0; i < 256; ++i){
                trans_info[i]._char = i;
                trans_info[i].count = 0;
            }
        }
        
        void FileCompress(const std::string &in_file_path, const std::string &out_file_path){
            // 1. 读取源文件，统计每个字符出现的次数

            FILE *FileIn = fopen(in_file_path.c_str(), "rb");
            if(FileIn == NULL){
                std::cout << "Open file error" << std::endl;
                return;
            }
            
            unsigned char* pReadBuff = new unsigned char[1024];  // 定义一个1K的缓冲区

            while(true){
                size_t ret = fread(pReadBuff, 1, 1024, FileIn);     // 从文件中一次读取1K数据
                if(ret == 0){
                    break;
                }
                for(size_t i = 0; i < ret; ++i){
                    std::cout << (char)pReadBuff[i];
                    trans_info[pReadBuff[i]].count++;
                } 
            }

            // 2. 创建Huffman树

            HuffmanTree<CharInfo> hf;
            hf.CreateHuffmanTree(trans_info, CharInfo(0));
            // 3. 通过Huffman树，找到对应的编码
            GetHuffmanCode(hf.GetRoot()); 

            // 4. 用每个字符的新编码重新对文件进行改写
            
            unsigned char ch = 0;
            int bitcount = 0;
            fseek(FileIn, 0, SEEK_SET);
            FILE *FileOut = fopen(out_file_path.c_str(), "wb");
			
			WriteDeCompressInfo(FileOut, out_file_path, CharInfo(0));

            while(true){
                size_t readSize = fread(pReadBuff, 1, 1024, FileIn);
                if(readSize == 0){
                    break;
                }

                for(size_t i = 0; i < readSize; ++i){
                    std::string &tranStr = trans_info[pReadBuff[i]].trans_string;
                    for(size_t j = 0; j < tranStr.size(); ++j){
                        ch <<= 1;
                        if(tranStr[j] == '1'){
                            ch |= 1;
                        }

                        bitcount++;

                        if(bitcount == 8){
                            fputc(ch, FileOut); 
                            ch = 0;
                            bitcount = 0;
                        }
                    }
                }
            }


			if(bitcount < 8 && bitcount > 0){
				ch <<= 8 - bitcount;
				fputc(ch, FileOut);
			}

            delete[] pReadBuff;
            fclose(FileIn);
            fclose(FileOut);
        }

        void FileDeCompress(const std::string &in_file_path, const std::string &out_file_path){
			FILE *FileIn = fopen(in_file_path.c_str(), "rb");
			if (FileIn == nullptr) {
				std::cout << "Open File Error" << std::endl;
			}
			// 从压缩文件中读取后缀
			 std::string suffix = "";
			 suffix = GetOneLineFromFile(FileIn); 
			// std::string path = "a";
			// path += suffix;

			// 读取附加行长度
			int line_count = 0;
			line_count = atoi(GetOneLineFromFile(FileIn).c_str());

			// 读取附加行，并将每个字符的权值放入到trans_info中
			std::string line;
			std::string Character;
			std::string weight;
			for(int i = 0; i < line_count; i++){
				line = GetOneLineFromFile(FileIn);
				if(line == ""){
					line = GetOneLineFromFile(FileIn);
					weight = line.substr(line.find(':') + 1);
					trans_info['\n'].count = atoi(weight.c_str());
					continue;
				}

				unsigned char character = line[0];
				weight = line.substr(2);
				
				trans_info[character].count = atoi(weight.c_str());
			}

			// 根据权值创建huffman树
			HuffmanTree<CharInfo> hf;
			hf.CreateHuffmanTree(trans_info, CharInfo(0));

			// 根据Huffman获取每个字符的编码
			GetHuffmanCode(hf.GetRoot());

			unsigned long long fileSize = hf.GetRootWeight();
			unsigned long long sizeCount = 0;

			FILE *FileOut = fopen(out_file_path.c_str(), "wb");
			if (FileOut == nullptr) {
				std::cout << "Open File Error" << std::endl;
			}

			unsigned char *pReadBuff = new unsigned char[1024];

			std::string decompress;

			HuffTreeNode<CharInfo>* pCur = hf.GetRoot();
			while(true){
				size_t readSize = fread(pReadBuff, 1, 1024, FileIn);
				for(size_t i = 0; i < readSize; ++i){
					unsigned char ch = pReadBuff[i];

					for(int j = 0 ; j < 8; ++j){
						if( (ch & (1 << (7 - j))) != 0 ){
							pCur = pCur->pRight;
						}else{
							pCur = pCur->pLeft;
						}

						// 向缓冲区中写入字符
						if(pCur->pLeft == nullptr && pCur->pRight == nullptr){
							decompress += pCur->huffWeight._char;
							pCur = hf.GetRoot();	
							sizeCount++;
						}

						if(decompress.size() == 8){
							fwrite(decompress.c_str(), 1, 8, FileOut);
							decompress.clear();
						}

						if (sizeCount == fileSize) {
							if(decompress.size() > 1 && decompress.size() < 8){
								fwrite(decompress.c_str(), 1, decompress.size(), FileOut);
							}

							delete[] pReadBuff;
							fclose(FileIn);
							fclose(FileOut);
							return;
						}
					}
				}
			}
        }

		
        ~HuffmanCompress(){};

    private:
        void GetHuffmanCode(HuffTreeNode<CharInfo>* pRoot){
            if(pRoot == nullptr)
                return;

            GetHuffmanCode(pRoot->pLeft);
            GetHuffmanCode(pRoot->pRight);

            if(pRoot->pLeft == nullptr && pRoot->pRight == nullptr){
                HuffTreeNode<CharInfo>* pCur = pRoot; 
                HuffTreeNode<CharInfo>* pParent = pRoot->pParent;
                
                std::string &tranStr = trans_info[pRoot->huffWeight._char].trans_string;

                while(pParent){
                    if(pCur == pParent->pLeft){
                        tranStr += '0';
                    }else{
                        tranStr += '1';
                    }
                    pCur = pParent;
                    pParent = pCur->pParent;
                }

                reverse(tranStr.begin(), tranStr.end());
            }

        }

		void WriteDeCompressInfo(FILE* FilePtr, std::string path, CharInfo invaild){
			std::string addtionfield = "";

			std::string suffix = path.substr(path.rfind('.'));

			addtionfield += suffix;
			addtionfield += '\n';

			std::string decompressinfo = "";

			int count = 0;
			for(int i = 0; i < 256; i++){
				if(trans_info[i] != invaild){
					decompressinfo += trans_info[i]._char;

                    std::stringstream ss;
                    ss << trans_info[i].count;
                    std::string weight;
                    ss >> weight;

					decompressinfo += ':';
					decompressinfo += weight;
					decompressinfo += '\n';
					count++;
				}
			}

            std::stringstream ss;
            ss << count;
            std::string weight;
            ss >> weight;

			addtionfield += weight;
			addtionfield += '\n';
			addtionfield += decompressinfo;

			fwrite(addtionfield.c_str(), 1, addtionfield.size(), FilePtr);
		}

		std::string GetOneLineFromFile(FILE *file){
			char ch;
			std::string line ;
			while((ch = fgetc(file)) != '\n'){
				if (!feof(file)) {
					line += ch;
				}
			}
			return line;
		}
};
