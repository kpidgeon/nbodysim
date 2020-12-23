#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include "BHTree.h"
#include <random>
#include <string>
#include "Gravity.h"
#include "DistributionSampler.h"

Vec3D randomPoint(Vec3D bound1, Vec3D bound2, std::mt19937_64& mtGen){

    std::uniform_real_distribution<float> distrX(bound1.x, bound2.x);
    std::uniform_real_distribution<float> distrY(bound1.y, bound2.y);
    std::uniform_real_distribution<float> distrZ(bound1.z, bound2.z);

    // generate random x,y,z coordinates from distributions
    auto randX = distrX(mtGen);
    auto randY = distrY(mtGen);
    auto randZ = distrZ(mtGen);

    return Vec3D({randX, randY, randZ});

}

/* Generate randomly placed 'identical' particles within cuboid represented
 by two vertices */
std::vector<Particle> genUniformEnsemble(int n, Vec3D bound1, Vec3D bound2){

    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::vector<Particle> particles;

    for(int i=0; i<n; i++){

        Particle p(randomPoint(bound1, bound2, gen), Vec3D(0,0,0), 1);
        particles.push_back(p);
    
    }

    return particles;

}

// Print representation of a BHTree
void printCells(const std::string& prefix, const BHNode& node, bool isLast){

    std::cout << prefix;

    isLast ? std::cout << "└───" : std::cout << "├───";

    std::cout << "mass=" << node.getTotalMass()
            << ", COM = " << node.getCentreOfMass()
            << "\n";

    for (size_t i = 0; i < node.octTrees.size(); i++)
    {
        if (i == node.octTrees.size() - 1)
            printCells(prefix + (isLast ? "    " : "|   "), node.octTrees.at(i), true);
        else 
            printCells(prefix + (isLast ? "    " : "|   "), node.octTrees.at(i), false);
    }

}
