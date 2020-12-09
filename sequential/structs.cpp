#include "structs.h"
#include <iostream>
#include <vector>


GravObj::GravObj(Point3D ini_pos, float ini_vx, float ini_vy, float ini_vz, float mass)
    : pos(ini_pos), vel({ini_vx, ini_vy, ini_vz}), _mass(mass)
{}




BHNode::BHNode(Point3D lowBound, Point3D highBound)
{

    _lowBound = lowBound;
    _highBound = highBound;

    oct1 = nullptr;
    oct2 = nullptr;
    oct3 = nullptr;
    oct4 = nullptr;
    oct5 = nullptr;
    oct6 = nullptr;
    oct7 = nullptr;
    oct8 = nullptr;

    _obj = nullptr;
}

Point3D BHNode::getLowBound(){
    return _lowBound;
}

Point3D BHNode::getHighBound(){
    return _highBound;
}

const GravObj* BHNode::getObject(){
    return _obj;
}

void BHNode::setObject(const GravObj &p){
    _obj = &p;
}

void BHNode::clearObject(){
    _obj = nullptr;
}

bool BHNode::contains(const GravObj& g){

    bool contains = false;
    if (g.pos.x >= _lowBound.x && g.pos.y >= _lowBound.y && g.pos.z >= _lowBound.z
        && g.pos.x <= _highBound.x && g.pos.y <= _highBound.y && g.pos.z <= _highBound.z){
        
        contains = true;

    }

    return contains;

}





BHTree::BHTree(const std::vector<GravObj>& particles)
{

    root = std::make_unique<BHNode>(BHNode(Point3D({0,0,0}), Point3D({10,10,10})));

    for (auto &&p : particles)
    {
        insertParticle(root, p);
    }
    

}


void BHTree::insertParticle(std::unique_ptr<BHNode>& node, const GravObj& p){

    // Could conditionally call this function
    // instead of checking this here
    if (node->contains(p)){

        if (not node->getObject()){
            
            // Currently all child nodes are created together, so
            // if one is non-null, they all are
            if (node->oct1 != nullptr){
                BHTree::insertParticle(node->oct1, p);
                BHTree::insertParticle(node->oct2, p);
                BHTree::insertParticle(node->oct3, p);
                BHTree::insertParticle(node->oct4, p);
                BHTree::insertParticle(node->oct5, p);
                BHTree::insertParticle(node->oct6, p);
                BHTree::insertParticle(node->oct7, p);
                BHTree::insertParticle(node->oct8, p);
            }
            else{
                // If there is no lower level for the
                // particle to be inserted into then insert here
                node->setObject(p);
            }
        }
        else{

            // There are two particles in the same cell,
            // so need to branch
            auto v1 = node->getLowBound();
            auto v2 = node->getHighBound();

            auto dX = v2.x - v1.x;
            auto dY = v2.y - v1.y;
            auto dZ = v2.z - v1.z;

            // TODO Clean this up
            node->oct1 = std::make_unique<BHNode>(BHNode(v1, Point3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2})));
            node->oct2 = std::make_unique<BHNode>(BHNode(Point3D({v1.x, v1.y + dY/2, v1.z}), Point3D({v1.x + dX/2, v2.y, v1.z + dZ/2})));

            node->oct3 = std::make_unique<BHNode>(BHNode(Point3D({v1.x, v1.y, v1.z + dZ/2}), Point3D({v1.x + dX/2, v1.y + dY/2, v2.z})));
            node->oct4 = std::make_unique<BHNode>(BHNode(Point3D({v1.x, v1.y + dY/2, v1.z + dZ/2}), Point3D({v1.x + dX/2, v2.y, v2.z})));

            node->oct5 = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y, v1.z}), Point3D({v2.x, v1.y + dY/2, v1.z + dZ/2})));
            node->oct6 = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y + dY/2, v1.z}), Point3D({v2.x, v2.y, v1.z + dZ/2})));

            node->oct7 = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y, v1.z + dZ/2}), Point3D({v2.x, v1.y + dY/2, v2.z})));
            node->oct8 = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2}), v2));

            // The particle p needs to be inserted
            // into the lower level...
            BHTree::insertParticle(node->oct1, p);
            BHTree::insertParticle(node->oct2, p);
            BHTree::insertParticle(node->oct3, p);
            BHTree::insertParticle(node->oct4, p);
            BHTree::insertParticle(node->oct5, p);
            BHTree::insertParticle(node->oct6, p);
            BHTree::insertParticle(node->oct7, p);
            BHTree::insertParticle(node->oct8, p);

            // ...but the particle that was already in this
            // cell also needs to be propagated down to a
            // finer sub-volume...
            auto obj = node->getObject();
            node->clearObject();

            BHTree::insertParticle(node->oct1, *obj);
            BHTree::insertParticle(node->oct2, *obj);
            BHTree::insertParticle(node->oct3, *obj);
            BHTree::insertParticle(node->oct4, *obj);
            BHTree::insertParticle(node->oct5, *obj);
            BHTree::insertParticle(node->oct6, *obj);
            BHTree::insertParticle(node->oct7, *obj);
            BHTree::insertParticle(node->oct8, *obj);
        }
    }
}
