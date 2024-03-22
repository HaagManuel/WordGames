#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>

#include "trie.h"
#include "measure_time.h"
#include "io.h"

void benchmark(std::vector<std::string> &words)
{

    std::vector<int> times1;
    std::vector<int> times2;
    int repeats = 10;
    int checksum = 0;
    for (int r = 0; r < repeats; r++)
    {
        Trie trie;
        auto construction = [&]()
        {
            for (auto &s : words)
            {
                trie.insert(s);
            }
        };
        auto pos_lu = [&]()
        {
            bool b = false;
            for (auto &s : words)
            {
                b ^= trie.contains_word(s);
            }
            checksum += b;
        };
        
        // int time1 = measureTimeMicroS(construction);
        // int time2 = measureTimeMicroS(pos_lu);
        int time1 = measureTimeMs(construction);
        int time2 = measureTimeMs(pos_lu);
        times1.push_back(time1);
        times2.push_back(time2);

        // 1027817 nodes for large dictionary
        // std::cout << trie.nodes.size() << "\n";
    }
    std::string unit = "ms";
    // std::string unit = "microseconds";
    int avg_construction = std::accumulate(times1.begin(), times1.end(), 0) / repeats;
    int avg_pos_lu = std::accumulate(times2.begin(), times2.end(), 0) / repeats;
    std::cout << "construction: " << avg_construction << " " << unit
              << "\n";
    std::cout << "lookup: " << avg_pos_lu << " " << unit
              << "\n";
    std::cout << checksum << "\n";
}

int main()
{
    std::cout << "Hello Wordle!"
              << "\n";

    // paths are relative to build directory
    // std::string file = "../dictionary_9030.txt";
    std::string file = "../dictionary_large.txt";
    auto words = read_dictionary(file);
    std::cout << "num words: " << words.size() << "\n";
    benchmark(words);
}