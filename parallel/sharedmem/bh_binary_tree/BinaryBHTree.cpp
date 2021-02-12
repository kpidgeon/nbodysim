#include "BinaryBHTree.h"
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>


BinaryBHTree::BinaryBHTree(std::vector<Particle>& particles){

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
    root = std::make_unique<BinaryBHNode>(BinaryBHNode(lowBound, highBound));
    root->depth = 0;

    for (auto &&p : particles)
    {
        insertParticle(*root, p);
    }
    genPhysicalInfo(*root);

}


/*
Given a cuboid region that a Particle is known to be in, find the sub-octant
that the Particle is within.
*/
std::pair<Vec3D, Vec3D> BinaryBHTree::BinaryBHNode::getSubRegion(const Particle& p) const {

    auto midpoint = (lowBound + highBound) * 0.5;

    auto newLowX = p.pos.x > midpoint.x ? midpoint.x : lowBound.x;
    auto newLowY = p.pos.y > midpoint.y ? midpoint.y : lowBound.y;
    auto newLowZ = p.pos.z > midpoint.z ? midpoint.z : lowBound.z;

    auto newHighX = p.pos.x > midpoint.x ? highBound.x : midpoint.x;
    auto newHighY = p.pos.y > midpoint.y ? highBound.y : midpoint.y;
    auto newHighZ = p.pos.z > midpoint.z ? highBound.z : midpoint.z;

    Vec3D newLowBound(newLowX, newLowY, newLowZ);
    Vec3D newHighBound(newHighX, newHighY, newHighZ);

    return std::pair<Vec3D, Vec3D>(newLowBound, newHighBound);

}



/*
Given a cuboid region represented by a node, that the Particle is not within,
find the cell (sibling) region (with the same parent region) that contains the Particle.
*/
std::pair<Vec3D, Vec3D> BinaryBHTree::BinaryBHNode::getSiblingRegion(const Particle& p) const {

    auto distX = highBound.x - lowBound.x;
    auto distY = highBound.y - lowBound.y;
    auto distZ = highBound.z - lowBound.z;
    auto dist = highBound - lowBound;

    Vec3D l = lowBound;

    if ( p.pos.x < lowBound.x )
        l.x -= distX;
    else if ( p.pos.x >= highBound.x )
        l.x += distX;

    if ( (p.pos.y < lowBound.y) )
        l.y -= distY;
    else if ( p.pos.y >= highBound.y )
        l.y += distY;


    if ( (p.pos.z < lowBound.z) )
        l.z -= distZ;
    else if ( p.pos.z >= highBound.z )
        l.z += distZ;

    Vec3D h = l + dist;

    return std::pair<Vec3D, Vec3D>(l, h);

}


/*
Insert a particle into the tree.
*/
void BinaryBHTree::insertParticle(BinaryBHNode& node, Particle& p){

    if (node.depth > maxDepth)
        maxDepth = node.depth;

    if (node.contains(p)){

        if ( node.left )
            insertParticle(*node.left, p);
        else{

            if ( node.particleID != -1 ){

                auto currentlyHere = pLookup.at(node.particleID);

                auto bounds = node.getSubRegion(p);
                node.left = std::make_unique<BinaryBHNode>(bounds.first, bounds.second);
                node.left->depth = node.depth + 1;

                insertParticle(*node.left, p);
                insertParticle(*node.left, *currentlyHere);

                // Reset node
                node.particleID = -1;
                node.com = Vec3D(0,0,0);
                node.mass = 0;

            }
            else{

                node.particleID = p.ID;
                node.com = p.pos;
                node.mass = p.getMass();
                pLookup.emplace(p.ID, &p);

            }

        }
        
    }
    else{

        if ( !node.right ){

            auto bounds = node.getSiblingRegion(p);
            node.right = std::make_unique<BinaryBHNode>(bounds.first, bounds.second);
            node.right->depth = node.depth + 1;
        
        }
        insertParticle(*node.right, p);

    }
    
}


/*
Propagate physical information (centre-of-mass, mass) from the leaves to
the root of the tree.
*/
void BinaryBHTree::genPhysicalInfo(BinaryBHTree::BinaryBHNode& node){

    if (node.left){

        node.mass = 0;
        node.com.x = 0;
        node.com.y = 0;
        node.com.z = 0;

        auto temp = node.left.get();
        while (temp)
        {

            genPhysicalInfo(*temp);

            node.mass += temp->mass;
            node.com = node.com + temp->com*temp->mass;

            temp = temp->right.get();
        
        }
        node.com = node.com * (1/node.mass);

    }

}



// std::vector<BinaryBHTree::BinaryBHNode::MinimalBHNode> BinaryBHTree::toContigFormat() const {

//     std::queue<BinaryBHNode*> q;

//     // Reserve enough space for a complete tree
//     std::vector<BinaryBHNode::MinimalBHNode> minimalData;//std::pow(2, maxDepth+1));

//     // q.push(*root.get());
    
//     // Do a breadth-first-search
    
//     // For each node, if it is not one of the nodes at
//     // the maximum depth of the tree, we need to generate two child nodes
//     // for it.

//     q.push(root.get());
//     BinaryBHNode* temp = q.front();
//     while ( !q.empty() ){

//         temp = q.front();
//         q.pop();
        
//         if ( temp->depth < (maxDepth - 1) ){

//             if ( !temp->left ){
//                 temp->left = std::make_unique<BinaryBHNode>();
//                 temp->left->depth = temp->depth + 1;
//             }

//             if ( !temp->right && temp->depth != 0){
//                 temp->right = std::make_unique<BinaryBHNode>();
//                 temp->right->depth = temp->depth + 1;
//             }

//             q.push(temp->left.get());
            
//             if (temp->depth != 0)
//                 q.push(temp->right.get());

//         }

//         minimalData.push_back(temp->toMinimal());

//     }

//     return minimalData;

// }