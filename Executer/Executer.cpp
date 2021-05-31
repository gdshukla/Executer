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

std::unique_ptr<stats> stats::m_instance;
// read data from json file
std::map<std::string, nlohmann::json> getCommandsFromFile(std::string filename)
{
    using json = nlohmann::json;
    std::map<std::string, nlohmann::json> cmds;
    std::ifstream ifs(filename);
    if (ifs.good())
    {
        json jf = json::parse(ifs);
        std::cout << jf["commands"].size() << "\n";
        for (auto& c : jf["commands"]) {
            std::cout << c["name"] << "\n";
            cmds[c["name"]] = c;
        }
    }
    return cmds;
}

std::vector<std::unique_ptr<ICommand>> loadCommandArguments(std::string filename)
{
    std::vector<std::unique_ptr<ICommand>> cmdTasks;

    using json = nlohmann::json;
    //std::map<std::string, nlohmann::json> commands = getCommandsFromFile(filename);
    using json = nlohmann::json;
    std::map<std::string, nlohmann::json> cmds;
    std::ifstream ifs(filename);
    if (ifs.good())
    {
        json jf = json::parse(ifs);
        std::cout << jf["commands"].size() << "\n";
        for (auto& c : jf["commands"]) 
        {
            std::unique_ptr<ICommand> cmd;
            std::string name = c["name"];
            std::cout << name << "\n";
            cmds[name] = c;
            if (name == "command1")
            {
                 cmd = std::make_unique<command1>();
            }
            else if (name == "command2")
            {
                cmd = std::make_unique<command2>();
            }
            cmd->parseArgs(c);
            cmdTasks.push_back(std::move(cmd));
        }
    }

    //for (auto& cmd : commands)
    //{
    //    if (cmd.first == "command1")
    //    {
    //        cmdTasks.push_back(std::make_unique<command1>());
    //    }
    //    else if (cmd.first == "command2")
    //    {
    //        cmdTasks.push_back(std::make_unique<command2>());
    //    }


    //}
    //for (auto& c : cmdTasks)
    //{
    //    if (commands.count(c->getName()) > 0)
    //    {
    //        c->parseArgs(commands.at(c->getName()));
    //    }

    //}
    return cmdTasks;
}

std::vector<std::shared_ptr<Task>> makeTasks(std::vector<std::unique_ptr<ICommand>> &cmdTasks)
{
    std::vector<std::shared_ptr<Task>> tasks;
    for (auto& c : cmdTasks)
    {
        auto func = [&c] {
            int result = c->operator()();
            print(c->getName() + "\n");
            return result;
        };
            //c.
        tasks.push_back(make_task(func));
    }
    return tasks;
}

int main(int argc, char* argv[])
{
    if (argc < 1) {
        std::cout << "Please specify input json file name";
    }

    std::vector<std::unique_ptr<ICommand>> cmdTasks = loadCommandArguments(argv[1]);


    Scheduler::init();
    auto getInt = [] {
        for(int i = 0; i < 5; i++){
            print(". ");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        return 10;
    };
    std::vector<std::shared_ptr<Task>> tasks = makeTasks(cmdTasks);
    //for (int i = 0; i < 6; i++)
    //{
    //    tasks.push_back(make_task(getInt));
    //}
    //auto getIntTask1 = make_task(getInt);
    //auto getIntTask2 = make_task(getInt);
    //auto getIntTask3 = make_task(getInt);
    //auto getIntTask4 = make_task(getInt);
    //auto getIntTask5 = make_task(getInt);
    //auto getIntTask6 = make_task(getInt);
    size_t tasksCount = tasks.size();
    while (tasksCount > 0)
    {
        stats::instance()->wait();
        stats::instance()->display();
        for (auto& t : tasks)
        {
            if (t->status())
            {
                tasksCount--;
                int res = t->result();
                std::cout << "result: " << res << "\n";
                std::cout << "start: " << toTime(t->start) << "\n";
                std::cout << "end  : " << toTime(t->end) << "\n";
            }
        }
    }
    //res = getIntTask2->result();
    //std::cout << "result: " << res << "\n";
    //res = getIntTask3->result();
    //std::cout << "result: " << res << "\n";
    //res = getIntTask4->result();
    //std::cout << "result: " << res << "\n";
    //res = getIntTask5->result();
    //std::cout << "result: " << res << "\n";
    //res = getIntTask6->result();
    //std::cout << "result: " << res << "\n";

    Scheduler::shutdown();
    std::cout << "Hello World!\n";
}

