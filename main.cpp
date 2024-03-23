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

void benchmark(std::vector<std::string> &words)
{

    std::vector<int> times1;
    std::vector<int> times2;
    std::vector<int> times3;
    int repeats = 10;
    int checksum = 0;
    for (int r = 0; r < repeats; r++)
    {
        Trie trie;
        auto build_trie = [&]()
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

        auto build_graph = [&]()
        {
            auto graph = trie.extract_graph();
            auto adj_array = AdjacencyArray<TrieEdge>(graph);
        };

        int time1 = measureTimeMs(build_trie);
        int time2 = measureTimeMs(pos_lu);
        int time3 = measureTimeMs(build_graph);
        times1.push_back(time1);
        times2.push_back(time2);
        times3.push_back(time3);

        if (r == 0)
        {
            trie.print_degree_statistic();
        }
        // 1027817 nodes for large dictionary
    }
    std::string unit = "ms";
    // std::string unit = "microseconds";
    int avg_build_trie = std::accumulate(times1.begin(), times1.end(), 0) / repeats;
    int avg_pos_lu = std::accumulate(times2.begin(), times2.end(), 0) / repeats;
    int avg_build_graph = std::accumulate(times3.begin(), times3.end(), 0) / repeats;
    std::cout << "build trie: " << avg_build_trie << " " << unit
              << "\n";
    std::cout << "lookup: " << avg_pos_lu << " " << unit
              << "\n";
    std::cout << "build graph: " << avg_build_graph << " " << unit
              << "\n";
    std::cout << checksum << "\n";
}

void order_test()
{
    Trie trie;
    std::string s1 = "aaa";
    std::string s2 = "bbb";
    trie.insert(s1);
    trie.insert(s2);

    auto adj_list = trie.extract_graph();
    auto graph = AdjacencyArray(adj_list);

    auto bfs = compute_bfs_order(graph, 0);
    auto dfs = compute_dfs_order(graph, 0);

    for (int v = 0; v < graph.num_nodes(); v++)
    {
        std::cout << "node: " << v << "\n";
        for (auto &e : graph.neighbors(v))
        {
            std::cout << e.id << " " << e.is_word << " " << e.letter << "\n";
        }
    }
    std::cout << "\n";

    for (auto x : bfs)
    {
        std::cout << x << "\n";
    }
    std::cout << "\n";

    for (auto x : dfs)
    {
        std::cout << x << "\n";
    }
    std::cout << "\n";
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