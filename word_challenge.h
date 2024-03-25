#pragma once

#include <vector>
#include <string>
#include <cassert>

#include "graph.h"
#include "trie.h"
#include "common.h"

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

    inline int get_count(char c)
    {
        int i = char_to_int(c);
        return counter[i];
    }

    std::vector<int> counter;
};

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