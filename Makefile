CC=g++
STD=-std=c++11

HuffmanCompress:HuffmanCompress.cpp
	$(CC) $^ -o $@ $(STD)

.PHONY:clean
clean:
	rm -f HuffmanCompress
