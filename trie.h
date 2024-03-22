#pragma once
#pragma once
#include <unordered_map>
#include <vector>
#include <string>

struct TrieNode
{
    inline std::pair<int, bool> insert_child_if_not_present(char c, int idx)
    {
        auto [iter, inserted] = children.try_emplace(c, idx);
        return {(*iter).second, inserted};
    }

    inline bool has_child(char c)
    {
        return children.count(c) > 0;
    }

    inline void insert_child(char c, int idx)
    {
        children[c] = idx;
    }

    inline int get_child(char c)
    {
        return children[c];
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
            nodes[node_idx].set_is_word();
        }
    }
    bool contains_word(std::string &s)
    {
        int node_idx = 0;
        for (char c : s)
        {
            if (nodes[node_idx].has_child(c))
            {
                node_idx = nodes[node_idx].get_child(c);
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