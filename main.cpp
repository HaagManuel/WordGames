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

    // benchmark_word_challenge(words);

    // GuesserStrategy strategy = GuesserStrategy::RANDOM_CANDITATE;
    GuesserStrategy strategy = GuesserStrategy::LETTER_FREQUENCY;
    bool print_csv = true;

    benchmark_wordle(words, strategy, print_csv);
    // benchmark_wordle(words, strategy);
    return 0;

    int seed = 0;
    int max_guesses = 10;
    int word_length = 3;
    WordleSimulation sim(words, max_guesses, seed, strategy);
    RandomWordGenerator word_gen(words, seed);
    int n = 3;
    for (int i = 0; i < n; i++)
    {
        sim.play_one_round<true>(word_gen.random_word_of_length(word_length));
    }

    return 0;
}