#ifndef __READBUF
#define __READBUF

#include <vector>
#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <omp.h>

class ReadBuf
{
public:
	size_t capacity;
	size_t size;
	std::vector<char> buff;
	std::ifstream fin;
	size_t size_processed;

	ReadBuf(const char *fileName, size_t capacity);
	template<class T>
	void Scan(T operation) {
		size_t used = capacity;
		while (Fill(used)) {
			used = Process(operation);
		}
		Process(operation);
	}

private:
	bool Fill(size_t used);
	template <class T>
	size_t Process(T operation) {
		// Find last '\n'
	//	char lf = '\n';
		int pos = -1;
		for (long long n = buff.size()-1; n>=0; n--) if (buff[n] == '\n') {
			pos = n;
			break;
		}
		if (pos == -1)
			throw std::runtime_error("input line too line");

		size_t used = pos + 1;

		// Find all occurence of '\n'
		int N = omp_get_max_threads();
		std::vector<std::vector<size_t>> crs_h(N);
		std::vector<size_t> allcr;
		allcr.push_back(-1);
		#pragma omp parallel
		{
			int id = omp_get_thread_num();
			size_t part = buff.size() / N;
			std::vector<size_t> &crs = crs_h[id];
			size_t beg = id * part;
			size_t end = (id+1 == N ? buff.size() : (id+1)*part);
			for (size_t n=beg; n<end; n++)
				if (buff[n] == '\n')
					crs.push_back(n);
		}

		for (int i=0; i<N; i++)
			allcr.insert(allcr.end(), crs_h[i].begin(), crs_h[i].end());

		int ndocs = allcr.size() - 1;
		#pragma omp parallel
		{
			#pragma omp for
			for (int i=0; i<ndocs; i++) {
				std::string doc = std::string(buff.begin()+allcr[i]+1, buff.begin()+allcr[i+1]);
				operation(doc);
			}
		}

		return used;
	}

};

#endif
