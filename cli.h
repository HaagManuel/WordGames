#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "benchmarks.h"
#include "io.h"
#include "application.h"
#include "CLI/CLI.hpp"
#include "wordle.h"

#define SHOW_ARGUMENT(x) std::cout << std::setfill(' ') << std::setw(30) << std::string(#x " = ") << (x) << "\n";

namespace cli
{
    struct Config
    {
        uint word_length;
        uint repeats;
        uint max_guesses;
        int seed;

        std::string game_type;
        std::string game_mode_word_challenge;
        std::string game_mode_wordle;
        std::string wordle_guesser_strategy;
        std::string dictionary_file;

        void print()
        {
            std::string banner(20, '#');
            std::cout << banner << "\n";
            std::cout << "configuration"
                      << "\n";
            SHOW_ARGUMENT(word_length);
            SHOW_ARGUMENT(repeats);
            SHOW_ARGUMENT(max_guesses);
            SHOW_ARGUMENT(seed);
            SHOW_ARGUMENT(game_type);
            SHOW_ARGUMENT(game_mode_word_challenge);
            SHOW_ARGUMENT(game_mode_wordle);
            SHOW_ARGUMENT(wordle_guesser_strategy);
            SHOW_ARGUMENT(dictionary_file);
            std::cout << banner << "\n";
            std::cout << "\n";
        }
    };

    void word_challenge_application(Config &config)
    {
        WordList words = io::read_dictionary(config.dictionary_file);
        if (!io::check_word_list(words))
        {
            return;
        }

        WordChallengeApplication app(words, config.seed);
        if (config.game_mode_word_challenge == "auto")
        {
            app.play_auto_mode(config.repeats, config.word_length);
        }
        else
        {
            app.play_interactive();
        }
    }

    void wordle_application(Config &config)
    {
        WordList words = io::read_dictionary(config.dictionary_file);
        if (!io::check_word_list(words))
        {
            return;
        }
        GuesserStrategy guesser_strategy = GuesserStrategy::RANDOM_CANDITATE;
        if (config.wordle_guesser_strategy == "letter_frequency")
        {
            guesser_strategy = GuesserStrategy::LETTER_FREQUENCY;
        }
        WordleApplication app(words, config.seed, guesser_strategy);

        if (config.game_mode_wordle == "guesser")
        {
            app.play_as_guesser(config.word_length, config.max_guesses);
        }
        else if (config.game_mode_wordle == "keeper")
        {
            app.play_as_keeper(config.word_length, config.max_guesses);
        }
        else
        {
            app.play_automatic(config.word_length, config.max_guesses, config.repeats);
        }
    }

    int start_cli_application(int argc, char *argv[])
    {
        CLI::App app{"Word Challenge and Wordle Game."};

        uint word_length = 5;
        uint repeats = 10;
        uint max_guesses = 10;
        int seed = 0;
        std::string game_type = "word_challenge";
        std::string game_mode_word_challenge = "auto";
        std::string game_mode_wordle = "auto";
        std::string wordle_guesser_strategy = "letter_frequency";
        std::string dictionary_file = "../dictionary_9030.txt";
        bool run_wordle_experiment = false;

        std::vector<std::string> allowed_game_types = {"word_challenge", "wordle"};
        std::vector<std::string> allowed_game_mode_wordle = {"auto", "keeper", "guesser"};
        std::vector<std::string> allowed_game_mode_word_challenge = {"auto", "interactive"};
        std::vector<std::string> allowed_wordle_strategies = {"random_canditate", "letter_frequency"};

        app.add_option("-l, --word_length", word_length, "word length to be used in game")->check(CLI::Range(1, 100));
        app.add_option("-r, --repeats", repeats, "number of times automatic mode repeats game")->check(CLI::Range(1, 1000000000));
        app.add_option("-g, --max_guesses", max_guesses, "maximal number of guess in wordle game")->check(CLI::Range(1, 1000000000));
        app.add_option("-s, --seed", max_guesses, "seed for random number generation");
        app.add_option("-t, --game_type", game_type, "select type of game")->check(CLI::IsMember(allowed_game_types));
        app.add_option("-w, --game_mode_wordle", game_mode_wordle, "game mode in wordle game")->check(CLI::IsMember(allowed_game_mode_wordle));
        app.add_option("-c, --game_mode_word_challenge", game_mode_word_challenge, "game mode in word challenge game")->check(CLI::IsMember(allowed_game_mode_word_challenge));
        app.add_option("--wordle_strategy", wordle_guesser_strategy, "strategy of the guesser in wordle")->check(CLI::IsMember(allowed_wordle_strategies));
        app.add_option("-f, --file", dictionary_file, "path to dictionary file")->check(CLI::ExistingFile);
        
        app.add_flag("-e, --run_wordle_experiment", run_wordle_experiment, "run wordle experiment");

        CLI11_PARSE(app, argc, argv);

        Config config{word_length, repeats, max_guesses, seed, game_type, game_mode_word_challenge, game_mode_wordle, wordle_guesser_strategy, dictionary_file};

        config.print();

        if (run_wordle_experiment)
        {
            wordle_experiment();
        }
        else if (game_type == "wordle")
        {
            wordle_application(config);
        }
        else
        {
            word_challenge_application(config);
        }
        return 0;
    }
};
