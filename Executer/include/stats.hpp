#pragma once

#include <atomic>
#include <iostream>

class stats
{
    std::atomic_int running;
    std::atomic_int done;
    std::atomic_int total;

public:
    stats() : running{ 0 }, done{ 0 }, total{ 0 }
    {}
    void started() { running++; total++; }
    void completed() { done++; running--; }
    void display() {
        std::cout << "Tasks running  : " << running << "\n";
        std::cout << "Tasks completed: " << done << "\n";
        std::cout << "Total Tasks    : " << total << "\n";
    }
};