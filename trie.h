#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <type_traits>
#include <cstdint>

#include "small_map.h"
#include "graph.h"

struct TrieEdge
{
    TrieEdge() : id(0), letter('a'), node_is_word(false) {}
    TrieEdge(uint32_t _id, char _letter, bool _node_is_a_word) : id(_id), letter(_letter), node_is_word(_node_is_a_word) {}

    int get_id() const { return id; }
    char get_letter() const { return letter; }
    bool is_word() const { return node_is_word; }

    int set_id(int new_id) { return id = new_id; }

    int id;
    char letter;
    bool node_is_word;
};
struct CompressedTrieEdge
{
    // 1 bit bool, 8 bit char, 23 bit index
    // 2^23 = 8_388_608 max index

    CompressedTrieEdge() : edge(0) {}
    CompressedTrieEdge(uint32_t id, char letter, bool is_a_word)
    {
        edge = 0;
        edge |= id;
        edge |= ((uint32_t)letter) << 23;
        edge |= ((uint32_t)is_a_word) << 31;
    }

    static CompressedTrieEdge from_trie_edge(TrieEdge &trie_edge)
    {
        return CompressedTrieEdge(trie_edge.get_id(), trie_edge.get_letter(), trie_edge.get_id());
    }

    uint32_t get_id() const { return edge & ID_MASK; }
    char get_letter() const { return (edge >> 23) & CHAR_MASK; }
    bool is_word() const { return (edge >> 31) & 1; }

    void set_id(uint32_t new_id)
    {
        edge &= ~ID_MASK;
        edge |= new_id;
    }

    uint32_t edge;
    static constexpr uint32_t ID_MASK = (1 << 23) - 1;
    static constexpr uint32_t CHAR_MASK = (1 << 8) - 1;
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

    template <typename EdgeType>
    std::vector<EdgeType> get_children_vector()
    {
        std::vector<EdgeType> edges;
        for (auto [letter, w] : children)
        {
            EdgeType e(w, letter, false);
            edges.push_back(e);
        }
        return edges;
    }

    SmallSortedMap<char, int> children;
    bool is_a_word = false;
};

// keeps edge sorted by character
struct Trie
{
    using TrieNode = TrieNodeSmallMap;

    Trie()
    {
        root_idx = make_new_node();
    }

    Trie(std::vector<std::string> &word_list)
    {
        root_idx = make_new_node();
        for (auto &s : word_list)
        {
            insert(s);
        }
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

    template <typename EdgeType>
    AdjacencyList<EdgeType> extract_graph()
    {
        std::vector<std::vector<EdgeType>> graph(nodes.size());
        for (uint v = 0; v < nodes.size(); v++)
        {
            graph[v].reserve(nodes[v].num_children());
        }
        for (uint v = 0; v < nodes.size(); v++)
        {
            for (auto &edge : nodes[v].get_children_vector<EdgeType>())
            {
                int w = edge.get_id();
                EdgeType e(w, edge.get_letter(), nodes[w].is_word());
                graph[v].push_back(e);
            }
        }
        return AdjacencyList{graph};
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

    inline std::pair<int, bool> get_child_if_present(char c) const
    {
        int j = c - 'a';
        int i = children[j];
        return {i, i != -1};
    }

    inline void set_is_word()
    {
        is_a_word = true;
    }

    inline bool is_word() const
    {
        return is_a_word;
    }

    int children[26];
    bool is_a_word = false;
};

// only works for alphabet a-z, has faster look up
struct TrieArray
{
    using TrieNode = TrieNodeFixedArray;

    TrieArray()
    {
        root_idx = make_new_node();
    }

    TrieArray(std::vector<std::string> &word_list)
    {
        root_idx = make_new_node();
        for (auto &s : word_list)
        {
            insert(s);
        }
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

    std::vector<TrieNode> nodes;
    int root_idx;
};
