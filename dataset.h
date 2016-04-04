#ifndef __DATASET_H
#define __DATASET_H

#include "corpus.h"
#include "vocabulary.h"

class DataSet
{
    public:
        void ReadFromDMLC(std::vector<std::string> files);
        void WriteToDMLC(std::vector<std::string> files);

        void ReadFromLibSVM(std::string vocabFile, std::vector<std::string> files);
        void WriteToLibSVM(std::string vocabFile, std::vector<std::string> files);

    private:   
        std::vector<Corpus> corpora;
        Vocabulary vocab;
};

#endif
