#pragma once
#include "json.hpp"
#include <string>
class IArguments
{
    std::string m_name;
public:
    IArguments(std::string name) : m_name{name}
    {}

    //virtual bool operator()(nlohmann::json& j) = 0;
    virtual bool parseArgs(nlohmann::json& j) = 0;
    virtual std::string getName() {
        return m_name;
    }
    virtual ~IArguments() = default;
};

