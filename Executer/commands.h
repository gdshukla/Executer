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
    virtual std::string resultToString(std::any& result) = 0;
    virtual ~ICommand() = default;
};


class adder: public ICommand
{
    int m_start;
    int m_end;
    int m_delay;
    std::vector<std::string> args = { 
                                        {"start"}, 
                                        {"end"}, 
                                        {"delay"} 
                                    };
public:
    adder() :ICommand("adder"), m_start(0), m_end{ 0 }, m_delay(0)
    {}
    ~adder() = default;
    /*virtual*/ bool parseArgs(nlohmann::json& j) override
    {
        m_start = j.contains(args[0]) ? j[args[0]].get<int>() : 0;
        m_end = j.contains(args[1]) ? j[args[1]].get<int>() : 0;
        m_delay = j.contains(args[2]) ? j[args[2]].get<int>() : 0;
        return true;
    }

    // increment m_start by 1 and use m_delay between each iteration
    std::any operator()() override
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
    virtual std::string resultToString(std::any& result)
    {
        if (m_result.has_value())
        {
            
            int value = std::any_cast<int>(result);
            return std::to_string(value);
        }
        return "no results";
        //return m_result.has_value() ? std::to_string(std::any_cast<int>(m_result)) : "result not available";
    }
    std::string argumentsToString()
    {
        std::string result = getName() + "(";
        result += args[0] + "=" + std::to_string(m_start) + ", ";
        result += args[1] + "=" + std::to_string(m_end) + ", ";
        result += args[2] + "=" + std::to_string(m_delay);
        result += ")";
        return result;
    }
};

class factorial : public ICommand
{
    int m_num;
    int m_delay;
    long long result = 0;
    std::vector<std::string> args = {
                                        {"num"},
                                        {"delay"}
    };
    long long fact(int num)
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
        m_num = j.contains(args[0]) ? j[args[0]].get<int>() : 0;
        m_delay = j.contains(args[1]) ? j[args[1]].get<int>() : 0;
        return true;
    }
    std::any operator()() override
    {
        result = fact(m_num);
        return result;
    }
    virtual std::string resultToString(std::any& result)
    {
        return result.has_value() ? std::to_string(std::any_cast<long long>(result)) : "result not available";
    }
    std::string argumentsToString()
    {
        std::string result = getName() + "(";
        result += args[0] + "=" + std::to_string(m_num) + ", ";
        result += args[1] + "=" + std::to_string(m_delay);
        result += ")";
        return result;
    }

};
