#pragma once
#include <chrono>

template <typename Function>
int measureTimeMs(Function f)
{
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count();
}

template <typename Function>
int measureTimeMicroS(Function f)
{
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count();
}