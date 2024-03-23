#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
namespace io
{
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