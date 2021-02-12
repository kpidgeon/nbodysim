#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <map>
#include <memory>
#include <vector>
// #include "BHTree.h"
#include "BinaryBHTree.h"
#include "Gravity.h"
#include "DistributionSampler.h"
#include <omp.h>
#include <chrono>
// #include "MPI_Types.h"
// #include "DistTreeUtils.h"

void integrate(Particle& p, const float deltaT, const Vec3D acc){

    p.pos = p.pos + p.vel*deltaT;
    p.vel = p.vel + acc*deltaT;

}


std::vector<std::vector<int>> greedyPartition(std::vector<Particle>& particles, size_t k){

    std::sort(particles.begin(), particles.end(),
                    [](const Particle& p1, const Particle& p2){ return (p1.depth > p2.depth); });

    std::vector<std::vector<int>> partitions;
    std::vector<int> workSums;
    for (size_t i = 0; i < particles.size(); i++)
    {
        int currShortest = 0;
        int n = -1;
        for (size_t j = 0; j < k; j++)
        {
            if (j < partitions.size()){
                
                if (workSums.at(j) < currShortest || n == -1){
                    currShortest = workSums.at(j);
                    n = j;
                }

            }else{
                partitions.emplace_back(std::vector<int>());
                workSums.push_back(0);
                n = 0;
            }
        }

        partitions.at(n).push_back(particles.at(i).ID);
        workSums.at(n) += particles.at(i).depth;
        
    }

    return partitions;

}


void simulate(std::vector<Particle>& particles, float totalTime, const float deltaT, const int num_threads){

    float timeElapsed = 0;
    int totalSteps = 0;
    std::vector<Vec3D> accels(particles.size());

    while (timeElapsed < totalTime){

        auto partitions = greedyPartition(particles, num_threads);

        BinaryBHTree tree(particles);

        #pragma omp parallel num_threads(num_threads)
        {

            auto id = omp_get_thread_num();

            for (size_t i = 0; i < partitions.at(id).size(); i++)
            {
                auto particleID = partitions.at(id).at(i);
                Vec3D acc(0,0,0);
                Gravity::totalAcceleration(*tree.pLookup.at(particleID), *(tree.root), 1, &acc);
                accels[particleID] = acc;
            }

            
            for (size_t i = 0; i < partitions.at(id).size(); i++)
            {

                auto particleID = partitions.at(id).at(i);

                auto p = tree.pLookup.at(particleID);

                if (timeElapsed == 0){
                    p->vel = p->vel + (deltaT/2)*accels.at(particleID); // offset velocity by half a time step
                }else{
                    integrate(*p, deltaT, accels.at(particleID));
                }
            
            }
            
        }

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
        // #pragma omp parallel for// schedule(dynamic,1)
        // for (size_t i = 0; i < particles.size(); i++)
        // {
        //     Vec3D acc(0,0,0);
        //     Gravity::totalAcceleration(particles[i], *(tree.root), 1, &acc);
        //     accels[i] = acc;
        // }
        
        // #pragma omp parallel for
        // for (size_t i = 0; i < particles.size(); i++)
        // {
            
        //     auto p = &particles.at(i);

        //     if (timeElapsed == 0){
        //         p->vel = p->vel + (deltaT/2)*accels.at(i); // offset velocity by half a time step
        //     }else{
        //         integrate(*p, deltaT, accels.at(i));
        //     }

        // }

        totalSteps += 1;
        timeElapsed += deltaT;

    }

}



int main(int argc, char* argv[]){

    int samples = 10;
    if (argc >= 2){
        std::stringstream input(argv[1]);
        if (!(input >> samples)){
            samples = 10;
        }
    }

    int num_threads = omp_get_max_threads();
    if(argc >= 3){
        std::stringstream input(argv[2]);
        if (!(input >> num_threads)){
            num_threads = omp_get_max_threads();
        }
    }

    const float totalTime = 100;
    const float deltaT = .01;
    
    auto particles = DistributionSampler::samplePlummerStars(samples, 1, 1);
    // Particle p1(Vec3D(1,1,1), Vec3D(0,0,0), 1);
    // Particle p2(Vec3D(9,9,9), Vec3D(0,0,0), 1);

    // std::vector<Particle> particles = {p1, p2};

    {
        // std::chrono::high_resolution_clock clk;
        // auto start = clk.now();
        simulate(particles, totalTime, deltaT, num_threads);
        // auto end = clk.now();
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        // printf("%ld \n", duration);
    }

    return 0;
    
}