#include "BHTree.h"
#include <iostream>
#include <vector>


BHTree::BHTree(const std::vector<Particle>& particles, Vec3D& lowBound, Vec3D& highBound)
{

    root = std::make_unique<BHNode>(BHNode(lowBound, highBound));

    for (auto &&p : particles)
    {
        insertParticle(root, p);
    }
    

}


void BHTree::insertParticle(std::unique_ptr<BHNode>& node, const Particle& p){

    if (not node->getObject()){
        
        // Currently all child nodes are created together, so
        // if one is non-null, they all are
        bool noChildren = true;
        for (auto &&s : node->octTrees)
        {
            if (s){
                noChildren = false;
                if (s->contains(p)){
                    BHTree::insertParticle(s, p);
                    break; // Handles case where a particle is on a boundary
                            // and could be 'contained' within multiple cells
                }
            }
        }
        if (noChildren) node->setObject(p);
        
    }
    else{

        // TODO Handle case where two particles exist at the same location
        // as recursion continues forever

        // There are two particles in the same cell,
        // so need to branch
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

        // The particle p needs to be inserted
        // into the lower level...
        for (auto &&s : node->octTrees)
        {
            if (s->contains(p)){
                BHTree::insertParticle(s, p);
                break;
            }
        }
        
        // ...but the particle that was already in this
        // cell also needs to be propagated down to a
        // finer sub-volume...
        auto obj = node->getObject();
        node->clearObject();

        for (auto &&s : node->octTrees)
        {
            if (s->contains(*obj)){
                BHTree::insertParticle(s, *obj);
                break;
            }
        }
        
    }
}

// TODO Would make more sense inside of BHTree constructor
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
    else{

        if (node->getObject()){
            auto p = *node->getObject();
            node->setTotalMass(p.getMass());
            node->setCentreOfMass(p.pos);
        }
        else{
            // Temporary - ideally won't consider this case
            node->setTotalMass(0);
            node->setCentreOfMass(Vec3D({0,0,0}));
        }

    }

}