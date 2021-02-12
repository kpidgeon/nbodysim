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

    struct BinaryBHNode : public BaseBinaryNode {

        using BaseBinaryNode::BaseBinaryNode;
        // BinaryBHNode();
        // BinaryBHNode(Vec3D& l, Vec3D& h) : BaseBinaryNode(l, h) { size = h.x - l.x; }

        std::unique_ptr<BinaryBHNode> left = nullptr;
        std::unique_ptr<BinaryBHNode> right = nullptr;

        std::pair<Vec3D, Vec3D> getSubRegion(const Particle&) const;
        std::pair<Vec3D, Vec3D> getSiblingRegion(const Particle&) const;

        int particleID = -1;
        int depth;

    };

    BinaryBHTree(BinaryBHNode* r) { root.reset(r); }

    std::unique_ptr<BinaryBHTree::BinaryBHNode> root = nullptr;
    std::map<int, Particle* const> pLookup;

    int maxDepth = 0;

    void insertParticle(BinaryBHTree::BinaryBHNode&, Particle&);
    void genPhysicalInfo(BinaryBHTree::BinaryBHNode&);

};