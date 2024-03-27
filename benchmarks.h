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

template <typename TrieType>
void benchmark_trie_by_word_length(WordList &words, std::string trie_name)
{
    std::cout << trie_name << "\n";

    uint repeats = 10000;
    uint seed = 0;
    uint min_len = 3;
    uint min_words = 100;
    uint checksum = 0;

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
    for (uint len = min_len; len < index_len.size(); len++)
    {
        if (index_len[len].size() >= min_words)
        {
            indices = gen.n_random_elements(repeats, index_len[len]);
            double avg_time = (double)measureTimeMicroS(query) / repeats;
            std::cout << "query length " << len << ": " << avg_time << " microseconds \n";
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
    int min_len = 3;
    RandomWordGenerator gen_word(words, seed);
    WordChallenge wc(words);
    CharCounter counter;
    std::vector<std::vector<int>> index_len = compute_index_word_of_len(words);

    std::cout << "time word challenge: \n";
    for (uint len = min_len; len < index_len.size(); len++)
    {
        if (index_len[len].size() >= 100)
        {
            wc.reset_counter();
            int num_words = 0;
            auto sample_words = gen_word.n_random_words_of_len(repeats, len);
            auto run = [&]()
            {
                for (int i = 0; i < repeats; i++)
                {
                    counter.reset_counter();
                    counter.new_counter(sample_words[i]);
                    auto result = wc.possible_words(counter);
                    num_words += result.size();
                }
            };

            double timeMs = (double)measureTimeMs(run) / repeats;
            double avg_visited_nodes = wc.get_num_visited_nodes() / repeats;
            double avg_result = num_words / repeats;
            std::cout << "length " << len << ": " << timeMs << "ms"
                      << ", ";
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
        double avg_time = (double)measureTimeMs(run) / repeats;
        std::string unit = "ms";
        auto [guesses, visited, canditates] = sim.get_log_data();
        double avg_guesses = mean(guesses);
        auto avg_visited = component_wise_mean(visited);
        auto avg_canditates = component_wise_mean(canditates);
        sim.reset_logging();
        std::cout << "length: " << len << "\n";
        std::cout << "avg time per game: " << avg_time << " " << unit << "\n";
        std::cout << "avg_guesses: " << avg_guesses << "\n";

        std::cout << "visited ";
        print_vector(avg_visited);
        std::cout << "canditates ";
        print_vector(avg_canditates);

        std::cout << "\n";
        std::cout << "\n";
    }
}