#include "Gravity.h"



Vec3D Gravity::gAcceleration(const float m, const Vec3D r, const float G=1){
    auto r_mag = Vec3D::mag(r);
    return (G*m / (r_mag*r_mag*r_mag)) * r;
}



void Gravity::totalAcceleration(const Particle& p, const std::unique_ptr<BHNode>& node,
                            const float openingCriterion, Vec3D* const pv){

    auto xLength = node->getHighBound().x - node->getLowBound().x;
    auto delta = node->getCentreOfMass() - p.pos;
    auto dist = Vec3D::mag(delta);
    
    if (xLength / dist < openingCriterion){

        // Compute force on p due to mass at COM of node
        // (could add node to interaction list instead)
        *pv = *pv + gAcceleration(node->getTotalMass(), delta);

    }else{

        bool branch = false;
        auto obj = node->getObject();
        if (obj){
            // Don't want to consider the force
            // between a particle and itself
            if (obj != &p){
                *pv = *pv + gAcceleration(obj->getMass(), delta);
            }
        }
        else{
            branch = true;
        }

        // Open up node for finer granularity
        if (branch){
            for (auto &&s : node->octTrees)
            {
                if (s) { totalAcceleration(p, s, openingCriterion, pv); }
            }
        }

    }

}