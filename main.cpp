#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>

#include "trie.h"
#include "measure_time.h"
#include "io.h"
#include "small_map.h"
#include "graph.h"
#include "static_trie.h"

double mean(std::vector<int> &v)
{
    return (double)std::accumulate(v.begin(), v.end(), 0) / v.size();
}

void benchmark(std::vector<std::string> &words)
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

int main()
{
    std::cout << "Hello Wordle!"
              << "\n";

    // std::string file = "../dictionary_9030.txt";
    std::string file = "../dictionary_large.txt";
    auto words = io::read_dictionary(file);
    std::cout << "number of words: " << words.size() << "\n";

    benchmark(words);
}