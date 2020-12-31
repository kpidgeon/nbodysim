#include "Vec3D.h"
#include <iostream>
#include <cmath>

Vec3D operator*(const Vec3D v, const float s){
    return Vec3D(s*v.x, s*v.y, s*v.z);
}

Vec3D operator*(const float s, const Vec3D v){
    return Vec3D(s*v.x, s*v.y, s*v.z);
}

Vec3D operator+(const Vec3D v1, const Vec3D v2){
    return Vec3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vec3D operator-(const Vec3D v1, const Vec3D v2){
    return Vec3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

bool operator!=(const Vec3D v1, const Vec3D v2){
    return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
}

std::ostream& operator<<(std::ostream& os, const Vec3D v){
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

float Vec3D::mag(const Vec3D& v){
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}