#pragma once

#include <vector>

struct Point3D
{
    float x, y, z;
};


class GravObj
{
public:

    GravObj(Point3D, float vx, float vy, float vz, float mass);
    GravObj(float x, float y, float z,
            float vx, float vy, float vz,
            float mass);

    Point3D pos;

    // For clarity
    struct {
        float v_x, v_y, v_z;
    } vel;

    float getMass();

private:

    float _mass;

};


// Main structure for Barnes-Hut tree-code
class BHNode
{
public:

    BHNode(Point3D, Point3D);

	BHNode* oct1;
    BHNode* oct2;
    BHNode* oct3;
    BHNode* oct4;
    BHNode* oct5;
    BHNode* oct6;
    BHNode* oct7;
    BHNode* oct8;

    Point3D getCentreOfMass();
    float getTotalMass();

    Point3D getLowBound();
    Point3D getHighBound();

    GravObj getObject();
    void setObject(GravObj&);
    
    bool contains(GravObj);

private:

    GravObj *_obj;

    // should replace these vertex bounds with a 'Cell'
    // abstraction
    Point3D _lowBound;
    Point3D _highBound;
    // Cell cell;
    Point3D _centreOfMass;
    float _mass;

};


class BHTree
{

    public:

        BHTree(std::vector<GravObj>);

        BHNode root;

        ~BHTree();


};