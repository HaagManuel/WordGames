#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>

#include "benchmarks.h"
#include "common.h"
#include "trie.h"
#include "measure_time.h"
#include "io.h"
#include "small_map.h"
#include "graph.h"
#include "static_trie.h"
#include "word_challenge.h"
#include "wordle.h"

int main()
{
    std::cout << "Hello Wordle!"
              << "\n";


    // std::string file = "../dictionary_9030.txt";
    std::string file = "../dictionary_large.txt";
    WordList words = io::read_dictionary(file);
    std::cout << "number of words: " << words.size() << "\n";


    benchmark_trie_by_word_length<Trie>(words, "Trie");
    benchmark_trie_by_word_length<TrieArray>(words, "TrieArray");
    benchmark_trie_by_word_length<StaticTrieGraph<TrieEdge>>(words, "StaticTrie");
    benchmark_trie_by_word_length<StaticTrieGraph<CompressedTrieEdge>>(words, "StaticTrie Compressed Edge");
    
    benchmark_word_challenge(words);

    // auto_play_wordle(words);
}