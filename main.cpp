#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>
#include <ios>

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

void benchmark(WordList &words)
{
    std::vector<int> times1;
    std::vector<int> times2;
    std::vector<int> times3;
    std::vector<int> times4;
    std::vector<int> times5;
    int repeats = 10;
    int checksum = 0;
    for (int r = 0; r < repeats; r++)
    {
        Trie trie1;
        TrieArray trie2(words);
        StaticTrieGraph<TrieEdge> static_trie1(words);
        StaticTrieGraph<CompressedTrieEdge> static_trie2(words);
        auto build_trie = [&]()
        {
            for (auto &s : words)
            {
                trie1.insert(s);
            }
        };
        auto trie_query = [&]()
        {
            bool b = false;
            for (auto &s : words)
            {
                b ^= trie1.contains_word(s);
            }
            checksum += b;
        };
        auto array_trie_query = [&]()
        {
            bool b = false;
            for (auto &s : words)
            {
                b ^= trie2.contains_word(s);
            }
            checksum += b;
        };
        auto static_trie_query1 = [&]()
        {
            bool b = false;
            for (auto &s : words)
            {
                b ^= static_trie1.contains_word(s);
            }
            checksum += b;
        };
        auto static_trie_query2 = [&]()
        {
            bool b = false;
            for (auto &s : words)
            {
                b ^= static_trie2.contains_word(s);
            }
            checksum += b;
        };

        int time1 = measureTimeMs(build_trie);
        int time2 = measureTimeMs(trie_query);
        int time3 = measureTimeMs(array_trie_query);
        int time4 = measureTimeMs(static_trie_query1);
        int time5 = measureTimeMs(static_trie_query2);
        times1.push_back(time1);
        times2.push_back(time2);
        times3.push_back(time3);
        times4.push_back(time4);
        times5.push_back(time5);
        if (r == 0)
        {
            trie1.print_degree_statistic();
        }
    }
    std::string unit = "ms";
    // std::string unit = "microseconds";
    int avg_build_trie = mean(times1);
    int avg_query_trie = mean(times2);
    int avg_query_array_trie = mean(times3);
    int avg_query_static_trie = mean(times4);
    int avg_query_static_trie_compressed = mean(times5);
    std::cout << "build trie: " << avg_build_trie << " " << unit
              << "\n";
    std::cout << "look up trie: " << avg_query_trie << " " << unit
              << "\n";
    std::cout << "look up array trie: " << avg_query_array_trie << " " << unit
              << "\n";
    std::cout << "look up static trie: " << avg_query_static_trie << " " << unit
              << "\n";
    std::cout << "look up static compressed trie : " << avg_query_static_trie_compressed << " " << unit
              << "\n";
    std::cout << checksum << "\n";
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
            std::cout << "avg visited nodes " << avg_visited_nodes << ", ";
            std::cout << "avg result size " << avg_result << ", ";
            std::cout << "length " << len << ": " << timeMs << "ms"
                      << "\n";
        }
    }
}

void color_print_test()
{
    std::string s = "TEST";
    WordleHint hint = {WordleHintChar::CORRECT_POSITION, WordleHintChar::DIFFERENT_POSITION, WordleHintChar::DOES_NOT_OCCUR, WordleHintChar::CORRECT_POSITION};
    print_colorful_hint(hint, s);
}

int main()
{

    std::cout << "Hello Wordle!"
              << "\n";

    // std::string file = "../dictionary_9030.txt";
    std::string file = "../dictionary_large.txt";
    WordList words = io::read_dictionary(file);
    std::cout << "number of words: " << words.size() << "\n";

    // benchmark(words);
    // benchmark_word_challenge(words);

    // WordChallenge wc(words);
    // std::string s = "abnormal";
    // CharCounter counter(s);
    // auto idx = wc.possible_words(counter);
    // print_indexed_words(idx, words);
    // std::cout << "found " << idx.size() << " words"
    //           << "\n";
}