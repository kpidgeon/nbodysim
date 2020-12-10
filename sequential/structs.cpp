#include "structs.h"
#include <iostream>
#include <vector>

Point3D Point3D::operator*(const float& s){
    return Point3D({.x=s*x, .y=s*y, .z=s*z});
}

Point3D Point3D::operator+(const Point3D& p){
    return Point3D({.x=x+p.x, .y=y+p.y, .z=z+p.z});
}

std::ostream& operator<<(std::ostream& os, const Point3D& p){
    return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}



GravObj::GravObj(Point3D ini_pos, float ini_vx, float ini_vy, float ini_vz, float mass)
    : pos(ini_pos), vel({ini_vx, ini_vy, ini_vz}), _mass(mass)
{}

float GravObj::getMass(){
    return _mass;
}




BHNode::BHNode(Point3D lowBound, Point3D highBound)
{

    _lowBound = lowBound;
    _highBound = highBound;

    for (int i = 0; i < 8; i++)
    {
        octTrees.push_back(nullptr);
    }
    
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

    // TODO Change this so that a particle can't be put into more than one
    // cell i.e. change <= or >= to < or > ? or is this handled by BHTree::insertParticle?
    return (g.pos.x >= _lowBound.x && g.pos.y >= _lowBound.y && g.pos.z >= _lowBound.z
        && g.pos.x <= _highBound.x && g.pos.y <= _highBound.y && g.pos.z <= _highBound.z);

}

void BHNode::setTotalMass(float m){
    _mass = m;
}

float BHNode::getTotalMass(){
    return _mass;
}

void BHNode::setCentreOfMass(Point3D com){
    _centreOfMass = com;
}

Point3D BHNode::getCentreOfMass(){
    return _centreOfMass;
}




BHTree::BHTree(const std::vector<GravObj>& particles, Point3D& lowBound, Point3D& highBound)
{

    root = std::make_unique<BHNode>(BHNode(lowBound, highBound));

    for (auto &&p : particles)
    {
        insertParticle(root, p);
    }
    

}


void BHTree::insertParticle(std::unique_ptr<BHNode>& node, const GravObj& p){

    if (not node->getObject()){
        
        // Currently all child nodes are created together, so
        // if one is non-null, they all are
        bool noChildren = true;
        for (auto &&s : node->octTrees)
        {
            if (s){
                noChildren = false;
                if (s->contains(p)){
                    BHTree::insertParticle(s, p);
                    break; // Handles case where a particle is on a boundary
                            // and could be 'contained' within multiple cells
                }
            }
        }
        if (noChildren) node->setObject(p);
        
    }
    else{

        // There are two particles in the same cell,
        // so need to branch
        auto v1 = node->getLowBound();
        auto v2 = node->getHighBound();

        auto dX = v2.x - v1.x;
        auto dY = v2.y - v1.y;
        auto dZ = v2.z - v1.z;

        // TODO Clean this up - ideally only want to create BHNodes when absolutely necessary
        node->octTrees[0] = std::make_unique<BHNode>(BHNode(v1, Point3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2})));
        node->octTrees[1] = std::make_unique<BHNode>(BHNode(Point3D({v1.x, v1.y + dY/2, v1.z}), Point3D({v1.x + dX/2, v2.y, v1.z + dZ/2})));

        node->octTrees[2] = std::make_unique<BHNode>(BHNode(Point3D({v1.x, v1.y, v1.z + dZ/2}), Point3D({v1.x + dX/2, v1.y + dY/2, v2.z})));
        node->octTrees[3] = std::make_unique<BHNode>(BHNode(Point3D({v1.x, v1.y + dY/2, v1.z + dZ/2}), Point3D({v1.x + dX/2, v2.y, v2.z})));

        node->octTrees[4] = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y, v1.z}), Point3D({v2.x, v1.y + dY/2, v1.z + dZ/2})));
        node->octTrees[5] = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y + dY/2, v1.z}), Point3D({v2.x, v2.y, v1.z + dZ/2})));

        node->octTrees[6] = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y, v1.z + dZ/2}), Point3D({v2.x, v1.y + dY/2, v2.z})));
        node->octTrees[7] = std::make_unique<BHNode>(BHNode(Point3D({v1.x + dX/2, v1.y + dY/2, v1.z + dZ/2}), v2));

        // The particle p needs to be inserted
        // into the lower level...
        for (auto &&s : node->octTrees)
        {
            if (s->contains(p)){
                BHTree::insertParticle(s, p);
                break;
            }
        }
        
        // ...but the particle that was already in this
        // cell also needs to be propagated down to a
        // finer sub-volume...
        auto obj = node->getObject();
        node->clearObject();

        for (auto &&s : node->octTrees)
        {
            if (s->contains(*obj)){
                BHTree::insertParticle(s, *obj);
                break;
            }
        }
        
    }
}

// TODO Would make more sense inside of BHTree constructor
void BHTree::genPhysicalInfo(std::unique_ptr<BHNode>& node){

    // If the node has children - will need to change if child nodes
    // not all created at once as they currently are
    if (node->octTrees[0] != nullptr){

        for (auto &&s : node->octTrees)
        {
            genPhysicalInfo(s);
        }
        
        // Accumulate mass into node and determine COM
        float m = 0;
        Point3D com = Point3D({0,0,0});
        for (auto &&s : node->octTrees)
        {
            m += s->getTotalMass();
            com = com + s->getCentreOfMass()*s->getTotalMass();
        }
        
        node->setCentreOfMass(com*(1/m));
        node->setTotalMass(m);

    }
    else{

        if (node->getObject()){
            auto p = *node->getObject();
            node->setTotalMass(p.getMass());
            node->setCentreOfMass(p.pos);
        }
        else{
            // Temporary - ideally won't consider this case
            node->setTotalMass(0);
            node->setCentreOfMass(Point3D({0,0,0}));
        }

    }

}