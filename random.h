#pragma once

#include <random>

struct RandomGenerator
{
    RandomGenerator() : gen(0) {}

    RandomGenerator(int seed) : gen(seed) {}

    int random_index(int n)
    {
        std::uniform_int_distribution<int> dis(0, n - 1);
        return dis(gen);
    }

    template <typename T>
    T random_element(std::vector<T> &vector)
    {
        assert(vector.size() > 0);
        return vector[random_index(vector.size())];
    }

    template <typename T>
    std::vector<T> n_random_elements(int n, std::vector<T> &vector)
    {
        std::vector<T> elements;
        elements.reserve(n);
        for (int i = 0; i < n; i++)
        {
            elements.push_back(random_element(vector));
        }
        return elements;
    }
    std::mt19937 gen;
};

struct RandomWordGenerator
{
    RandomWordGenerator(WordList &_words) : words(_words), words_of_len(compute_index_word_of_len(words)), gen(0) {}
    RandomWordGenerator(WordList &_words, int seed) : words(_words), words_of_len(compute_index_word_of_len(words)), gen(seed) {}

    int count_words_of_len(int len)
    {
        return words_of_len[len].size();
    }

    std::vector<std::string> n_random_words(int n)
    {
        std::vector<std::string> v;
        v.reserve(n);
        for (int i = 0; i < n; i++)
        {
            v.push_back(random_word());
        }
        return v;
    }

    std::vector<std::string> n_random_words_of_len(int n, int len)
    {
        std::vector<std::string> v;
        v.reserve(n);
        for (int i = 0; i < n; i++)
        {
            v.push_back(random_word_of_length(len));
        }
        return v;
    }

    std::string random_word()
    {
        return gen.random_element(words);
    }

    std::string random_word_of_length(int len)
    {
        int idx = gen.random_element(words_of_len[len]);
        return words[idx];
    }

    WordList &words;
    std::vector<std::vector<int>> words_of_len;
    RandomGenerator gen;
};