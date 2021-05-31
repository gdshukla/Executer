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
std::vector<std::unique_ptr<ICommand>> loadCommands(std::string filename)
{
    std::vector<std::unique_ptr<ICommand>> cmdTasks;

    using json = nlohmann::json;
    std::map<std::string, nlohmann::json> cmds;
    std::ifstream ifs(filename);
    if (ifs.good())
    {
        json jf = json::parse(ifs);
        std::cout << "Loaded " << jf["commands"].size() << " commands\n";
        for (auto& c : jf["commands"]) 
        {
            std::unique_ptr<ICommand> cmd;
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

    return cmdTasks;
}

std::vector<std::shared_ptr<Task>> makeTasks(std::vector<std::unique_ptr<ICommand>> &cmdTasks)
{
    std::vector<std::shared_ptr<Task>> tasks;
    for (auto& c : cmdTasks)
    {
        auto func = [&c] {
            int result = c->operator()();
            //print(c->getName() + "\n");
            return result;
        };
        Stats::instance()->added();
        tasks.push_back(make_task(func));
    }

    std::cout << "Added " << tasks.size() << " tasks to task queue\n";
    return tasks;
}

void processCommands(std::vector<std::shared_ptr<Task>>& tasks)
{
    std::vector<std::unique_ptr<ICommand>> cmdTasks;
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
                "= stats or s: display overall task statistics\n"
                "- load or l: load new json file to add tasks to run\n"
                ">>>load commands.json\n"
                "- display or d: display stats about a certain task\n"
                "for example to display stats of task 1\n"
                ">>>d 1\n"
                "- quit or q: quit the program\n"
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
            std::string filename;
            if (input.starts_with("load "))
            {
                filename = input.substr(5);
            }
            else
            {
                filename = input.substr(2);
            }
            cmdTasks = loadCommands(filename);
            std::vector<std::shared_ptr<Task>> t = makeTasks(cmdTasks);
            for (auto& c : t)
            {
                tasks.push_back(c);
            }
            //std::cout << "loaded " << cmdTasks.size() << " tasks\n";
        }
        else if (input.starts_with("display ") || input.starts_with("d "))
        {
            int tasknum = -1;
            if (input.starts_with("display "))
            {
                tasknum = std::stoi(input.substr(8));
            }
            else
            {
                tasknum = std::stoi(input.substr(2));
            }
            if (tasknum < tasks.size())
            {
                std::cout << "Status: " << tasks[tasknum]->status() << "\n";
                std::cout << "Result: " << tasks[tasknum]->m_result << "\n";
                std::cout << "Started at: " << toTime(tasks[tasknum]->start) << "\n";
                if (tasks[tasknum]->status())
                {
                    std::cout << "Started at: " << toTime(tasks[tasknum]->end) << "\n";
                }
            }
            else
            {
                std::cout << "Invalid task number: " << tasknum << "\n";
                std::cout << "Valid values are: 0 to " << tasks.size() - 1 << "\n";
            }
        }
        else if (input == "quit" || input == "q")
        {
            break;
        }

    }
}

int main(int argc, char* argv[])
{
    if (argc < 1) {
        std::cout << "Please specify input json file name";
    }

    std::vector<std::unique_ptr<ICommand>> cmdTasks = loadCommands(argv[1]);

    Scheduler::init();

    std::vector<std::shared_ptr<Task>> tasks = makeTasks(cmdTasks);

    processCommands(tasks);



    Scheduler::shutdown();

}

