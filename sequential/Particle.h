#pragma once

#include "Vec3D.h"

class Particle
{
public:

    Particle(Vec3D, float vx, float vy, float vz, float mass);
    Particle(float x, float y, float z,
            float vx, float vy, float vz,
            float mass);

    Vec3D pos;
    Vec3D vel;

    float getMass();

private:

    float mass_;

};