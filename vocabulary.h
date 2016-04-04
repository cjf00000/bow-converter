#ifndef __VOCABULARY_H
#define __VOCABULARY_H

#include <string>
#include <vector>
#include <unordered_map>

class Vocabulary
{
    public:
        int Size() { return wordMap.size(); }
        std::string Get(int id) { return wordList[id]; }
        int Get(std::string word)
        { 
            auto it = wordMap.find(word);
            return it==wordMap.end() ? -1 : it->second;
        }
        int Add(std::string word)
        {
            auto it = wordMap.find(word);
            int size = Size();
            if (it==wordMap.end()) {
                wordMap[word] = size;
                wordList.push_back(word);
                return size;
            } else 
                return it->second;
        }
        void MergeFrom(const Vocabulary &other);

    private:
        std::unordered_map<std::string, int> wordMap;
        std::vector<std::string> wordList;
};

#endif
