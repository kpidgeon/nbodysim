#pragma once

#include "BHNode.h"
#include <memory>
#include "Particle.h"
#include <vector>
#include "Vec3D.h"
#include <algorithm>
#include <map>

// Wrapper for the Barnes-Hut tree
class BHTree
{

    public:

        BHTree(std::vector<Particle>&);

        // When a BHTree instance goes out of scope
        // the memory deallocation will propagate down the
        // tree
        std::unique_ptr<BHNode> root;

        std::map<int, Particle* const> pLookup;

        void insertParticle(BHNode&, Particle& p);
        void genPhysicalInfo(BHNode&);

        std::vector<BHNode> contigTree();

};