#pragma once

#include "queue.hpp"
#include <thread>
#include <vector>

class Task;

class Scheduler 
{
    static std::unique_ptr<Scheduler> m_instance;
    Queue<std::shared_ptr<Task>> m_queue;
    std::vector<std::thread> m_threads;

    Scheduler();
public:
    static Scheduler* instance();
    static void init();
    static void shutdown();
    void abort();
    void schedule(std::shared_ptr<Task> task);
};