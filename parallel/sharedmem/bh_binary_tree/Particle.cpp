#include "Particle.h"

int Particle::currID = 0;

Particle::Particle(Vec3D p, Vec3D v, float m) : pos(p), vel(v), ID(Particle::currID), mass(m) {
        Particle::currID += 1;
        depth = 1;
}

float Particle::getMass() const{
    return mass;
}