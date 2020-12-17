#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include "BHTree.h"
#include "Gravity.h"
#include "DistributionSampler.h"


void integrate(Particle& p, const float deltaT, const Vec3D acc){

    p.pos = p.pos + p.vel*deltaT;
    p.vel = p.vel + acc*deltaT;

}


void simulate(std::vector<Particle>& particles, float totalTime, const float deltaT){

    float timeElapsed = 0;

    while (timeElapsed < totalTime){

        // Create BHTree
        BHTree tree(particles);

        auto accels = std::make_unique<std::vector<Vec3D>>();

        // Determine acceleration vector for each particle
        for (auto &&p : particles)
        {

            Vec3D acc(0,0,0);
            Gravity::totalAcceleration(p, tree.root, 1, &acc);
            accels->push_back(acc);

        }

        for (size_t i = 0; i < particles.size(); i++)
        {
            
            auto p = &particles.at(i);

            if (timeElapsed == 0){
                p->vel = p->vel + (deltaT/2)*accels->at(i); // offset velocity by half a time step
            }else{
                integrate(*p, deltaT, accels->at(i));
            }

        }

        accels->clear();
        timeElapsed += deltaT;

    }

}



int main(int argc, char* argv[]){

    Vec3D lowBound = Vec3D({0,0,0});
    Vec3D highBound = Vec3D({10,10,10});

    int samples = 10;
    if (argc >= 2){
        std::stringstream input(argv[1]);
        if (!(input >> samples)){
            samples = 10;
        }
    }

    const float totalTime = 10;
    const float deltaT = .01;
    
    std::vector<Particle> particles;
    auto p1 = Particle(Vec3D(0, 0, 0), Vec3D(0, 0, 0), 1e6);
    auto p2 = Particle(Vec3D(1e2, 0, 0), Vec3D(0, 100, 0), 1);

    particles.push_back(p1);
    particles.push_back(p2);

    simulate(particles, totalTime, deltaT);

    return 0;
    
}