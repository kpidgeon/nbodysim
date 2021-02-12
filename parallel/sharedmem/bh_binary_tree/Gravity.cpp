#include "Gravity.h"
#include <cmath>
#include <queue>


Vec3D Gravity::gAcceleration(const float m, const Vec3D r, const float G){
    auto r_mag = Vec3D::mag(r);
    return (G*m / (r_mag*r_mag*r_mag)) * r; // With r being the vector from a particle to a COM position
}

Vec3D Gravity::gAcceleration(const float m, const Vec3D r, const float G, const float epsilon){
    auto r_mag = Vec3D::mag(r);
    return (G*m / std::pow(std::sqrt(r_mag*r_mag + epsilon*epsilon), 3)) * r; // With r being the vector from a particle to a COM position
}


// void Gravity::totalAcceleration(const Particle& p, const BHNode& node,
//                             const float openingCriterion, Vec3D* const pv){

//     auto xLength = node.getHighBound().x - node.getLowBound().x;
//     auto delta = node.getCentreOfMass() - p.pos;
//     auto dist = Vec3D::mag(delta);
    
//     // std::cout << node.getDepth() << "\n";

//     if (xLength < openingCriterion * dist){

//         // Compute force on p due to mass at COM of node
//         // (could add node to interaction list instead)
//         *pv = *pv + gAcceleration(node.getTotalMass(), delta, 1);

//     }else{

//         bool branch = false;
//         if (node.octTrees.empty()){
//             if (node.particleID != p.ID)
//                 *pv = *pv + gAcceleration(node.getTotalMass(), delta, 1);
//         }
//         else{
//             branch = true;
//         }

//         // Open up node for finer granularity
//         if (branch){
//             for (auto &&s : node.octTrees)
//             {
//                 totalAcceleration(p, *s, openingCriterion, pv);
//             }
//         }

//     }

// }



void Gravity::totalAcceleration(const Particle& p, const BinaryBHTree::BinaryBHNode& node,
                            const float openingCriterion, Vec3D* const pv){

    auto xLength = node.highBound.x - node.lowBound.x;
    auto delta = node.com - p.pos;
    auto dist = Vec3D::mag(delta);
    
    if (xLength / dist < openingCriterion){

        // Compute force on p due to mass at COM of node
        // (could add node to interaction list instead)
        *pv = *pv + gAcceleration(node.mass, delta, 1);

    }
    else if (!node.left)
    {
        if (node.particleID != p.ID)
            *pv = *pv + gAcceleration(node.mass, delta, 1);
    }
    else if (node.left)
    {

        auto temp = node.left.get();
        while (temp)
        {
            
            totalAcceleration(p, *temp, openingCriterion, pv);
            temp = temp->right.get();

        }

    }

}