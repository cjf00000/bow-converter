#include "corpus.h"
#include "readbuf.h"
#include <iostream>
#include <algorithm>
using namespace std;

void Corpus::ComputeStatistics() {
    T = 0;
    D = docs.size();
    random_shuffle(docs.begin(), docs.end());
    for (auto &d: docs) T += d.size(); 
    cout << D << " documents, " << T << " tokens." << endl;
}
