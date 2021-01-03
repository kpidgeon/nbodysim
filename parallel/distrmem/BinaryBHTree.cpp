#include "BinaryBHTree.h"
#include <algorithm>
#include <vector>
#include <queue>


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
std::pair<Vec3D, Vec3D> BinaryBHTree::getSubRegion(const BinaryBHNode& node, const Particle& p) const {

    auto midpoint = (node.lowBound + node.highBound) * 0.5;

    auto newLowX = p.pos.x > midpoint.x ? midpoint.x : node.lowBound.x;
    auto newLowY = p.pos.y > midpoint.y ? midpoint.y : node.lowBound.y;
    auto newLowZ = p.pos.z > midpoint.z ? midpoint.z : node.lowBound.z;

    auto newHighX = p.pos.x > midpoint.x ? node.highBound.x : midpoint.x;
    auto newHighY = p.pos.y > midpoint.y ? node.highBound.y : midpoint.y;
    auto newHighZ = p.pos.z > midpoint.z ? node.highBound.z : midpoint.z;

    Vec3D newLowBound(newLowX, newLowY, newLowZ);
    Vec3D newHighBound(newHighX, newHighY, newHighZ);

    return std::pair<Vec3D, Vec3D>(newLowBound, newHighBound);

}



/*
Given a cuboid region represented by a node, that the Particle is not within,
find the cell (sibling) region (with the same parent region) that contains the Particle.
*/
std::pair<Vec3D, Vec3D> BinaryBHTree::getSiblingRegion(const BinaryBHNode& node, const Particle& p) const {

    auto distX = node.highBound.x - node.lowBound.x;
    auto distY = node.highBound.y - node.lowBound.y;
    auto distZ = node.highBound.z - node.lowBound.z;

    auto newLowX = p.pos.x < node.lowBound.x ? node.lowBound.x - distX : node.lowBound.x + (p.pos.x > node.highBound.x)*distX;
    auto newLowY = p.pos.y < node.lowBound.y ? node.lowBound.y - distY : node.lowBound.y + (p.pos.y > node.highBound.y)*distY;
    auto newLowZ = p.pos.z < node.lowBound.z ? node.lowBound.z - distZ : node.lowBound.z + (p.pos.z > node.highBound.z)*distZ;

    Vec3D lowBound(newLowX, newLowY, newLowZ);
    Vec3D highBound(newLowX + distX, newLowY + distY, newLowZ + distZ);

    return std::pair<Vec3D, Vec3D>(lowBound, highBound);

}


bool BinaryBHTree::BinaryBHNode::contains(const Particle& p) const {

    return (p.pos.x >= lowBound.x && p.pos.x < highBound.x
        && p.pos.y >= lowBound.y && p.pos.y < highBound.y
        && p.pos.z >= lowBound.z && p.pos.z < highBound.z);

}


/*
Insert a particle into the tree.
*/
void BinaryBHTree::insertParticle(BinaryBHNode& node, Particle& p){

    if (not node.contains(p)){

        if (!node.right)
        {
            auto bounds = getSiblingRegion(node, p);
            node.right = std::make_unique<BinaryBHNode>(bounds.first, bounds.second);
        }
        insertParticle(*node.right, p);

    }
    else{

        if (node.particleID == -1 && !node.left){

            // 'Insert' particle at this node
            node.particleID = p.ID;
            node.com = p.pos;
            node.mass = p.getMass();

            pLookup.emplace(p.ID, &p);
            
        }
        else if (node.particleID != -1) {

            // Already a particle here, so need to branch
            auto bounds = getSubRegion(node, *pLookup.at(node.particleID));
            node.left = std::make_unique<BinaryBHNode>(bounds.first, bounds.second);

            insertParticle(*node.left, *pLookup.at(node.particleID));

            node.particleID = -1;
            node.com = Vec3D(0,0,0);
            node.mass = 0;

        }

        if (node.left)
            insertParticle(*node.left, p);

    }

}


/*
Propagate physical information (centre-of-mass, mass) from the leaves to
the root of the tree.
*/
void BinaryBHTree::genPhysicalInfo(BinaryBHNode& node){

    if (node.left){

        node.mass = 0;
        node.com.x = 0;
        node.com.y = 0;
        node.com.z = 0;

        auto temp = node.left.get();

        std::queue<BinaryBHNode*> q;
        q.push(temp);

        while (temp->right)
        {
            q.push(temp->right.get());
            temp = temp->right.get();
        }
        
        while (!q.empty())
        {
            auto t = q.front();

            genPhysicalInfo(*t);

            node.mass += t->mass;
            node.com = node.com + t->com*t->mass;

            q.pop();
        }
        node.com = node.com * (1/node.mass);

    }

}