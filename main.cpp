#include <iostream>
#include <stdexcept>
#include "dataset.h"
using namespace std;

int main(int argc, char **argv)
{
    std::vector<string> list1, list2;
    // Look for "outputformat"
    int pos = -1;
    for (int i=0; i<argc; i++)
        if (argv[i] == string("outputformat"))
            pos = i;
    if (pos==-1)
        throw invalid_argument("");

    // list1 = 2..pos-1
    for (int i=2; i<pos; i++)
        list1.push_back(argv[i]);

    DataSet dataset;
    dataset.ReadFromLibSVM(list1[1],
            vector<string>(list1.begin()+2, list1.end()));

    // list2 = pos+1..end
    for (int i=pos+1; i<argc; i++)
        list2.push_back(argv[i]);

    dataset.WriteToDMLC(vector<string>(list2.begin()+1, list2.end()));

    //// list1 = 2..pos-1
    //for (int i=2; i<pos; i++)
    //    list1.push_back(argv[i]);

    //DataSet dataset;
    //dataset.ReadFromDMLC(vector<string>(list1.begin()+1, list1.end()));

    //// list2 = pos+1..end
    //for (int i=pos+1; i<argc; i++)
    //    list2.push_back(argv[i]);

    //dataset.WriteToLibSVM(list2[1], 
    //        vector<string>(list2.begin()+2, list2.end()));

    return 0;
}
