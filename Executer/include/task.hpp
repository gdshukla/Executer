#pragma once
#include <functional>
#include <future>
#include <vector>
#include <mutex>
#include <chrono>
#include <ctime>
#include <any>
#include "Scheduler.h"
#include "stats.hpp"

// task interface
class ITask 
{
public:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    virtual std::any operator()() = 0;
    virtual ~ITask() = default;
    virtual std::any result() = 0;
};

class basic_task : public ITask 
{
protected:
    std::shared_future<std::any> m_future;
    std::mutex m_mutex;
    std::vector<std::function<std::any()>> m_actions;
    bool m_scheduled = false;

public:
    virtual std::any result()
    {
        return m_future.get();
    }

    void addAction(std::function<std::any()> action)
    {
        auto lock = std::lock_guard<std::mutex>(m_mutex);
        if (m_scheduled) 
        {
            action();
        }
        else 
        {
            m_actions.push_back(action);
        }
    }
    std::any schedule()
    {
        {
            auto lock = std::lock_guard<std::mutex>(m_mutex);
            m_scheduled = true;
        }
        start = std::chrono::system_clock::now();
        Stats::instance()->started();
        std::any result = 0;
        for (auto& action : m_actions) 
        {
            result = action();
        }
        m_actions.pop_back();
        Stats::instance()->completed();
        end = std::chrono::system_clock::now();
        return result;
    }
};

using F = std::function<std::any(void)>;
class Task : public basic_task
{
protected:
    std::promise<std::any> m_promise;
    F m_func;
public:
    std::any m_result;
    bool m_checked;
    Task() : basic_task(), m_result{ 0 }, m_checked{ false }{}
    Task(F func) : basic_task(), m_result{ 0 }, m_func(func), m_checked{ false }
    {
        basic_task::m_future = m_promise.get_future();
        basic_task::addAction(m_func);
    }
    bool status()
    {
        return std::future_status::ready == m_future.wait_until(std::chrono::system_clock::now());
    }
    virtual std::any operator()() override
    {
        m_result = basic_task::schedule();
        m_promise.set_value(m_result);
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
