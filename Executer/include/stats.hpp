#pragma once

#include <atomic>
#include <iostream>
#include <condition_variable>

// singleton class to keep track of task stats
class Stats
{
    std::atomic_int m_running;
    std::atomic_int m_completed;
    std::atomic_int m_total;
    bool m_ready = false;
    std::condition_variable m_cv;
    std::mutex m_mutex;

    Stats() : m_running{ 0 }, m_completed{ 0 }, m_total{ 0 }
    {}
    static std::unique_ptr<Stats> m_instance;

public:
    int running() 
    { 
        return m_running; 
    }
    void wait() 
    {
        auto lock = std::unique_lock<std::mutex>(m_mutex);
        m_cv.wait(lock, [this] { return m_ready; });
        m_ready = false;
    }
    void started() 
    { 
        m_running++;
    }
    void added() 
    { 
        m_total++;
    }
    void completed() 
    { 
        m_completed++; m_running--;
        auto lock = std::unique_lock<std::mutex>(m_mutex);
        m_ready = true;
        m_cv.notify_one();
    }
    void display() 
    {
        std::cout << "Tasks running  : " << m_running << "\n";
        std::cout << "Tasks completed: " << m_completed << "\n";
        std::cout << "Total Tasks    : " << m_total << "\n";
    }
    static Stats* instance()
    {
        if (!m_instance)
        {
            m_instance.reset(new Stats);
        }
        return m_instance.get();
    }
};
