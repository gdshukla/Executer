#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

// thread safe queue
class ITask;

template <class T>
class Queue 
{
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;

public:
    void push(T value) 
    {
        auto lock = std::unique_lock(m_mutex);
        m_queue.push(value);
        m_cv.notify_one();
    }

    void pop(T& value) 
    {
        auto lock = std::unique_lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_queue.empty(); });
        value = m_queue.front();
        m_queue.pop();
    }
};
