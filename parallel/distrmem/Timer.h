#pragma once

#include <chrono>
#include <stdio.h>



class Timer{

public:

    Timer() : start(clk.now()) {}
    Timer(std::chrono::milliseconds* pd) : start(clk.now()), timeStore(pd) {}

    ~Timer();

private:
    
    std::chrono::milliseconds* timeStore;
    std::chrono::high_resolution_clock clk;
    std::chrono::_V2::system_clock::time_point start;
    std::chrono::_V2::system_clock::time_point end;

};