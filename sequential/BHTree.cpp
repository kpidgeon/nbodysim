#include "BHTree.h"
#include <iostream>
#include <vector>

/* Constructor creates a Barnes-Hut tree in full */
BHTree::BHTree(const std::vector<Particle>& particles)
{

    /* Determine the root cell dimensions */
    auto xVals = std::make_unique<std::vector<float>>();
    auto yVals = std::make_unique<std::vector<float>>();
    auto zVals = std::make_unique<std::vector<float>>();

    for (auto &&p : particles)
    {
        xVals->push_back(p.pos.x);
        yVals->push_back(p.pos.y);
        zVals->push_back(p.pos.z);

        // std::cout << p.pos << "  ";
    }
    // std::cout << "\n";

    const auto xExtrema = std::minmax_element(xVals->begin(), xVals->end());
    const auto xMin = *xExtrema.first;
    const auto xMax = *xExtrema.second;

    const auto yExtrema = std::minmax_element(yVals->begin(), yVals->end());
    const auto yMin = *yExtrema.first;
    const auto yMax = *yExtrema.second;

    const auto zExtrema = std::minmax_element(zVals->begin(), zVals->end());
    const auto zMin = *zExtrema.first;
    const auto zMax = *zExtrema.second;

    // Slightly offset bounds of starting cell from extrema
    Vec3D lowBound(xMin - 1, yMin - 1, zMin - 1);
    Vec3D highBound(xMax + 1, yMax + 1, zMax + 1);


    /* Build the Barnes-Hut tree */
    root = std::make_unique<BHNode>(BHNode(lowBound, highBound));

    for (auto &&p : particles)
    {
        insertParticle(root, p);
    }
    
    genPhysicalInfo(root);

}



/* 
Propagate the physical information from the leaf nodes up
to the root of the tree
*/
void BHTree::genPhysicalInfo(std::unique_ptr<BHNode>& node){

    // If the node has children - will need to change if child nodes
    // not all created at once as they currently are
    if (node->octTrees[0] != nullptr){

        for (auto &&s : node->octTrees)
        {
            genPhysicalInfo(s);
        }
        
        // Accumulate mass into node and determine COM
        float m = 0;
        Vec3D com = Vec3D({0,0,0});
        for (auto &&s : node->octTrees)
        {
            m += s->getTotalMass();
            com = com + s->getCentreOfMass()*s->getTotalMass();
        }
        
        node->setCentreOfMass(com*(1/m));
        node->setTotalMass(m);

    }

}




void BHTree::insertParticle(std::unique_ptr<BHNode>& node, const Particle& p){

    if (not node->octTrees.at(0)) {

        if (node->getTotalMass() == 0){
            // 'Insert' particle
            node->setTotalMass(p.getMass());
            node->setCentreOfMass(p.pos);
        }
        else{

            // Already a particle here, so need to branch
            auto v1 = node->getLowBound();
            auto v2 = node->getHighBound();

            auto dX = v2.x - v1.x;
            auto dY = v2.y - v1.y;
            auto dZ = v2.z - v1.z;

            // TODO Clean this up - ideally only want to create BHNodes when absolutely necessary
            node->octTrees[0] = std::make_unique<BHNode>(BHNode(v1, Vec3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2})));
            node->octTrees[1] = std::make_unique<BHNode>(BHNode(Vec3D({v1.x, v1.y + dY/2, v1.z}), Vec3D({v1.x + dX/2, v2.y, v1.z + dZ/2})));

            node->octTrees[2] = std::make_unique<BHNode>(BHNode(Vec3D({v1.x, v1.y, v1.z + dZ/2}), Vec3D({v1.x + dX/2, v1.y + dY/2, v2.z})));
            node->octTrees[3] = std::make_unique<BHNode>(BHNode(Vec3D({v1.x, v1.y + dY/2, v1.z + dZ/2}), Vec3D({v1.x + dX/2, v2.y, v2.z})));

            node->octTrees[4] = std::make_unique<BHNode>(BHNode(Vec3D({v1.x + dX/2, v1.y, v1.z}), Vec3D({v2.x, v1.y + dY/2, v1.z + dZ/2})));
            node->octTrees[5] = std::make_unique<BHNode>(BHNode(Vec3D({v1.x + dX/2, v1.y + dY/2, v1.z}), Vec3D({v2.x, v2.y, v1.z + dZ/2})));

            node->octTrees[6] = std::make_unique<BHNode>(BHNode(Vec3D({v1.x + dX/2, v1.y, v1.z + dZ/2}), Vec3D({v2.x, v1.y + dY/2, v2.z})));
            node->octTrees[7] = std::make_unique<BHNode>(BHNode(Vec3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2}), v2));

            // Propagate the information that's currently in this node
            // down to the next level
            Particle temp(node->getCentreOfMass(), Vec3D(0,0,0), node->getTotalMass());
            node->setTotalMass(0);
            for (auto &&s : node->octTrees)
            {
                if (s->contains(node->getCentreOfMass())){
                    BHTree::insertParticle(s, temp);
                    break;
                }
            }
            
        }

    }

    // Propagate information down to the next level
    // if it exists
    if (node->octTrees.at(0)){
        for (auto &&s : node->octTrees)
        {
            if (s->contains(p)){
                BHTree::insertParticle(s, p);
                break;
            }
        }

    }
 
}