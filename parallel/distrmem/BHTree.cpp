#include "BHTree.h"
#include <iostream>
#include <vector>
#include <map>

/* Constructor creates a Barnes-Hut tree in full */
BHTree::BHTree(std::vector<Particle>& particles)
{

    /* Determine the root cell dimensions */
    auto xVals = std::vector<float>(particles.size());
    auto yVals = std::vector<float>(particles.size());
    auto zVals = std::vector<float>(particles.size());

    for (auto &&p : particles)
    {
        xVals.push_back(p.pos.x);
        yVals.push_back(p.pos.y);
        zVals.push_back(p.pos.z);
    }

    const auto xExtrema = std::minmax_element(xVals.begin(), xVals.end());
    const auto xMin = *xExtrema.first;
    const auto xMax = *xExtrema.second;

    const auto yExtrema = std::minmax_element(yVals.begin(), yVals.end());
    const auto yMin = *yExtrema.first;
    const auto yMax = *yExtrema.second;

    const auto zExtrema = std::minmax_element(zVals.begin(), zVals.end());
    const auto zMin = *zExtrema.first;
    const auto zMax = *zExtrema.second;

    // Slightly offset bounds of starting cell from extrema
    Vec3D lowBound(xMin - 1, yMin - 1, zMin - 1);
    Vec3D highBound(xMax + 1, yMax + 1, zMax + 1);


    /* Build the Barnes-Hut tree */
    root = std::make_unique<BHNode>(BHNode(lowBound, highBound));

    for (auto &&p : particles)
    {
        insertParticle(*root, p);
    }
    
    genPhysicalInfo(*root);

}



/* 
Propagate the physical information from the leaf nodes up
to the root of the tree
*/
void BHTree::genPhysicalInfo(BHNode& node){

    // If the node has children - will need to change if child nodes
    // not all created at once as they currently are
    if (not node.octTrees.empty()){

        for (auto &&s : node.octTrees)
        {
            genPhysicalInfo(s);
        }
        
        // Accumulate mass into node and determine COM
        float m = 0;
        Vec3D com = Vec3D({0,0,0});
        for (auto &&s : node.octTrees)
        {
            m += s.getTotalMass();
            com = com + s.getCentreOfMass()*s.getTotalMass();
        }
        
        node.setCentreOfMass(com*(1/m));
        node.setTotalMass(m);

    }

}


void BHTree::insertParticle(BHNode& node, Particle& p){

    if (node.octTrees.empty()) {

        if (node.getTotalMass() == 0){
            // 'Insert' particle
            node.setTotalMass(p.getMass());
            node.setCentreOfMass(p.pos);
            node.particleID = p.ID;
            pLookup.emplace(p.ID, &p);
            p.depth = node.getDepth();
        }
        else{

            // Already a particle here, so need to branch
            auto v1 = node.getLowBound();
            auto v2 = node.getHighBound();

            auto dX = v2.x - v1.x;
            auto dY = v2.y - v1.y;
            auto dZ = v2.z - v1.z;

            auto nextDepth = node.getDepth() + 1;

            // TODO Clean this up - ideally only want to create BHNodes when absolutely necessary
            node.octTrees.emplace_back(BHNode(v1, Vec3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2}), nextDepth));
            node.octTrees.emplace_back(BHNode(Vec3D({v1.x, v1.y + dY/2, v1.z}), Vec3D({v1.x + dX/2, v2.y, v1.z + dZ/2}), nextDepth));

            node.octTrees.emplace_back(BHNode(Vec3D({v1.x, v1.y, v1.z + dZ/2}), Vec3D({v1.x + dX/2, v1.y + dY/2, v2.z}), nextDepth));
            node.octTrees.emplace_back(BHNode(Vec3D({v1.x, v1.y + dY/2, v1.z + dZ/2}), Vec3D({v1.x + dX/2, v2.y, v2.z}), nextDepth));

            node.octTrees.emplace_back(BHNode(Vec3D({v1.x + dX/2, v1.y, v1.z}), Vec3D({v2.x, v1.y + dY/2, v1.z + dZ/2}), nextDepth));
            node.octTrees.emplace_back(BHNode(Vec3D({v1.x + dX/2, v1.y + dY/2, v1.z}), Vec3D({v2.x, v2.y, v1.z + dZ/2}), nextDepth));

            node.octTrees.emplace_back(BHNode(Vec3D({v1.x + dX/2, v1.y, v1.z + dZ/2}), Vec3D({v2.x, v1.y + dY/2, v2.z}), nextDepth));
            node.octTrees.emplace_back(BHNode(Vec3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2}), v2, nextDepth));

            // Propagate the information that's currently in this node
            // down to the next level
            // Particle temp(node.getCentreOfMass(), Vec3D(0,0,0), node.getTotalMass());
            auto temp = pLookup.at(node.particleID);
            node.setTotalMass(0);
            node.particleID = -1;
            for (auto &&s : node.octTrees)
            {
                if (s.contains(node.getCentreOfMass())){
                    BHTree::insertParticle(s, *temp);
                    break;
                }
            }
            
        }

    }

    // Propagate information down to the next level
    // if it exists
    if (not node.octTrees.empty()) {
        for (auto &&s : node.octTrees)
        {
            if (s.contains(p)){
                BHTree::insertParticle(s, p);
                break;
            }
        }

    }
 
}