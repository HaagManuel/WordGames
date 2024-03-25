#pragma once

#include <string>
#include <vector>

#include "common.h"
#include "trie.h"

enum WordleHintChar
{
    DOES_NOT_OCCUR,
    DIFFERENT_POSITION,
    CORRECT_POSITION,
};
using WordleHint = std::vector<WordleHintChar>;
static const std::vector<std::string> HINT_COLOR = {BLACK, YELLOW, GREEN};

void print_colorful_hint(WordleHint &hint, std::string &word)
{
    int n = hint.size();
    for (int i = 0; i < n; i++)
    {
        color_print(word[i], HINT_COLOR[hint[i]]);
    }
    std::cout << "\n";
}

struct Wordle
{
    Wordle(WordList &_words) : words(_words), trie(words), counter(), number_of_guesses(0) {}

    void get_wordle_hint(WordleHint &hints, std::string &guess)
    {
        assert(hints.size() == guess.size());
        int n = hints.size();
        number_of_guesses++;

        for (int i = 0; i < n; i++)
        {
            char c = guess[i];
            if (c == secret_word[i])
            {
                hints[i] = WordleHintChar::CORRECT_POSITION;
            }
            else if (counter.get_count(c) > 0)
            {

                hints[i] = WordleHintChar::DIFFERENT_POSITION;
            }
            else
            {
                hints[i] = WordleHintChar::DOES_NOT_OCCUR;
            }
        }
    }

    void set_word(std::string s)
    {
        secret_word = s;
        number_of_guesses = 0;
        counter.new_counter(s);
    }

    bool is_valid_word(std::string &s) { return trie.contains_word(s); }

    bool is_secret_word(std::string &s) const { return s == secret_word; }

    int get_number_of_guesses() const { return number_of_guesses; }

    WordList &words;
    Trie trie;
    CharCounter counter;
    std::string secret_word;
    int number_of_guesses;
};