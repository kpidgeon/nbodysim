#pragma once

#include "BHTree.h"
#include "Particle.h"
#include <memory>



class Gravity{

    public:

        static Vec3D gAcceleration(const float, const Vec3D, const float);

        static void totalAcceleration(const Particle&, const std::unique_ptr<BHNode>&, 
                                const float, Vec3D* const);

};