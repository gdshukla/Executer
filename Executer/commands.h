#pragma once
#include <string>
#include <functional>
#include "arguments.h"
#include "task.hpp"
#include "json.hpp"
#include "utilities.hpp"
#include "stats.hpp"

class ICommand : public IArguments, public Task
{
public:
    ICommand(std::string name) : IArguments(name)
    { }
    virtual ~ICommand() = default;
};


class command1: public ICommand
{
    int m_start;
    int m_end;
    int m_delay;
public:
    command1() :ICommand("command1"), m_start(0), m_end{ 0 }, m_delay(0)
    {}
    ~command1() = default;
    /*virtual*/ bool parseArgs(nlohmann::json& j) override
    {
        m_start = j.contains("start") ? j["start"].get<int>() : 0;
        m_end = j.contains("end") ? j["end"].get<int>() : 0;
        m_delay = j.contains("delay") ? j["delay"].get<int>() : 0;
        return true;
    }

    // increment m_start by 1 and use m_delay between each iteration
    int operator()() override
    {
        int i = 0;
        for (i = m_start; i < m_end; i++)
        {
            //print(". ");
            std::this_thread::sleep_for(std::chrono::milliseconds(m_delay));
        }
        Task::m_result = m_end;
        return m_result;
    }

};

class command2 : public ICommand
{
    int m_num;
    int m_delay;
    int factorial(int num)
    {
        if (num <= 1)
        {
            return 1;
        }
        //print("~ ");
        std::this_thread::sleep_for(std::chrono::milliseconds(m_delay));
        return num * factorial(num - 1);
    }
public:
    command2() :ICommand("command2"), m_num{ 0 }, m_delay{ 0 }
    {}
    ~command2() override = default;
    /*virtual*/ bool parseArgs(nlohmann::json& j) override
    {
        m_num = j.contains("num") ? j["num"].get<int>() : 0;
        m_delay = j.contains("delay") ? j["delay"].get<int>() : 0;
        return true;
    }
    int operator()() override
    {
        m_result = factorial(m_num);
        return m_result;
    }
    int result() override
    {
        return m_result;
    }

};