#include <sstream>
#include "dataset.h"
#include "readbuf.h"
#include "threadbuffer.h"
using namespace std;

constexpr int READ_BUF_CAPACITY = 1<<24;

void DataSet::ReadFromDMLC(std::vector<std::string> files)
{
    // Construct vocabulary
    ThreadBuffer<Vocabulary> vocabs;
    for (auto &f: files) {
        ReadBuf readBuf(f.c_str(), READ_BUF_CAPACITY);
        readBuf.Scan([&](std::string line) {
            Vocabulary &vocab = vocabs.Get();
            istringstream sin(line);
            string word;
            while (sin >> word)
                vocab.Add(word);
        });
    }
    for (auto &tVocab: vocabs) 
        vocab.MergeFrom(tVocab);
    cout << vocab.Size() << endl;

    // Parse
    ThreadBuffer<vector<int>> docBuffer;
    for (auto &f: files) {
        Corpus corpus;
        ReadBuf readBuf(f.c_str(), READ_BUF_CAPACITY);
        readBuf.Scan([&](std::string line) {
                istringstream sin(line);
                auto &doc = docBuffer.Get();
                doc.clear();
                string word;
                while (sin >> word)
                    doc.push_back(vocab.Get(word));

                #pragma omp critical
                corpus.docs.push_back(doc);
        });
        corpus.ComputeStatistics();
        corpora.push_back(corpus);
    }
}

void DataSet::WriteToLibSVM(std::string vocabFile, std::vector<std::string> files)
{
    // Write vocabulary
    ofstream vout(vocabFile);
    for (int i=0; i<vocab.Size(); i++)
        vout << i << "\t" << vocab.Get(i) << "\t0\n";

    ThreadBuffer<unordered_map<int, int>> compressedBuffer;
    // Write files
    int N = files.size();
    for (int i=0; i<N; i++) {
        ofstream fout(files[i]);
        auto &docs = corpora[i].docs;
        int M = docs.size();
        #pragma omp parallel for
        for (int id = 0; id < M; id++) {
            ostringstream sout;
            auto &doc = docs[id];
            auto &comp = compressedBuffer.Get();
            sout << id << "\t";
            comp.clear();
            for (auto elem: doc) comp[elem]++;
            for (auto elem: comp)
                sout << elem.first << ":" << elem.second << " ";
            sout << "\n";

            std::string out = sout.str();

            #pragma omp critical
            fout << out;
        }
    }
}
