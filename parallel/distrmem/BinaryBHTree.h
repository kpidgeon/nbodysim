#pragma once

#include <memory>
#include "Vec3D.h"
#include <vector>
#include <map>
#include "Particle.h"

class BinaryBHTree
{

public:

    BinaryBHTree(std::vector<Particle>&);

    struct BinaryBHNode{

        BinaryBHNode(const Vec3D& l, const Vec3D& h) : lowBound(l), highBound(h) {}

        std::unique_ptr<BinaryBHNode> left = nullptr;
        std::unique_ptr<BinaryBHNode> right = nullptr;

        bool contains(const Particle&) const;
        std::pair<Vec3D, Vec3D> getSubRegion(const Particle&) const;
        std::pair<Vec3D, Vec3D> getSiblingRegion(const Particle&) const;

        Vec3D lowBound;
        Vec3D highBound;
        int particleID = -1;

        Vec3D com = Vec3D(0,0,0);
        float mass = 0;
        int depth;

    };

    std::unique_ptr<BinaryBHTree::BinaryBHNode> root = nullptr;
    std::map<int, Particle* const> pLookup;

    void insertParticle(BinaryBHTree::BinaryBHNode&, Particle&);
    void genPhysicalInfo(BinaryBHTree::BinaryBHNode&);

};