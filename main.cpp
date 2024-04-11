#include <iostream>
#include <vector>
#include <string>

#include "benchmarks.h"
#include "io.h"

#include "wordle.h"
#include "cli.h"

void run_benchmarks()
{
    // std::string file = "../dictionary_9030.txt";
    std::string file = "../dictionary_large.txt";
    WordList words = io::read_dictionary(file);

    print_word_statistics(words);

    benchmark_trie_by_word_length<Trie>(words, "Trie");
    benchmark_trie_by_word_length<TrieArray>(words, "TrieArray");
    benchmark_trie_by_word_length<StaticTrieGraph<TrieEdge>>(words, "StaticTrie");
    benchmark_trie_by_word_length<StaticTrieGraph<CompressedTrieEdge>>(words, "StaticTrie Compressed Edge");

    benchmark_word_challenge(words);

    GuesserStrategy strategy = GuesserStrategy::RANDOM_CANDITATE;
    benchmark_wordle(words, strategy);

    strategy = GuesserStrategy::LETTER_FREQUENCY;
    benchmark_wordle(words, strategy);
}

int main(int argc, char *argv[])
{

    // cli::start_cli_application(argc, argv);
    // return 0;
    // run_benchmarks();

    // std::string file = "../dictionary_9030.txt";
    std::string file = "../dictionary_large.txt";
    WordList words = io::read_dictionary(file);

    // Trie trie(words);
    // int num_nodes = trie.get_num_nodes();
    // std::cout << "Trie has " << num_nodes << " nodes. \n";
    // return 0;

    // benchmark_word_challenge(words);

    GuesserStrategy strategy = GuesserStrategy::RANDOM_CANDITATE;
    // GuesserStrategy strategy = GuesserStrategy::LETTER_FREQUENCY;
    bool print_csv = false;

    benchmark_wordle(words, strategy, print_csv);
    return 0;
}