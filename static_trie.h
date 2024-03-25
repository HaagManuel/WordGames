#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <cstdint>

#include "graph.h"
#include "trie.h"

template <typename EdgeType>
struct StaticTrieGraph
{
    StaticTrieGraph(std::vector<std::string> &words)
    {
        Trie trie(words);
        AdjacencyList<EdgeType> adj_list = trie.extract_graph<EdgeType>();
        graph = AdjacencyArray(adj_list);
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