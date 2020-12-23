#pragma once

#include "Vec3D.h"

class Particle
{

public:

    static int currID;

    Particle(Vec3D, Vec3D, float);

    Vec3D pos;
    Vec3D vel;

    float getMass() const;

    int ID;

private:

    float mass_;

};