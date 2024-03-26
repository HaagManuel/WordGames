#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
namespace io
{

    std::string get_user_input()
    {
        std::string str;
        std::getline(std::cin, str);
        return str;
    }

    bool word_is_lower(std::string &word)
    {
        for (char c : word)
        {
            if (c < 'a' || c > 'z')
            {
                return false;
            }
        }
        return true;
    }

    bool check_word_list(std::vector<std::string> &words)
    {
        bool ok = true;
        for (auto &s : words)
        {
            if (!word_is_lower(s))
            {
                ok = false;
                std::cout << s << " is not full lowercase in [a-z]\n";
            }
        }
        return ok;
    }

    std::vector<std::string> read_dictionary(std::string &path)
    {
        std::ifstream file(path);
        std::vector<std::string> words;
        if (!file.is_open())
        {
            std::cerr << "Error: Unable to open the file: " << path << std::endl;
            return words;
        }

        std::string word;
        while (file >> word)
        {
            words.push_back(word);
        }
        file.close();
        return words;
    }
}