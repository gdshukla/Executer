// Executer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <map>
#include <time.h>
#include "json.hpp"
#include "queue.hpp"
#include "stats.hpp"
#include "task.hpp"
#include "scheduler.h"
#include "arguments.h"
#include "commands.h"
#include "utilities.hpp"

std::unique_ptr<Stats> Stats::m_instance;

// read data from json file
std::vector<std::shared_ptr<ICommand>> loadCommands(std::string filename)
{
    std::vector<std::shared_ptr<ICommand>> cmdTasks;

    using json = nlohmann::json;
    std::map<std::string, nlohmann::json> cmds;
    std::ifstream ifs(filename);
    if (ifs.good())
    {
        json jf = json::parse(ifs);
        std::cout << "Loaded " << jf["commands"].size() << " commands\n";
        for (auto& c : jf["commands"]) 
        {
            std::shared_ptr<ICommand> cmd;
            std::string name = c["name"];
            //std::cout << name << "\n";
            cmds[name] = c;
            // ----------------------------------------------------------------
            // load command tasks and initialize them
            if (name == "adder")
            {
                 cmd = std::make_unique<adder>();
            }
            else if (name == "factorial")
            {
                cmd = std::make_unique<factorial>();
            }
            // ----------------------------------------------------------------
            cmd->parseArgs(c);  // initialize object members
            cmdTasks.push_back(std::move(cmd));
        }
    }
    else
    {
        std::cout << filename << " file not found\n";
    }

    return cmdTasks;
}

std::vector<std::shared_ptr<Task>> makeTasks(std::vector<std::shared_ptr<ICommand>> &cmd)
{
    std::vector<std::shared_ptr<Task>> tasks;
    for (auto& c : cmd)
    {
        auto func = [&c] {
            std::any result = c->operator()();
            //print(c->getName() + "\n");
            return result;
        };
        Stats::instance()->added();
        tasks.push_back(make_task(func));
    }

    std::cout << "Added " << tasks.size() << " tasks to task queue\n";
    return tasks;
}

std::multimap<std::shared_ptr<Task>, std::shared_ptr<ICommand>> makeTasksMap(std::vector<std::shared_ptr<ICommand>>& cmd)
{
    std::multimap<std::shared_ptr<Task>, std::shared_ptr<ICommand>> cmdTasks;
    for (auto c : cmd)
    {
        auto func = [c] {
            std::any result = c->operator()();
            //print(c->getName() + "\n");
            return result;
        };
        Stats::instance()->added();
        cmdTasks.insert(std::make_pair(make_task(func), c));
    }

    std::cout << "Added " << cmdTasks.size() << " tasks to task queue\n";
    return cmdTasks;
}

void displayTaskDetails(const std::multimap<std::shared_ptr<Task>, std::shared_ptr<ICommand>> &cmdTasks, int tasknum)
{
    int index = 0;
    for (auto& p : cmdTasks)
    {
        if (index == tasknum)
        {
            std::string taskStatus = p.first->status() ? "Complete" : "running";
            std::cout << "Status: " << taskStatus << "\n";
            std::cout << "Details: " << p.second->argumentsToString() << "\n";
            if (p.first->status())
            {
                std::cout << "Result: " << p.second->resultToString(p.first->m_result) << "\n";
                std::cout << "Started at: " << toTime(p.first->start) << "\n";
                std::cout << "Started at: " << toTime(p.first->end) << "\n";
            }
        }
        index++;
    }
}

void processCommands(std::string filename)
{
    std::vector<std::shared_ptr<ICommand>> cmds = loadCommands(filename);
    std::multimap<std::shared_ptr<Task>, std::shared_ptr<ICommand>> cmdTasks = makeTasksMap(cmds);
    while (1)
    {
        std::cout << ">>>";
        std::fflush(stdout);
        const size_t size = 256;
        char buf[size];
        std::cin.getline(buf, size);
        std::string input(buf);

        if (input == "help" || input == "h")
        {
            std::string helpText =
                "--------------------------------------------------------\n"
                "Executer help\n"
                "To execute program run\n"
                ">Executer command.json\n"
                "on terminal\n"
                "In-program commands\n"
                "- help or h: display help text\n"
                "- stats or s: display overall task statistics\n"
                "- load or l: load new json file to add tasks to run\n"
                ">>>load commands.json\n"
                "- multiload or ml: load new json file multiple times to add tasks to run\n"
                ">>>ml 10 commands.json\n"
                "- display or d: display stats about a certain task\n"
                "for example to display stats of task 1\n"
                ">>>d 1\n"
                "- quit or q: quit the program\n"
                "- forcequit or fq: force quit the program.\n"
                "Complete currently scheduled threads and don't schedule any more\n"
                "--------------------------------------------------------\n\n";
                // print help
            std::cout << helpText;
        }
        else if (input == "stats" || input == "s")
        {
            Stats::instance()->display();
        }
        else if (input.starts_with("load ") || input.starts_with("l "))
        {
            std::string filename = "";
            size_t index = 0;
            if (input.starts_with("load "))
            {
                index = 5;
            }
            else
            {
                index = 2;
            }
            if (input.size() > index)
            {
                filename = input.substr(index);
            }
            if (filename.size() > 0)
            {
                std::vector<std::shared_ptr<ICommand>> tempCmds = loadCommands(filename);
                for (auto& c : tempCmds)
                {
                    cmds.push_back(c);
                }
                std::multimap<std::shared_ptr<Task>, std::shared_ptr<ICommand>> temp = makeTasksMap(tempCmds);
                for (auto& c : temp)
                {
                    cmdTasks.insert(std::move(c));
                }
            }
        }
        // for testing:: load same file multiple times
        else if (input.starts_with("multiload ") || input.starts_with("ml "))
        {
            std::stringstream ss(input);
            int count = 0;
            std::string cmd, filename = "";
            ss >> cmd >> count >> filename;
            if (filename.size() > 0)
            {
                for (int i = 0; i < count; i++)
                {
                    std::vector<std::shared_ptr<ICommand>> tempCmds = loadCommands(filename);
                    for (auto& c : tempCmds)
                    {
                        cmds.push_back(c);
                    }
                    std::multimap<std::shared_ptr<Task>, std::shared_ptr<ICommand>> temp = makeTasksMap(tempCmds);
                    for (auto& c : temp)
                    {
                        cmdTasks.insert(std::move(c));
                    }
                }
            }
        }
        else if (input.starts_with("display ") || input.starts_with("d "))
        {
            int tasknum = -1;
            size_t index = 0;
            if (input.starts_with("display "))
            {
                index = 8;
            }
            else
            {
                index = 2;
            }
            if (input.size() > index)
            {
                tasknum = std::stoi(input.substr(index));
            }
            if (tasknum >= 0 && tasknum < cmdTasks.size())
            {
                displayTaskDetails(cmdTasks, tasknum);

            }
            else
            {
                std::cout << "Invalid task number: " << tasknum << "\n";
                std::cout << "Valid values are: 0 to " << cmdTasks.size() - 1 << "\n";
            }
        }
        else if (input == "quit" || input == "q")
        {
            break;
        }
        else if (input == "forcequit" || input == "fq")
        {
            Scheduler::instance()->abort();
            break;
        }

    }
}


int main(int argc, char* argv[])
{
    if (argc < 1) {
        std::cout << "Please specify input json file name";
    }

    Scheduler::init();
    std::string filename = argv[1];

    processCommands(filename);



    Scheduler::shutdown();

}

