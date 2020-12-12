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

    // All memory pertaining to sub-trees will be deleted
    // when a BHNode goes out of scope 
    std::vector<std::unique_ptr<BHNode>> octTrees;

    Vec3D getCentreOfMass();
    void setCentreOfMass(Vec3D);

    float getTotalMass();
    void setTotalMass(float m);

    Vec3D getLowBound();
    Vec3D getHighBound();

    const Particle* getObject();
    void setObject(const Particle&);
    void clearObject();
    
    bool contains(const Particle&);

private:

    const Particle *obj_;

    // should replace these vertex bounds with a 'Cell'
    // abstraction
    Vec3D lowBound_;
    Vec3D highBound_;

    // for holding substructure information
    Vec3D centreOfMass_;
    float mass_;

};