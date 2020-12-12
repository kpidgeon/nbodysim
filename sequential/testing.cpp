#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include "structs.h"
#include <random>
#include <string>

#include <set>

Point3D randomPoint(Point3D bound1, Point3D bound2, std::mt19937_64& mtGen){

    std::uniform_real_distribution<float> distrX(bound1.x, bound2.x);
    std::uniform_real_distribution<float> distrY(bound1.y, bound2.y);
    std::uniform_real_distribution<float> distrZ(bound1.z, bound2.z);

    // generate random x,y,z coordinates from distributions
    auto randX = distrX(mtGen);
    auto randY = distrY(mtGen);
    auto randZ = distrZ(mtGen);

    return Point3D({randX, randY, randZ});

}

/* Generate randomly placed 'identical' particles within cuboid represented
 by two vertices */
std::vector<GravObj> genUniformEnsemble(int n, Point3D bound1, Point3D bound2){

    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::vector<GravObj> particles;

    for(int i=0; i<n; i++){

        GravObj p(randomPoint(bound1, bound2, gen), 0, 0, 0, 10);
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



int main(int argc, char* argv[]){

    Point3D lowBound = Point3D({0,0,0});
    Point3D highBound = Point3D({10,10,10});

    int samples = 100;
    if (argc >= 2){
        std::stringstream input(argv[1]);
        if (!(input >> samples)){
            samples = 100;
        }
    }


    auto ensemble = std::make_unique<std::vector<GravObj>>(genUniformEnsemble(samples, lowBound, highBound));

    // GravObj p1(Point3D({1,1,1}), 0, 0, 0, 5);
    // GravObj p2(Point3D({1,9,1}), 0, 0, 0, 5);
    // GravObj p3(Point3D({9,1,1}), 0, 0, 0, 5);
    // GravObj p4(Point3D({9,9,1}), 0, 0, 0, 5);
    // GravObj p5(Point3D({1,1,9}), 0, 0, 0, 5);
    // GravObj p6(Point3D({1,9,9}), 0, 0, 0, 5);
    // GravObj p7(Point3D({9,1,9}), 0, 0, 0, 5);
    // GravObj p8(Point3D({9,9,9}), 0, 0, 0, 5);

    // // GravObj p9(Point3D({5,5,5}), 0, 0, 0, 20);

    // std::vector<GravObj> ensemble = {p1, p2, p3, p4, p5, p6, p7, p8};

    BHTree tree(*ensemble, lowBound, highBound);
    tree.genPhysicalInfo(tree.root);

    // std::cout << "\n";

    // std::string p = "";
    // printCells(p, tree.root, true);

    return 0;
}