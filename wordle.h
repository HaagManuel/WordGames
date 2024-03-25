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
    EMPTY,
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

// chooses a random word from the set of possible words
struct RandomWordleGuesser
{
    using EdgeType = TrieEdge;
    RandomWordleGuesser(WordList &_words) : words(_words)
    {
        Trie trie(words);
        AdjacencyList<EdgeType> adj_list = trie.extract_graph<EdgeType>();
        graph = AdjacencyArray<EdgeType>::construct_with_dfs_order(adj_list);
        // graph = AdjacencyArray<EdgeType>::construct_with_bfs_order(adj_list);

        node_to_word_index = StaticTrieGraph<EdgeType>(words, graph).construct_node_to_word_index(words);
        words_of_len = index_word_of_len(words);
    }

    void reset(int _word_len)
    {
        assert(words_of_len[_word_len].size() > 0);

        word_len = _word_len;
        number_of_guesses = 0;
        know_chars = std::string(word_len, UNKNOWN);
        existing_letters.reset_counter();
        not_occuring_letter.reset_counter();
    }

    void give_hint(WordleHint &hint, std::string &guessed_word)
    {
        for (uint i = 0; i < hint.size(); i++)
        {
            char c = guessed_word[i];
            if (hint[i] == WordleHintChar::CORRECT_POSITION)
            {
                know_chars[i] = c;
                existing_letters.set_count(c, 1);
            }
            else if (hint[i] == WordleHintChar::DIFFERENT_POSITION)
            {
                existing_letters.set_count(c, 1);
            }
            else if (hint[i] == WordleHintChar::DOES_NOT_OCCUR)
            {
                not_occuring_letter.set_count(c, 1);
            }
        }
    }

    std::string make_guess()
    {
        // guess random word from all possible
        // TODO make better, word with many different common letters
        number_of_guesses++;
        if (number_of_guesses == 1)
        {
            int i = rand() % words_of_len[word_len].size();
            int j = words_of_len[word_len][i];
            return words[j];
        }
        visited_nodes = 0;
        found_letters.reset_counter();
        search_rec(0, 0, false);

        int m = canditate_index.size();
        assert(m > 0);

        int i = rand() % m;
        int j = canditate_index[i];
        canditate_index.clear();
        std::cout << "candidate size: " << m << "\n";

        return words[j];
    }

    bool found_existing_letters()
    {
        for (int i = 0; i < 26; i++)
        {
            char c = i + 'a';
            if (found_letters.get_count(c) < existing_letters.get_count(c))
            {
                return false;
            }
        }
        return true;
    }

    int missing_letters()
    {
        int missing = 0;
        for (int i = 0; i < 26; i++)
        {
            char c = i + 'a';
            missing += (found_letters.get_count(c) < existing_letters.get_count(c));
        }
        return missing;
    }

    void search_rec(int v, int depth, bool is_word)
    {
        visited_nodes++;
        int missing = missing_letters();
        if (word_len - depth < missing)
        {
            // not enough letter left to fulfill constraints
            return;
        }
        if (depth == word_len)
        {
            // if (is_word && found_existing_letters())
            if (is_word)
            {
                int idx = node_to_word_index[v];
                canditate_index.push_back(idx);
            }
            return;
        }

        for (auto &e : graph.neighbors(v))
        {
            int w = e.get_id();
            char c = e.get_letter();
            bool is_word = e.is_word();
            char letter = know_chars[depth];
            if (letter != UNKNOWN)
            {
                // if we know letter, only look for right edge
                if (c == letter)
                {
                    found_letters.increment(c);
                    search_rec(w, depth + 1, is_word);
                    found_letters.decrement(c);
                }
                continue;
            }

            // ignore letters that do not occur
            if (not_occuring_letter.get_count(c) == 0)
            {
                found_letters.increment(c);
                search_rec(w, depth + 1, is_word);
                found_letters.decrement(c);
            }
        }
    }

    int get_visited_nodes() const { return visited_nodes; }

    const char UNKNOWN = '?';
    int visited_nodes;

    std::vector<int> canditate_index;

    std::string know_chars;
    CharCounter existing_letters;
    CharCounter not_occuring_letter;
    CharCounter found_letters;
    int word_len;
    int number_of_guesses;

    WordList &words;
    AdjacencyArray<TrieEdge> graph;
    std::vector<int> node_to_word_index;
    std::vector<std::vector<int>> words_of_len;
};

void auto_play_wordle(WordList &words)
{
    int seed = 6;
    srand(seed);

    Wordle wordle(words);
    RandomWordleGuesser guesser(words);

    int m = words.size();

    std::string secret_word = words[rand() % m];
    WordleHint hint(secret_word.size(), WordleHintChar::EMPTY);

    std::cout << "secret word: " << secret_word << "\n";
    wordle.set_word(secret_word);

    guesser.reset(secret_word.size());

    for (int i = 0; i < 10; i++)
    {
        std::string guess = guesser.make_guess();
        std::cout << "visited " << guesser.get_visited_nodes() << " nodes \n";
        if (wordle.is_secret_word(guess))
        {
            std::cout << "found secret word \n";
            return;
        }
        wordle.get_wordle_hint(hint, guess);
        guesser.give_hint(hint, guess);
        print_colorful_hint(hint, guess);
    }
}