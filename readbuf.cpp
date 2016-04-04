#include <iostream>

#include "readbuf.h"

ReadBuf::ReadBuf(const char *fileName, size_t capacity): capacity(capacity), buff(capacity), fin(fileName), size_processed(0) {
	if (!fin) {
		std::cerr << "Failed opening file: " << fileName << std::endl;
		exit(-1);
	}
}

/*
template<class T>
void ReadBuf::Scan(T operation) {
	size_t used = capacity;
	while (Fill(used)) {
		used = Process(operation);
	}
	Process(operation);
}
*/

bool ReadBuf::Fill(size_t used) {
	buff.erase(buff.begin(), buff.begin()+used);
	buff.resize(capacity);
	size_t reserve = capacity - used;
	bool success = !(fin.read(buff.data()+reserve, used).fail());
	size = reserve + fin.gcount();
	buff.resize(size);
	return success;
}

