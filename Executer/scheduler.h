#pragma once

#include "queue.hpp"
#include <thread>
#include <vector>

class ITask;

class Scheduler 
{
    static std::unique_ptr<Scheduler> m_instance;
    Queue<std::shared_ptr<ITask>> m_queue;
    std::vector<std::thread> m_threads;

    Scheduler();
public:
    static Scheduler* instance();
    static void init();
    static void shutdown();
    void schedule(std::shared_ptr<ITask> task);
};