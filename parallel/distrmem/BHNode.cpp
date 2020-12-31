#include "BHNode.h"

int d = 0;

BHNode::BHNode(Vec3D lowBound, Vec3D highBound)
{


    lowBound_ = lowBound;
    highBound_ = highBound;

}

BHNode::BHNode(Vec3D lowBound, Vec3D highBound, int depth)
{

    depth_ = depth;

    lowBound_ = lowBound;
    highBound_ = highBound;

}

Vec3D BHNode::getLowBound() const{
    return lowBound_;
}

Vec3D BHNode::getHighBound() const{
    return highBound_;
}

int BHNode::getDepth() const {
    return depth_;
}

bool BHNode::contains(const Particle& g){

    return (g.pos.x >= lowBound_.x && g.pos.y >= lowBound_.y && g.pos.z >= lowBound_.z
        && g.pos.x < highBound_.x && g.pos.y < highBound_.y && g.pos.z < highBound_.z);

}

bool BHNode::contains(const Vec3D& v){
    return (v.x >= lowBound_.x && v.y >= lowBound_.y && v.z >= lowBound_.z
        && v.x < highBound_.x && v.y < highBound_.y && v.z < highBound_.z);
}

void BHNode::setTotalMass(float m){
    mass_ = m;
}

float BHNode::getTotalMass() const{
    return mass_;
}

void BHNode::setCentreOfMass(Vec3D com){
    centreOfMass_ = com;
}

Vec3D BHNode::getCentreOfMass() const{
    return centreOfMass_;
}
