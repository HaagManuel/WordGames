#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <cstdint>

#include "common.h"
#include "graph.h"
#include "trie.h"

template <typename EdgeType>
struct StaticTrieGraph
{
    StaticTrieGraph(WordList &words)
    {
        Trie trie(words);
        AdjacencyList<EdgeType> adj_list = trie.extract_graph<EdgeType>();
        graph = AdjacencyArray<EdgeType>::construct_with_dfs_order(adj_list);
    }

    // use same order as in adj_list
    StaticTrieGraph(AdjacencyArray<EdgeType> &adj_array)
    {
        graph = adj_array;
    }

    std::vector<int> construct_node_to_word_index(WordList &words)
    {
        std::vector<int> node_to_word_index(graph.num_nodes(), -1);
        for (uint i = 0; i < words.size(); i++)
        {
            int v = find_node(words[i]);
            assert(v > 0);
            node_to_word_index[v] = i;
        }
        return node_to_word_index;
    }

    // assumes trie constains word
    int find_node(std::string &s)
    {
        uint32_t v = 0;
        for (char c : s)
        {
            for (auto &e : graph.neighbors(v))
            {
                char l = e.get_letter();
                if (l == c)
                {
                    v = e.get_id();
                    break;
                }
            }
        }
        return v;
    }

    bool contains_word(std::string &s)
    {
        uint32_t v = 0;
        bool is_a_word = false;
        for (char c : s)
        {
            bool found = false;
            for (auto &e : graph.neighbors(v))
            {
                char l = e.get_letter();
                if (l == c)
                {
                    found = true;
                    is_a_word = e.is_word();
                    v = e.get_id();
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
        }
        return is_a_word;
    }

    AdjacencyArray<EdgeType> graph;
};