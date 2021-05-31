#include "scheduler.h"
#include "task.hpp"
#include <functional>
#include <optional>

std::unique_ptr<Scheduler> Scheduler::m_instance;

void Scheduler::init() {
    m_instance.reset(new Scheduler());
}

void Scheduler::shutdown() {
    auto i = m_instance.get();

    for (auto j = 0u; j < i->m_threads.size(); ++j) {
        i->m_queue.push({});
    }

    for (auto& th : i->m_threads) {
        th.join();
    }
}

void Scheduler::schedule(std::shared_ptr<ITask> task) {
    m_queue.push(task);
}

Scheduler::Scheduler() {
    auto func = [&] {
        while(1) {
            std::shared_ptr<ITask> task;
            m_queue.pop(task);
            if (!task)
                return;
            (*task)();
        }
    };

    m_threads.resize(std::thread::hardware_concurrency());
    for (auto& t : m_threads) {
        t = std::thread(func);
    }
}


Scheduler* Scheduler::instance() {
    return m_instance.get();
}


