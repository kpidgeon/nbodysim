#include "Gravity.h"



float Gravity::gAcceleration(const float m, const float r, const float G=1){
    return G*m / (r*r);
}



void Gravity::totalAcceleration(const Particle& p, const std::unique_ptr<BHNode>& node,
                            const float openingCriterion, float* const pf){

    auto xLength = node->getHighBound().x - node->getLowBound().x;
    auto dist = Vec3D::mag(node->getCentreOfMass() - p.pos);
    
    if (xLength / dist < openingCriterion){

        // Compute force on p due to mass at COM of node
        // (could add node to interaction list instead)
        *pf += gAcceleration(node->getTotalMass(), dist);

    }else{

        bool branch = false;
        auto obj = node->getObject();
        if (obj){
            // Don't want to consider the force
            // between a particle and itself
            if (obj != &p){
                *pf += gAcceleration(obj->getMass(), dist);
            }
        }
        else{
            branch = true;
        }

        // Open up node for finer granularity
        if (branch){
            for (auto &&s : node->octTrees)
            {
                if (s) { totalAcceleration(p, s, openingCriterion, pf); }
            }
        }

    }

}