#include <gtest/gtest.h>
#include <random>
#include <string>
#include <vector>

#include "trie.h"
#include "io.h"
#include "small_map.h"

TEST(TrieTest, SmallDictionary)
{
    std::string file = "../dictionary_9030.txt";
    auto words = io::read_dictionary(file);
    Trie trie;
    for (auto &s : words)
    {
        trie.insert(s);
    }
    for (auto &s : words)
    {
        ASSERT_TRUE(trie.contains_word(s));
    }
}

TEST(TrieTest, PositveAndNegative)
{
    Trie trie;
    std::vector<std::string> v1 = {"apple", "banana", "pear", "grape"};
    std::vector<std::string> v2 = {"appl", "bpple", "banaa", "par", "grpe"};
    for (auto &s : v1)
    {
        trie.insert(s);
    }
    for (auto &s : v1)
    {
        ASSERT_TRUE(trie.contains_word(s));
    }
    for (auto &s : v2)
    {
        std::cout << s << "\n";
        ASSERT_FALSE(trie.contains_word(s));
    }
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