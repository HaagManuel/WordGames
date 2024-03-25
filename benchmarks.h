#include <iostream>
#include <vector>
#include <string>
#include <numeric>

#include "common.h"
#include "trie.h"
#include "measure_time.h"
#include "io.h"
#include "small_map.h"
#include "graph.h"
#include "static_trie.h"
#include "word_challenge.h"
#include "wordle.h"

double mean(std::vector<int> &v)
{
    return (double)std::accumulate(v.begin(), v.end(), 0) / v.size();
}

template <typename TrieType>
void benchmark_trie_by_word_length(WordList &words, std::string trie_name)
{
    std::cout << trie_name << "\n";
    int seed = 0;
    srand(seed);
    int repeats = 10000;
    TrieType trie(words);
    std::vector<std::vector<int>> index_len = index_word_of_len(words);
    int min_len = 3;
    int min_words = 100;
    int checksum = 0;

    std::vector<int> indices;

    auto query = [&]()
    {
        for (auto i : indices)
        {
            checksum ^= trie.contains_word(words[i]);
        }
    };
    for (uint i = min_len; i < index_len.size(); i++)
    {
        int m = index_len[i].size();
        if (m >= min_words)
        {
            for (int r = 0; r < repeats; r++)
            {
                int j = index_len[i][rand() % m];
                indices.push_back(j);
            }
            double avg_time = (double)measureTimeMicroS(query) / repeats;
            std::cout << "query length " << i << ": " << avg_time << " microseconds \n";
        }
        indices.clear();
    }
    std::cout << checksum << "\n";
    std::cout << "\n";
}

void benchmark_word_challenge(WordList &words)
{
    int repeats = 1000;
    int seed = 0;
    srand(seed);
    WordChallenge wc(words);
    std::vector<std::vector<int>> index_len = index_word_of_len(words);
    int min_len = 3;

    int num_words = 0;
    auto run = [&](int len)
    {
        for (int i = 0; i < repeats; i++)
        {
            int j = rand() % index_len[len].size();
            int word_idx = index_len[len][j];
            CharCounter counter(words[word_idx]);
            auto result = wc.possible_words(counter);
            num_words += result.size();
        }
    };

    std::cout << "time word challenge: \n";
    for (uint len = min_len; len < index_len.size(); len++)
    {
        if (index_len[len].size() >= 100)
        {
            wc.reset_counter();
            num_words = 0;
            auto f = [&]()
            {
                run(len);
            };

            double timeMs = (double)measureTimeMs(f) / repeats;
            double avg_visited_nodes = wc.get_num_visited_nodes() / repeats;
            double avg_result = num_words / repeats;
            std::cout << "length " << len << ": " << timeMs << "ms";
            std::cout << "avg visited nodes " << avg_visited_nodes << ", ";
            std::cout << "avg result size " << avg_result << ", "
                      << "\n";
        }
    }
}