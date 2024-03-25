#pragma once

#include <vector>
#include <string>
#include <cassert>

using WordList = std::vector<std::string>;

static constexpr int MAX_WORD_LEN = 100;
static constexpr int ALPHABET_SIZE = 26;

std::vector<std::vector<int>> index_word_of_len(WordList &words)
{
    std::vector<std::vector<int>> index_len(MAX_WORD_LEN + 1);

    for (uint i = 0; i < words.size(); i++)
    {
        assert(words[i].size() <= MAX_WORD_LEN);
        index_len[words[i].size()].push_back(i);
    }

    while (index_len.back().size() == 0)
        index_len.pop_back();
    return index_len;
}

template <typename T>
std::vector<T> concat_vectors(std::vector<std::vector<T>> &vectors)
{
    std::vector<T> merged;
    int total_size = 0;
    for (auto &vec : vectors)
    {
        total_size += vec.size();
    }
    merged.reserve(total_size);
    
    for (auto &vec : vectors)
    {
        merged.insert(merged.end(), vec.begin(), vec.end());
    }
    return merged;
}