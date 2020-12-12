#pragma once

#include "BHNode.h"
#include <memory>
#include "Particle.h"
#include <vector>
#include "Vec3D.h"


// Wrapper for the Barnes-Hut tree
class BHTree
{

    public:

        BHTree(const std::vector<Particle>&, Vec3D&, Vec3D&);

        // When a BHTree instance goes out of scope
        // the memory deallocation will propagate down the
        // tree
        std::unique_ptr<BHNode> root;

        void insertParticle(std::unique_ptr<BHNode>&, const Particle& p);

        void genPhysicalInfo(std::unique_ptr<BHNode>&);

};