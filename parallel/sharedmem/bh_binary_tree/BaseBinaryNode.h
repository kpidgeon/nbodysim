#pragma once

#include "Vec3D.h"
#include <vector>
#include <memory>
#include "Particle.h"

struct BaseBinaryNode
{

    BaseBinaryNode(Vec3D& l, Vec3D& h) : lowBound(l), highBound(h) { size = h.x - l.x; }
    BaseBinaryNode() {};

    std::unique_ptr<BaseBinaryNode> left = nullptr;
    std::unique_ptr<BaseBinaryNode> right = nullptr;

    Vec3D lowBound;
    Vec3D highBound;
    float size;

    Vec3D com;
    float mass;

    bool contains(const Particle&) const;
    float minDistToCell(const BaseBinaryNode&) const;


};