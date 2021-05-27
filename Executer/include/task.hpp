#pragma once


// task interface
struct ITask 
{
    virtual void operator()() = 0;
    virtual ~ITask() = default;
};

