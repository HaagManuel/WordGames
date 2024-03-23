#pragma once
#include <unordered_map>
#include <vector>
#include <string>

#include "small_map.h"

struct TrieNodeSmallMap
{
    inline std::pair<int, bool> insert_child_if_not_present(char c, int idx)
    {
        auto [i, inserted] = children.insert_if_not_present(c, idx);
        return {i, inserted};
    }

    inline std::pair<int, bool> get_child_if_present(char c)
    {
        return children.get_value_if_key_present(c);
    }

    inline void set_is_word()
    {
        is_a_word = true;
    }

    inline bool is_word()
    {
        return is_a_word;
    }
    SmallSortedMap<char, int> children;
    bool is_a_word = false;
};

struct TrieNodeHashMap
{
    inline std::pair<int, bool> insert_child_if_not_present(char c, int idx)
    {
        auto [iter, inserted] = children.try_emplace(c, idx);
        return {(*iter).second, inserted};
    }

    inline std::pair<int, bool> get_child_if_present(char c)
    {
        if (children.count(c) > 0)
        {
            return {children[c], true};
        }
        return {0, false};
    }

    inline void set_is_word()
    {
        is_a_word = true;
    }

    inline bool is_word()
    {
        return is_a_word;
    }
    std::unordered_map<char, int> children;
    bool is_a_word = false;
};

struct Trie
{
    using TrieNode = TrieNodeSmallMap;
    Trie()
    {
        root_idx = make_new_node();
    }

    void insert(std::string &s)
    {
        int node_idx = 0;
        for (char c : s)
        {
            int maybe_next_idx = nodes.size();
            auto [next_idx, inserted] = nodes[node_idx].insert_child_if_not_present(c, maybe_next_idx);
            if (inserted)
            {
                make_new_node();
            }
            node_idx = next_idx;
        }
        nodes[node_idx].set_is_word();
    }
    bool contains_word(std::string &s)
    {
        int node_idx = 0;
        for (char c : s)
        {
            auto [next_idx, exists] = nodes[node_idx].get_child_if_present(c);
            if (exists)
            {
                node_idx = next_idx;
            }
            else
            {
                return false;
            }
        }
        return nodes[node_idx].is_word();
    }

    // returns index of new node
    int make_new_node()
    {
        nodes.emplace_back();
        return nodes.size() - 1;
    }
    std::vector<TrieNode> nodes;
    int root_idx;
};