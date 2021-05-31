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


class adder: public ICommand
{
    int m_start;
    int m_end;
    int m_delay;
public:
    adder() :ICommand("adder"), m_start(0), m_end{ 0 }, m_delay(0)
    {}
    ~adder() = default;
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
        int sum = 0;
        for (int i = m_start; i < m_end; i++)
        {
            sum += i;
            //print(". ");
            std::this_thread::sleep_for(std::chrono::milliseconds(m_delay));
        }

        return sum;
    }

};

class factorial : public ICommand
{
    int m_num;
    int m_delay;
    int fact(int num)
    {
        if (num <= 1)
        {
            return 1;
        }
        //print("~ ");
        std::this_thread::sleep_for(std::chrono::milliseconds(m_delay));
        return num * fact(num - 1);
    }
public:
    factorial() :ICommand("factorial"), m_num{ 0 }, m_delay{ 0 }
    {}
    ~factorial() override = default;
    /*virtual*/ bool parseArgs(nlohmann::json& j) override
    {
        m_num = j.contains("num") ? j["num"].get<int>() : 0;
        m_delay = j.contains("delay") ? j["delay"].get<int>() : 0;
        return true;
    }
    int operator()() override
    {
        return fact(m_num);
    }

};