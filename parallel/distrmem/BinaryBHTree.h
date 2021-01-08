#pragma once

#include <memory>
#include "Vec3D.h"
#include <vector>
#include <map>
#include "Particle.h"
#include "BaseBinaryNode.h"

class BinaryBHTree
{

public:

    BinaryBHTree(std::vector<Particle>&);

    struct BinaryBHNode : BaseBinaryNode {

        using BaseBinaryNode::BaseBinaryNode;

        std::unique_ptr<BinaryBHNode> left = nullptr;
        std::unique_ptr<BinaryBHNode> right = nullptr;

        std::pair<Vec3D, Vec3D> getSubRegion(const Particle&) const;
        std::pair<Vec3D, Vec3D> getSiblingRegion(const Particle&) const;

        int particleID = -1;
        int depth;

    };

    std::unique_ptr<BinaryBHTree::BinaryBHNode> root = nullptr;
    std::map<int, Particle* const> pLookup;

    int maxDepth = 0;

    void insertParticle(BinaryBHTree::BinaryBHNode&, Particle&);
    void genPhysicalInfo(BinaryBHTree::BinaryBHNode&);

};