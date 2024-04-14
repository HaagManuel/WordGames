#include <gtest/gtest.h>
#include <random>
#include <string>
#include <vector>

#include "trie.h"
#include "static_trie.h"
#include "io.h"
#include "small_map.h"

TEST(TrieTest, SmallDictionary)
{
    std::string file = "../dictionary_9030.txt";
    auto words = io::read_dictionary(file);
    Trie trie1(words);
    TrieArray trie2(words);
    StaticTrieGraph<TrieEdge> trie3(words);
    StaticTrieGraph<CompressedTrieEdge> trie4(words);
    for (auto &s : words)
    {
        ASSERT_TRUE(trie1.contains_word(s));
        ASSERT_TRUE(trie2.contains_word(s));
        ASSERT_TRUE(trie3.contains_word(s));
        ASSERT_TRUE(trie4.contains_word(s));
    }
}

TEST(TrieTest, PositveAndNegative)
{
    std::vector<std::string> v1 = {"apple", "banana", "pear", "grape"};
    std::vector<std::string> v2 = {"appl", "bpple", "banaa", "par", "grpe"};
    Trie trie1(v1);
    TrieArray trie2(v1);
    StaticTrieGraph<TrieEdge> trie3(v1);
    StaticTrieGraph<CompressedTrieEdge> trie4(v1);
    for (auto &s : v1)
    {
        ASSERT_TRUE(trie1.contains_word(s));
        ASSERT_TRUE(trie2.contains_word(s));
        ASSERT_TRUE(trie3.contains_word(s));
        ASSERT_TRUE(trie4.contains_word(s));
    }
    for (auto &s : v2)
    {
        ASSERT_FALSE(trie1.contains_word(s));
        ASSERT_FALSE(trie2.contains_word(s));
        ASSERT_FALSE(trie3.contains_word(s));
        ASSERT_FALSE(trie4.contains_word(s));
    }
}

TEST(GraphTest, OrderTest)
{
    /*
         0,.
      1,a   4,b
      2,a   5,b
      3,a   6,b
    */
    std::vector<int> bfs = {0, 1, 3, 5, 2, 4, 6};
    std::vector<int> dfs = {0, 1, 2, 3, 4, 5, 6};
    std::vector<std::string> words = {"aaa", "bbb"};
    Trie trie(words);

    auto graph1 = trie.extract_graph<TrieEdge>();
    auto graph2 = AdjacencyArray(graph1);

    auto bfs1 = compute_bfs_order(graph1, 0);
    auto bfs2 = compute_bfs_order(graph2, 0);
    auto dfs1 = compute_dfs_order(graph1, 0);
    auto dfs2 = compute_dfs_order(graph2, 0);

    ASSERT_TRUE(bfs1 == bfs2);
    ASSERT_TRUE(dfs1 == dfs2);
    ASSERT_TRUE(bfs1 == bfs);
    ASSERT_TRUE(dfs1 == dfs);
}

TEST(SmallMapTest, TestSorted)
{
    SmallSortedMap<char, int> map;
    std::string s = "fedcba";
    int i = 0;
    for (char c : s)
    {
        auto [v, inserted] = map.insert_if_not_present(c, i);
        auto [v2, present] = map.get_value_if_key_present(c);
        ASSERT_TRUE(inserted);
        ASSERT_TRUE(present);
        ASSERT_EQ(v, i);
        ASSERT_EQ(v, v2);
        i++;
    }

    i = s.size() - 1;
    for (auto [k, v] : map.arr)
    {
        ASSERT_EQ(k, s[i]);
        ASSERT_EQ(v, i);
        i--;
    }

    i = 0;
    for (char c : s)
    {
        auto [v, inserted] = map.insert_if_not_present(c, 10);
        ASSERT_FALSE(inserted);
        ASSERT_EQ(v, i);
        i++;
    }
    auto [v1, present1] = map.get_value_if_key_present('g');
    auto [v2, present2] = map.get_value_if_key_present('h');
    ASSERT_FALSE(present1);
    ASSERT_FALSE(present2);
}