#pragma once

#include <string>
#include <sstream>
#include <tuple>
#include <vector>
#include <unordered_set>

#include "common.h"
#include "trie.h"
#include "random.h"

enum GuesserStrategy
{
    RANDOM_CANDITATE,
    LETTER_FREQUENCY,
};

std::string strategy_to_string(GuesserStrategy strategy)
{
    if (strategy == GuesserStrategy::RANDOM_CANDITATE)
    {
        return "random_canditate";
    }
    else if (strategy == GuesserStrategy::LETTER_FREQUENCY)
    {
        return "letter_frequency";
    }
    else
    {
        return "";
    }
}

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
    Wordle(WordList &_words) : words(_words), trie(words) {}

    void get_wordle_hint(WordleHint &hints, std::string &guess)
    {
        assert(hints.size() == guess.size());
        int n = hints.size();

        count.new_counter(secret_word);
        std::fill(hints.begin(), hints.end(), WordleHintChar::DOES_NOT_OCCUR);

        // first find correct position
        for (int i = 0; i < n; i++)
        {
            char c = guess[i];
            if (c == secret_word[i])
            {
                hints[i] = WordleHintChar::CORRECT_POSITION;
                count.decrement(c);
            }
        }

        // letters are marked yellow from left to right
        for (int i = 0; i < n; i++)
        {
            char c = guess[i];
            if (c != secret_word[i] && count.get_count(c) > 0)
            {
                hints[i] = WordleHintChar::DIFFERENT_POSITION;
                count.decrement(c);
            }
        }
    }

    void set_secret_word(std::string s) { secret_word = s; }

    bool is_valid_word(std::string &s) { return trie.contains_word(s); }

    bool is_secret_word(std::string &s) const { return s == secret_word; }

    CharCounter count;
    WordList &words;
    Trie trie;
    std::string secret_word;
};

struct RandomWordleGuesser
{
    using EdgeType = TrieEdge;
    RandomWordleGuesser(WordList &_words, int seed, GuesserStrategy strategy) : words(_words), gen(seed), guesser_strategy(strategy)
    {
        Trie trie(words);
        AdjacencyList<EdgeType> adj_list = trie.extract_graph<EdgeType>();
        graph = AdjacencyArray<EdgeType>::construct_with_dfs_order(adj_list);

        node_to_word_index = StaticTrieGraph<EdgeType>(graph).construct_node_to_word_index(words);
        words_of_len = compute_index_word_of_len(words);

        // precompute letter count of each word
        letter_cnt_words = std::vector<CharCounter>(words.size());
        for (uint i = 0; i < words.size(); i++)
        {
            CharCounter cnt(words[i]);
            letter_cnt_words[i] = cnt;
        }

        // precompute maximal upper bound for each word length
        upper_bound_words = std::vector<CharCounter>(words_of_len.size());
        for (uint i = 0; i < words_of_len.size(); i++)
        {
            if (words_of_len[i].size() > 0)
            {
                int bound = 0;
                for (char c : ALPHABET)
                {
                    for (auto idx : words_of_len[i])
                    {
                        bound = std::max(bound, letter_cnt_words[idx].get_count(c));
                    }
                    upper_bound_words[i].set_count(c, bound);
                }
                // print_vector(upper_bound_words[i].counter);
            }
        }

        // precompute best start word for each length
        lower_bound.reset_counter();
        for (char c : ALPHABET)
        {
            upper_bound.set_count(c, words_of_len.size());
        }
        best_start_word = std::vector<int>(words_of_len.size());
        for (uint i = 0; i < words_of_len.size(); i++)
        {
            if (words_of_len[i].size() > 0)
            {
                best_start_word[i] = compute_highest_score_word(words_of_len[i]);
                // std::cout << i << " " << words[best_start_word[i]] << "\n";
            }
        }
    }

    void new_word(int _word_len)
    {
        assert(words_of_len[_word_len].size() > 0);

        word_len = _word_len;
        number_of_guesses = 0;
        know_chars = std::string(word_len, UNKNOWN);

        lower_bound.reset_counter();
        for (char c : ALPHABET)
        {
            int char_bound = upper_bound_words[word_len].get_count(c);
            upper_bound.set_count(c, char_bound);
        }

        letter_not_at_pos.resize(word_len, std::vector<bool>(26, false));
        for (auto &v : letter_not_at_pos)
        {
            std::fill(v.begin(), v.end(), false);
        }
        guessed_words.clear();
    }

    void take_hint(WordleHint &hint, std::string &guessed_word)
    {
        for (uint i = 0; i < hint.size(); i++)
        {
            char c = guessed_word[i];
            if (hint[i] == WordleHintChar::CORRECT_POSITION)
            {
                know_chars[i] = c;
            }
            else if (hint[i] == WordleHintChar::DIFFERENT_POSITION)
            {
                letter_not_at_pos[i][c - 'a'] = true;
            }
        }

        int n = hint.size();
        int num_known = 0;
        CharCounter known_green;

        for (int i = 0; i < n; i++)
        {
            if (know_chars[i] != UNKNOWN)
            {
                num_known++;
                known_green.increment(know_chars[i]);
            }
        }
        for (char c : ALPHABET)
        {
            int yellow = 0;
            int green = 0;
            int gray = 0;
            for (int i = 0; i < n; i++)
            {
                char l = guessed_word[i];
                auto h = hint[i];
                if (l == c)
                {
                    yellow += h == WordleHintChar::DIFFERENT_POSITION;
                    green += h == WordleHintChar::CORRECT_POSITION;
                    gray += h == WordleHintChar::DOES_NOT_OCCUR;
                }
            }
            int old_bound, bound;

            // green and yellow letters have to be at least present
            old_bound = lower_bound.get_count(c);
            bound = green + yellow;
            lower_bound.set_count(c, std::max(old_bound, bound));

            // for each free place that is not green
            old_bound = upper_bound.get_count(c);
            bound = n - num_known + known_green.get_count(c);
            upper_bound.set_count(c, std::min(old_bound, bound));

            // if there is a gray letter, not more than the current count of yellow and green is possible
            if (gray > 0)
            {
                old_bound = upper_bound.get_count(c);
                bound = green + yellow;
                upper_bound.set_count(c, std::min(old_bound, bound));
            }
            // std::cout << c << " " << lower_bound.get_count(c) << " " << upper_bound.get_count(c) << "\n";
        }
    }

    void remove_already_guessed_words()
    {
        uint i = 0;
        while (i < canditate_index.size())
        {
            int idx = canditate_index[i];
            if (guessed_words.count(idx) > 0)
            {
                std::swap(canditate_index[i], canditate_index.back());
                canditate_index.pop_back();
                continue;
            }
            i++;
        }
    }

    // returns index to word
    int guess_random_canditate()
    {
        visited_nodes = 0;
        canditate_size = words_of_len[word_len].size();
        if (number_of_guesses == 1)
        {
            int i = gen.random_index(words_of_len[word_len].size());
            return words_of_len[word_len][i];
        }
        search_candidates();

        remove_already_guessed_words();
        canditate_size = canditate_index.size();

        if (canditate_size == 0)
        {
            int j = gen.random_element(words_of_len[word_len]);
            return j;
        }
        assert(canditate_size > 0);
        return gen.random_element(canditate_index);
    }

    // compute letter frequency of letters we have no information of for all candiates
    int compute_highest_score_word(std::vector<int> &candidates)
    {
        int word_length = words[candidates[0]].size();
        int num_words = words_of_len[word_length].size();
        int num_candidates = candidates.size();
        CharCounter cnt_freq;
        for (char c : ALPHABET)
        {
            // skip letter where we already made a guess information
            if (lower_bound.get_count(c) > 0 || upper_bound.get_count(c) == 0)
                continue;
            for (int i = 0; i < num_candidates; i++)
            {
                int idx = candidates[i];
                if (letter_cnt_words[idx].get_count(c) > 0)
                {
                    cnt_freq.increment(c);
                }
            }
        }
        long long sum = std::accumulate(cnt_freq.counter.begin(), cnt_freq.counter.end(), 0);
        double freq[26];
        for (int i = 0; i < 26; i++)
        {
            freq[i] = (double)cnt_freq.get_count(i + 'a') / sum;
        }

        score_word.resize(num_words);
        for (int i = 0; i < num_words; i++)
        {
            int idx = words_of_len[word_length][i];
            double score = 0;
            for (int j = 0; j < 26; j++)
            {
                int letter_freq = letter_cnt_words[idx].get_count(j + 'a');
                bool b = letter_freq > 0;
                score += b * freq[j];
            }
            score_word[i] = {score, words_of_len[word_length][i]};
        }
        auto [score, idx] = *std::max_element(score_word.begin(), score_word.end());
        return idx;
    }

    // returns index to word
    int guess_by_letter_frequency()
    {
        if (number_of_guesses == 1)
        {
            canditate_size = words_of_len[word_len].size();
            return best_start_word[word_len];
        }

        search_candidates();
        remove_already_guessed_words();
        canditate_size = canditate_index.size();

        if (canditate_size == 0)
        {
            return gen.random_element(words_of_len[word_len]);
        }
        else if (canditate_size < 10)
        {
            return gen.random_element(canditate_index);
        }
        return compute_highest_score_word(canditate_index);
    }

    std::string make_guess()
    {
        number_of_guesses++;
        int idx;
        if (guesser_strategy == GuesserStrategy::RANDOM_CANDITATE)
        {
            idx = guess_random_canditate();
        }
        else
        {
            idx = guess_by_letter_frequency();
        }
        guessed_words.insert(idx);
        return words[idx];
    }

    int missing_letters()
    {
        int missing = 0;
        for (char c : ALPHABET)
        {
            int cnt = found_letters.get_count(c);
            int lower = lower_bound.get_count(c);
            int upper = upper_bound.get_count(c);
            if (cnt <= upper)
            {
                missing += std::max(0, lower - cnt);
            }
            else
            {
                // indicates that we prune search
                return 1e9;
            }
        }
        return missing;
    }

    void search_candidates()
    {
        canditate_index.clear();
        visited_nodes = 0;
        found_letters.reset_counter();
        search_rec(0, 0, false);
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

            // we know letter at this position
            bool forced_move = c == letter;

            int new_cnt = found_letters.get_count(c) + 1;
            int upper = upper_bound.get_count(c);
            bool search_subtree = letter == UNKNOWN && !letter_not_at_pos[depth][c - 'a'] && new_cnt <= upper;

            if (forced_move || search_subtree)
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
    std::unordered_set<int> guessed_words;
    std::string know_chars;

    std::vector<CharCounter> upper_bound_words;
    CharCounter lower_bound;
    CharCounter upper_bound;

    CharCounter found_letters;
    std::vector<std::vector<bool>> letter_not_at_pos;

    std::vector<CharCounter> letter_cnt_words;
    std::vector<int> best_start_word;
    std::vector<std::pair<double, int>> score_word;

    int word_len;
    int number_of_guesses;

    WordList &words;
    RandomGenerator gen;
    GuesserStrategy guesser_strategy;
    AdjacencyArray<TrieEdge> graph;
    std::vector<int> node_to_word_index;
    std::vector<std::vector<int>> words_of_len;
};

struct WordleSimulation
{
    WordleSimulation(WordList &_words, int _max_guesses, int seed, GuesserStrategy strategy) : words(_words), wordle(words), gen(seed), guesser(words, seed + 1, strategy), max_guesses(_max_guesses) {}

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

    template <bool debug = false>
    bool play_one_round(std::string _secret_word)
    {
        bool found_word = false;
        secret_word = _secret_word;
        reset();
        guesser.new_word(secret_word.size());
        wordle.set_secret_word(secret_word);

        if constexpr (debug)
        {
            std::cout << "secret word: " << secret_word << "\n";
        }

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
                if (canditates <= 20)
                {
                    print_indexed_words(guesser.canditate_index, words);
                }
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
                std::cout << "found secret word after " << num_guesses << " guesses"
                          << "\n";
                color_print(secret_word, GREEN);
            }
            else
            {
                std::cout << "did not find secret word after " << num_guesses << " guesses"
                          << "\n";
                color_print(secret_word, RED);
            }
            std::cout << "\n";
            std::string banner(20, '#');
            std::cout << banner << "\n\n";
        }
        vec_num_guess.push_back(num_guesses);
        return found_word;
    }

    WordList &words;
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

void find_best_start_word(WordList &words, int len)
{
    Wordle wordle(words);
    RandomWordleGuesser guesser(words, 41, GuesserStrategy::RANDOM_CANDITATE);
    std::vector<std::vector<int>> words_of_len = compute_index_word_of_len(words);

    RandomGenerator gen(5);

    int m = words_of_len[len].size();
    if (m == 0)
        return;

    int sample_size = 100;
    std::vector<int> sum_canditates(m, 0);
    for (int i = 0; i < m; i++)
    {
        // for (int j = 0; j < m; j++)
        for (int j = 0; j < sample_size; j++)
        {
            int w1 = words_of_len[len][i];
            int w2 = gen.random_element(words_of_len[len]);
            // int w2 = words_of_len[len][j];

            std::string secret_word = words[w1];
            std::string guess = words[w2];
            WordleHint hint(len, WordleHintChar::EMPTY);

            wordle.set_secret_word(secret_word);
            wordle.get_wordle_hint(hint, guess);

            guesser.new_word(len);
            guesser.take_hint(hint, guess);
            // first is random guess
            guesser.make_guess();

            guesser.make_guess();
            sum_canditates[i] += guesser.get_canditate_size();
        }
    }

    std::vector<std::pair<int, std::string>> score_strings;
    score_strings.reserve(m);
    for (int i = 0; i < m; i++)
    {
        sum_canditates[i] /= sample_size;
    }

    std::sort(score_strings.begin(), score_strings.end());
    int i = 0;
    int top = 10;
    for (auto &[score, word] : score_strings)
    {
        std::cout << score << " " << word << "\n";
        i++;
        if (i == top)
            break;
    }
    std::cout << "\n";
}