#include "Particle.h"


Particle::Particle(Vec3D ini_pos, float ini_vx, float ini_vy, float ini_vz, float mass)
    : pos(ini_pos), vel(ini_vx, ini_vy, ini_vz), mass_(mass)
{}

float Particle::getMass() const{
    return mass_;
}