#pragma once

#include <functional>
#include <iostream>
#include <random>
#include <vector>
#include "Particle.h"


class DistributionSampler{

    public:

        DistributionSampler(std::function<float(float)> pdf) : pdf_(pdf) 
        { generator_.seed(rd_()); }

        std::vector<float> sample(int nSamples, float min, float max);
        
        static std::vector<Particle> samplePlummerStars(int, float, float);

    private:

        std::function<float(float)> pdf_;

        std::random_device rd_;
        std::mt19937 generator_;

};