// Executer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"
#include "queue.hpp"
#include "stats.hpp"

// read data from json file
std::vector<std::string> getCommandsFromFile(std::string filename)
{
    using json = nlohmann::json;
    std::vector<std::string> cmds;
    std::ifstream ifs(filename);
    if (ifs.good())
    {
        json jf = json::parse(ifs);
        std::cout << jf["commands"].size() << "\n";
        for (auto& c : jf["commands"]) {
            std::cout << c << "\n";
            cmds.push_back(c);
        }
    }
    return cmds;
}


int main(int argc, char *argv[])
{
    if (argc < 1) {
        std::cout << "Please specify input json file name";
    }
    std::vector<std::string> commands = getCommandsFromFile(argv[1]);
    std::cout << "Hello World!\n";
}

