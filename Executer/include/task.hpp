#pragma once
#include <functional>
#include <future>
#include <vector>
#include <mutex>
#include <chrono>
#include <ctime>

#include "Scheduler.h"
#include "stats.hpp"

// task interface
class ITask 
{
public:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    virtual int operator()() = 0;
    virtual ~ITask() = default;
    virtual int result() = 0;
};

class basic_task : public ITask 
{
protected:
    std::shared_future<int> m_future;
    std::mutex m_mutex;
    std::vector<std::function<int()>> m_actions;
    bool m_scheduled = false;

public:
    virtual int result() {
        return m_future.get();
    }

    void addAction(std::function<int()> action) {
        auto lock = std::lock_guard(m_mutex);
        if (m_scheduled) 
        {
            action();
        }
        else 
        {
            m_actions.push_back(action);
        }
    }
    int schedule() {
        {
            auto lock = std::lock_guard(m_mutex);
            m_scheduled = true;
        }
        start = std::chrono::system_clock::now();
        stats::instance()->started();
        int result = 0;
        for (auto& action : m_actions) {
            result = action();
        }
        stats::instance()->completed();
        end = std::chrono::system_clock::now();
        return result;
    }
};

using F = std::function<int(void)>;
class Task : public basic_task
{
protected:
    std::promise<int> m_promise;
    int m_result;
    F m_func;
public:
    Task() : basic_task(), m_result{ 0 } {}
    Task(F func) : basic_task(), m_result{ 0 }, m_func(func)
    {
        basic_task::m_future = m_promise.get_future();
        basic_task::addAction(m_func);  
    }
    bool status()
    {
        return std::future_status::ready == m_future.wait_until(std::chrono::system_clock::now());
    }
    virtual int operator()() override
    {
        m_promise.set_value(basic_task::schedule());
        return m_result;
    }

};



//template <class F>
inline auto make_task(F f) 
{
    auto task = std::make_shared<Task>(f);
    Scheduler::instance()->schedule(task);
    return task;
}
