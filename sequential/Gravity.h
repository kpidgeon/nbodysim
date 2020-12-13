#pragma once

#include "BHTree.h"
#include "Particle.h"
#include <memory>



class Gravity{

    public:

        static float gAcceleration(const float, const float, const float);

        static void totalAcceleration(const Particle&, const std::unique_ptr<BHNode>&, 
                                const float, float* const);

};