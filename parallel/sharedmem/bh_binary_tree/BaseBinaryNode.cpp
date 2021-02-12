#include "BaseBinaryNode.h"
#include <cmath>


/* Determine whether a cell (represented by a node) contains a given particle. */
bool BaseBinaryNode::contains(const Particle& p) const {

    return (p.pos.x >= lowBound.x && p.pos.x < highBound.x
            && p.pos.y >= lowBound.y && p.pos.y < highBound.y
            && p.pos.z >= lowBound.z && p.pos.z < highBound.z);

}


/* Find the minimum distance between two cellular volumes represented by nodes. */
float BaseBinaryNode::minDistToCell(const BaseBinaryNode& other) const {

    auto minX = std::min(lowBound.x, other.lowBound.x);
    auto minY = std::min(lowBound.y, other.lowBound.y);
    auto minZ = std::min(lowBound.z, other.lowBound.z);

    auto maxX = std::max(highBound.x, other.highBound.x);
    auto maxY = std::max(highBound.y, other.highBound.y);
    auto maxZ = std::max(highBound.z, other.highBound.z);

    auto dispX = std::max(.0f, (maxX - minX) - (highBound.x - lowBound.x) - (other.highBound.x - other.lowBound.x));
    auto dispY = std::max(.0f, (maxY - minY) - (highBound.y - lowBound.y) - (other.highBound.y - other.lowBound.y));
    auto dispZ = std::max(.0f, (maxZ - minZ) - (highBound.z - lowBound.z) - (other.highBound.z - other.lowBound.z));

    return std::sqrt( dispX*dispX + dispY*dispY + dispZ*dispZ );

}