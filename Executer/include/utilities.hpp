#pragma once
#include <mutex>
#include <string>
#include <iostream>
#include <chrono>

static std::mutex m_mutex;
static inline void print(std::string str)
{
    auto lock = std::lock_guard(m_mutex);
    std::cout << str;
}

static inline std::string toTime(std::chrono::system_clock::time_point tm)
{
    using namespace std::chrono;

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(tm.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(tm);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}