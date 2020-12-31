#pragma once

#include <vector>
#include <memory>
#include "Particle.h"
#include "Vec3D.h"


// Main structure for Barnes-Hut tree-code
class BHNode
{
public:

    BHNode(){};
    BHNode(Vec3D, Vec3D);
    BHNode(Vec3D, Vec3D, int);

    std::vector<BHNode> octTrees;

    int particleID = -1;
    
    static int d;

    Vec3D getCentreOfMass() const;
    void setCentreOfMass(Vec3D);

    float getTotalMass() const;
    void setTotalMass(float m);

    Vec3D getLowBound() const;
    Vec3D getHighBound() const;

    bool contains(const Particle&);
    bool contains(const Vec3D&);

    int getDepth() const;

private:

    // Should replace these vertex bounds with a 'Cell'
    // abstraction
    Vec3D lowBound_;
    Vec3D highBound_;

    // For holding substructure information
    Vec3D centreOfMass_ = Vec3D(0,0,0);
    float mass_ = 0;

    int depth_ = 0;

};