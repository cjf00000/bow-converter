#include "corpus.h"
#include "readbuf.h"
#include <iostream>
using namespace std;

void Corpus::ComputeStatistics() {
    T = 0;
    D = docs.size();
    for (auto &d: docs) T += d.size(); 
    cout << D << " documents, " << T << " tokens." << endl;
}
