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

void DataSet::WriteToDMLC(std::vector<std::string> files)
{
    int N = files.size();
    for (int i=0; i<N; i++) {
        ofstream fout(files[i]);
        auto &docs = corpora[i].docs;
        int M = docs.size();
        #pragma omp parallel for
        for (int d=0; d<M; d++) {
            ostringstream sout;
            auto &doc = docs[d];
            for (auto w: doc)
                sout << vocab.Get(w) << " ";
            sout << "\n";
            string s = sout.str();

            #pragma omp critical
            fout << s;
        }
    }
}

void DataSet::ReadFromLibSVM(std::string vocabFile, std::vector<std::string> files)
{
    // Read vocabulary
    ifstream vin(vocabFile);
    int id, tf;
    string word;
    while (vin >> id >> word >> tf)
        vocab.Add(word);

    // Read files
    ThreadBuffer<vector<int>> docBuffer;
    for (auto &f: files) {
        Corpus corpus;
        ReadBuf readBuf(f.c_str(), READ_BUF_CAPACITY);
        readBuf.Scan([&](std::string line) {
            for (auto &ch: line) if (ch==':') ch=' ';
            istringstream sin(line);
            auto &doc = docBuffer.Get();
            doc.clear();
            int idx, val, id;
            sin >> id;
            while (sin >> idx >> val)
                while (val--)
                    doc.push_back(idx);

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
        int doc_id = -1;
        #pragma omp parallel for
        for (int id = 0; id < M; id++) {
            ostringstream sout;
            auto &doc = docs[id];
            auto &comp = compressedBuffer.Get();
            comp.clear();
            for (auto elem: doc) comp[elem]++;
            if (comp.empty() || doc.size() > 1000)
                continue;

            #pragma omp critical
            doc_id++;
            sout << doc_id << "\t";
            for (auto elem: comp)
                sout << elem.first << ":" << elem.second << " ";
            sout << "\n";

            std::string out = sout.str();

            #pragma omp critical
            fout << out;
        }
    }
}
