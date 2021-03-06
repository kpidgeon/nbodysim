#pragma once

#include <vector>
#include <memory>
#include "Particle.h"
#include "Vec3D.h"


// Main structure for Barnes-Hut tree-code
class BHNode
{
public:

    BHNode(Vec3D, Vec3D);

    std::vector<BHNode> octTrees;

    int particleID = -1;

    Vec3D getCentreOfMass() const;
    void setCentreOfMass(Vec3D);

    float getTotalMass() const;
    void setTotalMass(float m);

    Vec3D getLowBound() const;
    Vec3D getHighBound() const;

    bool contains(const Particle&);
    bool contains(const Vec3D&);

private:

    // Should replace these vertex bounds with a 'Cell'
    // abstraction
    Vec3D lowBound_;
    Vec3D highBound_;

    // For holding substructure information
    Vec3D centreOfMass_ = Vec3D(0,0,0);
    float mass_ = 0;

};