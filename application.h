#pragma once

#include <string>
#include <sstream>
#include <tuple>
#include <vector>

#include "common.h"
#include "trie.h"
#include "random.h"
#include "graph.h"
#include "measure_time.h"
#include "io.h"
#include "word_challenge.h"
#include "wordle.h"

bool check_word_count(uint word_length, RandomWordGenerator &word_gen)
{
    int words_of_len = word_gen.count_words_of_len(word_length);
    if (words_of_len == 0)
    {
        std::cout << "there are no words of length " << word_length << "\n\n";
        return false;
    }
    std::cout << "there are " << words_of_len << " words of length " << word_length << "\n\n";
    return true;
}

struct WordChallengeApplication
{
    WordChallengeApplication(WordList &_words, int seed) : words(_words), word_challenge(words), word_gen(words, seed) {}

    void play_auto_mode(int repeats, int word_length)
    {
        CharCounter counter;
        std::vector<int> found_words;
        std::vector<int> word_cnt;
        std::vector<int> visited_nodes;
        word_cnt.reserve(repeats);
        visited_nodes.reserve(repeats);

        if (!check_word_count(word_length, word_gen))
            return;

        auto sample_words = word_gen.n_random_words_of_len(repeats, word_length);
        auto run = [&]()
        {
            for (int i = 0; i < repeats; i++)
            {
                counter.new_counter(sample_words[i]);
                found_words = word_challenge.possible_words(counter);
                word_cnt.push_back(found_words.size());
                visited_nodes.push_back(word_challenge.get_num_visited_nodes());
            }
        };
        double avg_time = (double)measureTimeMicroS(run) / repeats;
        double avg_words = mean(word_cnt);
        double avg_visited_nodes = mean(visited_nodes);

        std::string unit = "microseconds";
        std::cout << "average CPU time to find words: " << avg_time << " " << unit << "\n";
        std::cout << "average words found           : " << avg_words << "\n";
        std::cout << "average nodes visited         : " << avg_visited_nodes << "\n";
    }

    void play_interactive()
    {
        CharCounter counter;
        std::stringstream ss;
        ss << "Type any string consisting of [a-z] characters.\n";
        ss << "The program will list all possible words that can be formed by a subset of the given characters.\n\n";
        color_print(ss, YELLOW);

        while (true)
        {
            std::string input = io::get_user_input();
            if (!io::word_is_lower(input))
            {
                std::cout << input << " is not full lowercase \n";
                continue;
            }
            counter.new_counter(input);
            auto indices = word_challenge.possible_words(counter);
            std::cout << "\n";
            print_indexed_words(indices, words);
            std::cout << "\n"
                      << "--> found " << indices.size() << " words"
                      << "\n\n";
        }
    }

    WordList &words;
    WordChallenge word_challenge;
    RandomWordGenerator word_gen;
};

struct WordleApplication
{
    // guesser must have different seed than word generation, otherwise he will guess it in the first try
    WordleApplication(WordList &_words, int _seed, GuesserStrategy strategy) : seed(_seed), words(_words), wordle(words), word_gen(words, seed), guesser(words, seed + 1, strategy), guesser_strategy(strategy) {}

    bool check_word(uint word_length, std::string &guess)
    {
        if (!io::word_is_lower(guess) || guess.size() != word_length)
        {
            std::string msg3 = "word must be in [a-z] and have exactly length " + std::to_string(word_length) + "\n";
            color_print(msg3, YELLOW);
            return false;
        }
        if (!wordle.is_valid_word(guess))
        {
            std::string msg4 = guess + " is not a valid word from the dictionary \n";
            color_print(msg4, YELLOW);
            return false;
        }
        return true;
    }

    bool check_hint(uint word_length, std::string &hint)
    {
        bool ok = hint.size() == word_length;
        for (char c : hint)
        {
            ok &= c >= '0' && c <= '2';
        }
        if (!ok)
        {
            std::string msg = "hint must consists of exactly " + std::to_string(word_length) + " digits from [0-2]\n";
            color_print(msg, YELLOW);
            return false;
        }
        return true;
    }

    WordleHint parse_wordle_hint(std::string &input)
    {
        WordleHint hint;
        for (char c : input)
        {
            if (c == '0')
            {
                hint.push_back(WordleHintChar::DOES_NOT_OCCUR);
            }
            else if (c == '1')
            {
                hint.push_back(WordleHintChar::DIFFERENT_POSITION);
            }
            else
            {
                hint.push_back(WordleHintChar::CORRECT_POSITION);
            }
        }
        return hint;
    }

    void report_found_word(std::string &secret_word, int num_guesses)
    {
        std::string msg = "\nfound secret word after " + std::to_string(num_guesses) + " guesses\n";
        color_print(msg, YELLOW);
        color_print(secret_word, GREEN);
        std::cout << "\n";
    }

    void report_failed_to_find(std::string &secret_word, int num_guesses)
    {
        std::string msg = "\nfailed to find word after " + std::to_string(num_guesses) + " guesses\n";
        color_print(msg, YELLOW);
        color_print(secret_word, RED);
        std::cout << "\n";
    }

    void play_as_guesser(uint word_length, uint max_guesses)
    {
        if (!check_word_count(word_length, word_gen))
            return;

        uint num_guesses = 0;
        std::string secret_word = word_gen.random_word_of_length(word_length);
        WordleHint hint(word_length, WordleHintChar::EMPTY);
        wordle.set_secret_word(secret_word);
        std::stringstream ss;

        ss << "secret word has " << word_length << " letters\n";
        color_print(ss, YELLOW);

        while (true)
        {
            std::cout << "\n";
            uint remaining_guesses = max_guesses - num_guesses;
            ss << "guesses left: " << remaining_guesses << "\n";
            color_print(ss, BLUE);

            std::string guess = io::get_user_input();
            if (!check_word(word_length, guess))
                continue;

            num_guesses++;
            if (wordle.is_secret_word(guess))
            {
                report_found_word(secret_word, num_guesses);
                return;
            }
            if (num_guesses == max_guesses)
            {
                report_failed_to_find(secret_word, num_guesses);
                return;
            }
            wordle.get_wordle_hint(hint, guess);
            print_colorful_hint(hint, guess);
        }
    }

    void play_as_keeper(uint word_length, uint max_guesses)
    {
        if (!check_word_count(word_length, word_gen))
            return;

        std::stringstream ss;
        ss << "Think of a word with " << word_length << " letters. It must be in the dictionary.\n";
        ss << "To give hints to the guesser, type a word of length " << word_length << " consisting of [0-2], where\n";
        ss << "    0 - character does not occur in the word,\n";
        ss << "    1 - character occurs at a different position,\n";
        ss << "    2 - character at this position is correct.\n";
        ss << "When setting yellow letters, keep in mind that the green letters do not count and yellow letters are set from left to right.\n";
        ss << "Example: secret: eabcd, guess: eexyz, hint: 20000\n";
        ss << "Example: secret: eeabc, guess: exyee, hint: 20010\n";
        color_print(ss, YELLOW);

        guesser.new_word(word_length);
        uint num_guesses = 0;
        WordleHint correct(word_length, WordleHintChar::CORRECT_POSITION);
        while (true)
        {
            std::string guess = guesser.make_guess();
            num_guesses++;

            int visited_nodes = guesser.get_visited_nodes();
            int candidates = guesser.get_canditate_size();
            if (candidates == 0)
            {
                ss << "no candiates anymore\n";
                color_print(ss, YELLOW);
                return;
            }
            ss << "\n"
               << "visited nodes: " << visited_nodes << "\n"
               << "candiates: " << candidates << "\n";
            color_print(ss, MAGENTA);

            std::string input;
            do
            {
                std::cout << "guess: ";
                color_print(guess + "\n", BLUE);
                input = io::get_user_input();

            } while (!check_hint(word_length, input));

            WordleHint hint = parse_wordle_hint(input);
            if (hint == correct)
            {
                report_found_word(guess, num_guesses);
                return;
            }
            print_colorful_hint(hint, guess);
            if (num_guesses == max_guesses)
            {
                std::string word(' ', 1);
                report_failed_to_find(word, num_guesses);
            }
            guesser.take_hint(hint, guess);
        }
    }

    void play_automatic(uint word_length, int max_guesses, int repeats)
    {
        if (!check_word_count(word_length, word_gen))
            return;

        WordleSimulation wordle_sim(words, max_guesses, seed + 1, guesser_strategy);
        auto word_sample = word_gen.n_random_words_of_len(repeats, word_length);
        auto run = [&]()
        {
            for (int i = 0; i < repeats; i++)
            {
                wordle_sim.play_one_round<false>(word_sample[i]);
            }
        };
        double avg_time = (double)measureTimeMicroS(run) / repeats;
        auto [guesses, visited, candidates] = wordle_sim.get_log_data();
        double avg_guesses = mean(guesses);
        auto avg_visited = component_wise_mean(visited);
        auto avg_candidates = component_wise_mean(candidates);
        std::string unit = "microseconds";
        std::cout << "avg time per game: " << avg_time << " " << unit << "\n";
        std::cout << "avg_guesses      : " << avg_guesses << "\n";
        std::cout << "avg visited nodes:\n";
        print_vector(avg_visited);
        std::cout << "avg candidates   :\n";
        print_vector(avg_candidates);
        std::cout << "\n";
    }

    int seed;
    WordList &words;
    Wordle wordle;
    RandomWordGenerator word_gen;
    RandomWordleGuesser guesser;
    GuesserStrategy guesser_strategy;
};

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