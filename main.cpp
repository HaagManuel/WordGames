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

void wordle_experiment()
{
    std::string file_small = "../dictionary_9030.txt";
    std::string file_large = "../dictionary_large.txt";
    WordList words_small = io::read_dictionary(file_small);
    WordList words_large = io::read_dictionary(file_large);

    GuesserStrategy strategy_random = GuesserStrategy::RANDOM_CANDITATE;
    GuesserStrategy strategy_frequency = GuesserStrategy::LETTER_FREQUENCY;

    bool print_csv = true;
    bool print_header = true;
    benchmark_wordle(words_small, strategy_random, print_header, print_csv);
    benchmark_wordle(words_small, strategy_frequency, !print_header, print_csv);
    benchmark_wordle(words_large, strategy_random, !print_header, print_csv);
    benchmark_wordle(words_large, strategy_frequency, !print_header, print_csv);
}

int main(int argc, char *argv[])
{
    cli::start_cli_application(argc, argv);
    return 0;

    // run_benchmarks();

    // wordle_experiment();
}