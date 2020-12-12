#pragma once

#include <iostream>

struct Vec3D {

    Vec3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vec3D(){};

    float x, y, z;

    friend Vec3D operator*(const Vec3D, const float);
    friend Vec3D operator*(const float, const Vec3D);
    friend Vec3D operator+(const Vec3D, const Vec3D);
    friend std::ostream& operator<<(std::ostream&, const Vec3D);

};