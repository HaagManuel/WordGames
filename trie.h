#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include <string>

#include "small_map.h"

struct TrieNodeFixedArray
{
    TrieNodeFixedArray()
    {
        for (int i = 0; i < 26; i++)
            children[i] = -1;
    }
    inline std::pair<int, bool> insert_child_if_not_present(char c, int idx)
    {
        int j = c - 'a';
        int i = children[j];
        if (i == -1)
        {
            children[j] = idx;
            return {idx, true};
        }
        return {i, false};
    }

    inline std::pair<int, bool> get_child_if_present(char c)
    {
        int j = c - 'a';
        int i = children[j];
        return {i, i != -1};
    }

    inline void set_is_word()
    {
        is_a_word = true;
    }

    inline bool is_word()
    {
        return is_a_word;
    }

    inline int num_children() const
    {
        int cnt = 0;
        for (int i = 0; i < 26; i++)
        {
            cnt += children[i] != -1;
        }
        return cnt;
    }

    int children[26];
    bool is_a_word = false;
};

struct TrieNodeSmallMap
{
    inline std::pair<int, bool> insert_child_if_not_present(char c, int idx)
    {
        auto [i, inserted] = children.insert_if_not_present(c, idx);
        return {i, inserted};
    }

    inline std::pair<int, bool> get_child_if_present(char c) const
    {
        return children.get_value_if_key_present(c);
    }

    inline void set_is_word()
    {
        is_a_word = true;
    }

    inline bool is_word() const
    {
        return is_a_word;
    }

    inline int num_children() const
    {
        return children.size();
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

    inline int num_children() const
    {
        return children.size();
    }

    std::unordered_map<char, int> children;
    bool is_a_word = false;
};

struct Trie
{
    using TrieNode = TrieNodeSmallMap;
    // using TrieNode = TrieNodeFixedArray;
    // using TrieNode = TrieNodeHashMap;
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
    bool contains_word(std::string &s) const
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

    int get_num_nodes() const
    {
        return nodes.size();
    }

    std::vector<int> get_node_degrees()
    {
        std::vector<int> v;
        v.reserve(nodes.size());
        for (auto &node : nodes)
        {
            v.push_back(node.num_children());
        }
        return v;
    }

    void print_degree_statistic()
    {
        auto degrees = get_node_degrees();
        double avg = (double)std::accumulate(degrees.begin(), degrees.end(), 0) / nodes.size();
        std::map<int, int> counter;
        for (auto d : degrees)
        {
            if (counter.count(d) == 0)
            {
                counter[d] = 1;
            }
            else
            {
                counter[d] += 1;
            }
        }
        std::cout << "number of nodes: " << nodes.size() << "\n";
        std::cout << "avg degree: " << avg << "\n";
        std::cout << "degree --> count: " << avg << "\n";
        for (auto [key, value] : counter)
        {
            std::cout << key << " --> " << value << "\n";
        }
    }

    std::vector<TrieNode> nodes;
    int root_idx;
};