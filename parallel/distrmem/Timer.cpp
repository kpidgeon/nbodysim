#include "Timer.h"
#include <stdio.h>

Timer::~Timer(){

    end = clk.now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    *timeStore += duration;
    printf("%ld ms\n", (*timeStore).count());

}