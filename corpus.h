#ifndef __CORPUS_H
#define __CORPUS_H

#include "vocabulary.h"

struct Corpus 
{
	std::vector<std::vector<int>> docs;
    void ComputeStatistics();

	int D;
	long long T;
};

#endif
