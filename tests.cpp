#include <gtest/gtest.h>
#include <random>
#include <string>
#include <vector>

#include "trie.h"
#include "io.h"

TEST(TrieTest, SmallDictionary)
{
    std::string file = "../dictionary_9030.txt";
    auto words = read_dictionary(file);
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
    std::vector<std::string> v2 = {"appl", "bpple" "banaa", "par", "grpe"};
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
        ASSERT_FALSE(trie.contains_word(s));
    }
}