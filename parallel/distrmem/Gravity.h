#pragma once

#include "BHTree.h"
#include "BinaryBHTree.h"
#include "Particle.h"
#include <memory>



class Gravity{

    public:

        static Vec3D gAcceleration(const float, const Vec3D, const float);
        static Vec3D gAcceleration(const float, const Vec3D, const float, const float);

        static void totalAcceleration(const Particle&, const BHNode&, 
                                const float, Vec3D* const);

        static void totalAcceleration(const Particle&, const BinaryBHTree::BinaryBHNode&, 
                                const float, Vec3D* const);

};