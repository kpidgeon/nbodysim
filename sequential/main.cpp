#include <iostream>
#include <stdio.h>
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
    int totalSteps = 0;
    std::vector<Vec3D> accels(particles.size());

    while (timeElapsed < totalTime){

        // Create BHTree
        BHTree tree(particles);

        /* Temporary - output positions every n time steps
        if (totalSteps % 50 == 0){
            for (auto &&p : particles)
            {
                std::cout << p.pos << "  ";
            }
            std::cout << "\n";
        }
        */

        // Calculate acceleration vectors
        for (size_t i = 0; i < particles.size(); i++)
        {
            Vec3D acc(0,0,0);
            Gravity::totalAcceleration(particles[i], *(tree.root), 1, &acc);
            accels[i] = acc;
        }
        

        for (size_t i = 0; i < particles.size(); i++)
        {
            
            auto p = &particles.at(i);

            if (timeElapsed == 0){
                p->vel = p->vel + (deltaT/2)*accels.at(i); // offset velocity by half a time step
            }else{
                integrate(*p, deltaT, accels.at(i));
            }

        }

        totalSteps += 1;
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

    const float totalTime = 1;
    const float deltaT = .01;
    
    auto particles = DistributionSampler::samplePlummerStars(samples, 1, 1);

    simulate(particles, totalTime, deltaT);

    return 0;
    
}