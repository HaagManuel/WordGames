#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <numeric>

#include "common.h"
#include "trie.h"
#include "measure_time.h"
#include "io.h"
#include "small_map.h"
#include "random.h"
#include "graph.h"
#include "static_trie.h"
#include "word_challenge.h"
#include "wordle.h"

// if vector is to short compute the mean of remaining vectors
std::vector<long long> component_wise_mean(std::vector<std::vector<int>> &v)
{
    int max_cols = 0;
    for (auto &vec : v)
    {
        max_cols = std::max(max_cols, (int)vec.size());
    }
    std::vector<long long> sums(max_cols, 0);
    std::vector<long long> cnt(max_cols, 0);
    for (auto &vec : v)
    {
        for (uint i = 0; i < vec.size(); i++)
        {
            sums[i] += vec[i];
            cnt[i]++;
        }
    }
    for (int i = 0; i < max_cols; i++)
    {
        sums[i] /= cnt[i];
    }
    return sums;
}

template <typename TrieType>
void benchmark_trie_by_word_length(WordList &words, std::string trie_name)
{
    std::cout << trie_name << "\n";

    int repeats = 10000;
    int seed = 0;
    int min_len = 3;
    int min_words = 100;
    int checksum = 0;

    RandomGenerator gen(seed);
    TrieType trie(words);
    std::vector<std::vector<int>> index_len = compute_index_word_of_len(words);

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
                int j = index_len[i][gen.random_index(m)];
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
    RandomGenerator gen(seed);
    WordChallenge wc(words);
    std::vector<std::vector<int>> index_len = compute_index_word_of_len(words);
    int min_len = 3;

    int num_words = 0;
    auto run = [&](int len)
    {
        for (int i = 0; i < repeats; i++)
        {
            int m = index_len[len].size();
            int j = gen.random_index(m);
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

void benchmark_wordle(WordList &words)
{
    int repeats = 1000;
    int max_guesses = 10;
    int seed = 123;
    int min_len = 3;
    int max_len = 40;
    // int min_len = 3;
    // int max_len = 4;
    WordleSimulation sim(words, max_guesses, seed);
    RandomWordGenerator word_gen(words, seed);

    for (int len = min_len; len <= max_len; len++)
    {
        if (word_gen.count_words_of_len(len) == 0)
        {
            continue;
        }
        auto word_sample = word_gen.n_random_words_of_len(repeats, len);
        auto run = [&]()
        {
            for (int i = 0; i < repeats; i++)
            {
                sim.play_one_round<false>(word_sample[i]);
                // sim.play_one_round<true>(word_sample[i]);
            }
        };
        int time_run = measureTimeMs(run);
        double avg_time = (double)time_run / repeats;
        std::string unit = "ms";
        auto [guesses, visited, canditates] = sim.get_log_data();
        double avg_guesses = mean(guesses);
        auto avg_visited = component_wise_mean(visited);
        auto avg_canditates = component_wise_mean(canditates);
        sim.reset_logging();
        std::cout << "length: " << len << "\n";
        std::cout << "avg time per game: " << avg_time << " " << unit << "\n";
        std::cout << "avg_guesses: " << avg_guesses << "\n";

        // print_vector(visited[0]);
        // print_vector(visited[1]);
        // print_vector(canditates[0]);
        // print_vector(canditates[1]);
        
        std::cout << "visited ";
        print_vector(avg_visited);
        std::cout << "canditates ";
        print_vector(avg_canditates);

        // for (auto x : avg_visited)
        // {
        //     std::cout << std::fixed << std::setprecision(0) << x << " ";
        // }
        // std::cout << "\n";
        // for (auto x : avg_canditates)
        // {
        //     std::cout << std::fixed << std::setprecision(0) << x << " ";
        // }
        std::cout << "\n";
        std::cout << "\n";
    }
}