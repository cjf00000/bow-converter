#include "vocabulary.h"

void Vocabulary::MergeFrom(const Vocabulary &other) {
    for (auto elem: other.wordList)
        Add(elem);
}
