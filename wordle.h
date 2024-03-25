#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "common.h"
#include "trie.h"
#include "random.h"

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
    Wordle(WordList &_words) : words(_words), trie(words), counter() {}

    void get_wordle_hint(WordleHint &hints, std::string &guess)
    {
        assert(hints.size() == guess.size());
        int n = hints.size();

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
        counter.new_counter(s);
    }

    bool is_valid_word(std::string &s) { return trie.contains_word(s); }

    bool is_secret_word(std::string &s) const { return s == secret_word; }

    WordList &words;
    Trie trie;
    CharCounter counter;
    std::string secret_word;
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
        words_of_len = compute_index_word_of_len(words);
    }

    void new_word(int _word_len)
    {
        assert(words_of_len[_word_len].size() > 0);

        word_len = _word_len;
        number_of_guesses = 0;
        know_chars = std::string(word_len, UNKNOWN);
        existing_letters.reset_counter();
        not_occuring_letter.reset_counter();
    }

    void take_hint(WordleHint &hint, std::string &guessed_word)
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
        visited_nodes = 0;
        canditate_size = 0;
        if (number_of_guesses == 1)
        {
            int i = gen.random_index(words_of_len[word_len].size());
            int j = words_of_len[word_len][i];
            return words[j];
                }
        found_letters.reset_counter();
        search_rec(0, 0, false);

        canditate_size = canditate_index.size();
        assert(canditate_size > 0);

        int i = gen.random_index(canditate_size);
        int j = canditate_index[i];
        canditate_index.clear();

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
    int get_canditate_size() const { return canditate_size; }

    const char UNKNOWN = '?';
    int visited_nodes;
    int canditate_size;

    std::vector<int> canditate_index;

    std::string know_chars;
    CharCounter existing_letters;
    CharCounter not_occuring_letter;
    CharCounter found_letters;
    int word_len;
    int number_of_guesses;

    RandomGenerator gen;
    WordList &words;
    AdjacencyArray<TrieEdge> graph;
    std::vector<int> node_to_word_index;
    std::vector<std::vector<int>> words_of_len;
};

struct WordleSimulation
{
    WordleSimulation(WordList &_words, int _max_guesses, int seed) : words(_words), wordle(words), gen(seed), guesser(words), max_guesses(_max_guesses) {}

    void reset_logging()
    {
        num_guesses = 0;
        visited_nodes.clear();
        canditate_size.clear();
        vec_num_guess.clear();
    }

    std::tuple<std::vector<int>, std::vector<std::vector<int>>, std::vector<std::vector<int>>> get_log_data()
    {
        std::vector<int> log_guesses = vec_num_guess;
        std::vector<std::vector<int>> log_visited;
        std::vector<std::vector<int>> log_canditates;
        int k = 0;
        auto it_v = visited_nodes.begin();
        auto it_c = canditate_size.begin();
        for (auto guesses : log_guesses)
        {
            assert(k + guesses <= (int)visited_nodes.size());
            assert(k + guesses <= (int)canditate_size.size());
            std::vector<int> vis(it_v + k, it_v + k + guesses);
            std::vector<int> cand(it_c + k, it_c + k + guesses);
            k += guesses;
            log_visited.push_back(vis);
            log_canditates.push_back(cand);
        }
        return {log_guesses, log_visited, log_canditates};
    }

    void reset()
    {
        num_guesses = 0;
        hint.resize(secret_word.size(), WordleHintChar::EMPTY);
    }

    // returns true if word was found in <= max_guesses
    template <bool debug = false>
    bool play_one_round(std::string _secret_word)
    {
        bool found_word = false;
        secret_word = _secret_word;
        reset();
        guesser.new_word(secret_word.size());
        wordle.set_word(secret_word);

        if constexpr (debug)
            std::cout << "secret word: " << secret_word << "\n";

        for (int i = 0; i < max_guesses; i++)
        {
            num_guesses++;
            std::string guess = guesser.make_guess();
            int visited = guesser.get_visited_nodes();
            int canditates = guesser.get_canditate_size();
            visited_nodes.push_back(visited);
            canditate_size.push_back(canditates);
            if constexpr (debug)
            {
                std::cout << "visited nodes: " << visited << "\n";
                std::cout << "candiates: " << canditates << "\n";
            }
            if (wordle.is_secret_word(guess))
            {
                found_word = true;
                break;
            }
            wordle.get_wordle_hint(hint, guess);
            guesser.take_hint(hint, guess);
            if constexpr (debug)
            {
                print_colorful_hint(hint, guess);
            }
        }
        if constexpr (debug)
        {
            if (found_word)
            {
                std::cout << "found secret word: ";
                color_print(secret_word, GREEN);
            }
            else
            {
                std::cout << "failed to find word after " << max_guesses << " guesses\n";
                color_print(secret_word, RED);
            }
            std::cout << "\n";
            std::string banner(20, '#');
            std::cout << banner << "\n\n";
        }
        vec_num_guess.push_back(num_guesses);
        return found_word;
    }

    WordList words;
    Wordle wordle;
    RandomGenerator gen;
    RandomWordleGuesser guesser;

    std::string secret_word;
    WordleHint hint;
    int max_guesses;

    int num_guesses;
    std::vector<int> vec_num_guess;
    std::vector<int> visited_nodes;
    std::vector<int> canditate_size;
};