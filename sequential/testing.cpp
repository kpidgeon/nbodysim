#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include "BHTree.h"
#include <random>
#include <string>
#include <cmath>
#include "Gravity.h"

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

        Particle p(randomPoint(bound1, bound2, gen), 0, 0, 0, 10);
        particles.push_back(p);
    
    }

    return particles;

}

// Print representation of a BHTree
void printCells(const std::string& prefix, const std::unique_ptr<BHNode>& node, bool isLast){

    if (node){

        std::cout << prefix;

        isLast ? std::cout << "└───" : std::cout << "├───";

        if (node->getObject())
        {
            std::cout << node->getObject()->pos << ", mass="
                    << node->getTotalMass() << ", COM = "
                    << node->getCentreOfMass() << "\n";
        }
        else{
            std::cout << "n" << ", mass=" << node->getTotalMass()
                    << ", COM = " << node->getCentreOfMass()
                    << "\n";
        }

        int n = 1;
        for (auto &&s : node->octTrees)
        {
            if (n == node->octTrees.size()){
                printCells(prefix + (isLast ? "    " : "|   "), s, true);
            }
            else{
                printCells(prefix + (isLast ? "    " : "|   "), s, false);
            }
            n++;
        }

    }


}


void printCellBounds(const std::unique_ptr<BHNode>& node){

    std::cout << node->getLowBound() << "  " << node->getHighBound() << "\n";
    for (auto &&s : node->octTrees)
    {
        if (s) { printCellBounds(s); }
    }
    

}


// void leapfrog(Particle& p, const float deltaT){
// }


// void integrate(Particle& p, const float deltaT){

//     auto v = p.vel


// }



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


    auto ensemble = genUniformEnsemble(samples, lowBound, highBound);

    // Particle p1(Vec3D(1,1,1), 0, 0, 0, 5);
    // Particle p2(Vec3D({1,9,1}), 0, 0, 0, 5);
    // Particle p3(Vec3D({9,1,1}), 0, 0, 0, 5);
    // Particle p4(Vec3D({9,9,1}), 0, 0, 0, 5);
    // Particle p5(Vec3D({1,1,9}), 0, 0, 0, 5);
    // Particle p6(Vec3D({1,9,9}), 0, 0, 0, 5);
    // Particle p7(Vec3D({9,1,9}), 0, 0, 0, 5);
    // Particle p8(Vec3D({9,9,9}), 0, 0, 0, 5);

    // Particle p9(Vec3D({5,5,5}), 0, 0, 0, 20);

    // std::vector<Particle> ensemble = {p1, p2, p3, p4, p5, p6, p7, p8, p9};

    BHTree tree(ensemble, lowBound, highBound);
    // tree.genPhysicalInfo(tree.root);

    // std::string p = "";
    // printCells(p, tree.root, true);

    for (auto &&p : ensemble)
    {
        Vec3D v(0,0,0);
        Vec3D* const acc = &v;
        Gravity::totalAcceleration(p, tree.root, 1, acc);
        std::cout << Vec3D::mag(*acc) << "\n";
    }

    // printCellBounds(tree.root);

    return 0;
}