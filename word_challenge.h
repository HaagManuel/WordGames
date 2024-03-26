#pragma once

#include <vector>
#include <string>
#include <cassert>

#include "graph.h"
#include "trie.h"
#include "common.h"
#include "measure_time.h"
#include "random.h"
#include "io.h"

struct WordChallenge
{
    using EdgeType = TrieEdge;
    // using EdgeType = CompressedTrieEdge;
    using WordList = std::vector<std::string>;

    WordChallenge(WordList &words, bool rearrange_graph = true)
    {
        Trie trie(words);
        AdjacencyList<EdgeType> adj_list = trie.extract_graph<EdgeType>();
        if (rearrange_graph)
        {
            graph = AdjacencyArray<EdgeType>::construct_with_dfs_order(adj_list);
        }
        else
        {
            graph = AdjacencyArray(adj_list);
        }
        node_to_word_index = StaticTrieGraph<EdgeType>(words).construct_node_to_word_index(words);
    }

    std::vector<int> possible_words(CharCounter &char_count)
    {
        // stores words of length l
        std::vector<std::vector<int>> words_of_length(100);

        std::string word = "";
        rec(words_of_length, char_count, word, 0);

        auto words = concat_vectors(words_of_length);
        return words;
    }

    void rec(std::vector<std::vector<int>> &words_of_length, CharCounter &counter, std::string &word, int v)
    {
        visited_nodes++;
        for (auto &e : graph.neighbors(v))
        {
            char c = e.get_letter();
            int w = e.get_id();
            bool is_word = e.is_word();

            // prune subtree
            if (counter.get_count(c) == 0)
            {
                continue;
            }

            word.push_back(c);
            counter.decrement(c);

            if (is_word)
            {
                int index = node_to_word_index[w];
                words_of_length[word.size()].push_back(index);
            }

            rec(words_of_length, counter, word, w);

            word.pop_back();
            counter.increment(c);
        }
    }

    void reset_counter() { visited_nodes = 0; }
    int get_num_visited_nodes() const { return visited_nodes; }

    AdjacencyArray<EdgeType> graph;
    std::vector<int> node_to_word_index;
    int visited_nodes;
};

struct WordChallengeApplication
{
    WordChallengeApplication(WordList &_words, int seed) : words(_words), word_challenge(words), word_gen(words, seed) {}

    void play_auto_mode(int repeats, int word_length)
    {
        CharCounter counter;
        std::vector<int> found_words;
        std::vector<int> word_cnt;
        word_cnt.reserve(repeats);

        int words_of_len = word_gen.count_words_of_len(word_length);
        if (words_of_len == 0)
        {
            std::cout << "there are no words of length " << word_length << "\n";
            return;
        }
        std::cout << "there are " << words_of_len << " words of length " << word_length << "\n";
        auto sample_words = word_gen.n_random_words_of_len(repeats, word_length);

        auto run = [&]()
        {
            for (int i = 0; i < repeats; i++)
            {
                counter.new_counter(sample_words[i]);
                found_words = word_challenge.possible_words(counter);
                word_cnt.push_back(found_words.size());
            }
        };
        double avg_time = (double)measureTimeMicroS(run) / repeats;
        double avg_words = mean(word_cnt);

        std::string unit = "microseconds";
        std::cout << "average CPU time to find words: " << avg_time << " " << unit << "\n";
        std::cout << "average words found           : " << avg_words << "\n";
    }

    void play_interactive()
    {
        CharCounter counter;
        while (true)
        {
            std::string input = io::get_user_input();
            if (!io::word_is_lower(input))
            {
                std::cout << input << " is not full lowercase \n";
                continue;
            }
            counter.new_counter(input);
            auto indices = word_challenge.possible_words(counter);
            std::cout << "\n";
            print_indexed_words(indices, words);
            std::cout << "\n"
                      << "found " << indices.size() << " words"
                      << "\n";
        }
    }

    WordList &words;
    WordChallenge word_challenge;
    RandomWordGenerator word_gen;
};