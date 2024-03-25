#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <iomanip>

using WordList = std::vector<std::string>;

static constexpr int MAX_WORD_LEN = 100;
static constexpr int ALPHABET_SIZE = 26;

static const std::string RESET = "\033[0m";
static const std::string BLACK = "\033[30m";
static const std::string RED = "\033[31m";
static const std::string GREEN = "\033[32m";
static const std::string YELLOW = "\033[33m";
static const std::string BLUE = "\033[34m";
static const std::string MAGENTA = "\033[35m";
static const std::string CYAN = "\033[36m";
static const std::string WHITE = "\033[37m";

void color_print(std::string text, std::string color)
{
    std::cout << color << text << RESET;
}

void color_print(char c, std::string color)
{
    std::cout << color << c << RESET;
}

void print_word_statistics(WordList &words)
{
    std::vector<int> word_len_counter(100, 0);
    for (auto &s : words)
    {
        word_len_counter[s.size()]++;
    }
    while (word_len_counter.back() == 0)
        word_len_counter.pop_back();

    std::cout << "number of words of length"
              << "\n";
    for (uint i = 0; i < word_len_counter.size(); i++)
    {
        std::cout << word_len_counter[i] << " ";
    }
    std::cout << "\n";
    std::cout << "total: " << words.size() << "\n";
    std::cout << "\n";
}

template <typename T>
void print_vector(std::vector<T> &vec)
{
    for (auto &x : vec)
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

std::vector<std::vector<int>> compute_index_word_of_len(WordList &words)
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

void print_indexed_words(std::vector<int> &index, WordList &words)
{
    for (auto i : index)
    {
        std::cout << words[i] << "\n";
    }
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

struct CharCounter
{
    CharCounter() : counter(26) {}

    CharCounter(std::string &s) : counter(26)
    {
        for (char c : s)
        {
            increment(c);
        }
    }

    void reset_counter()
    {
        std::fill(counter.begin(), counter.end(), 0);
    }

    void new_counter(std::string &s)
    {
        reset_counter();
        for (char c : s)
        {
            increment(c);
        }
    }

    inline int char_to_int(char c) { return c - 'a'; }

    inline void decrement(char c)
    {
        int i = char_to_int(c);
        counter[i]--;
    }
    inline void increment(char c)
    {
        int i = char_to_int(c);
        counter[i]++;
    }

    inline void set_count(char c, int count)
    {
        int i = char_to_int(c);
        counter[i] = count;
    }
    inline int get_count(char c)
    {
        int i = char_to_int(c);
        return counter[i];
    }

    std::vector<int> counter;
};