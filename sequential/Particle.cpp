#include "Particle.h"

int Particle::currID = 0;

Particle::Particle(Vec3D p, Vec3D v, float m) : pos(p), vel(v), mass_(m), ID(Particle::currID) {
        Particle::currID += 1;
}

float Particle::getMass() const{
    return mass_;
}