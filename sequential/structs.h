#pragma once

#include <vector>
#include <memory>

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

    // All memory pertaining to sub-trees will be deleted
    // when a BHNode goes out of scope 
    std::unique_ptr<BHNode> oct1;
    std::unique_ptr<BHNode> oct2;
    std::unique_ptr<BHNode> oct3;
    std::unique_ptr<BHNode> oct4;
    std::unique_ptr<BHNode> oct5;
    std::unique_ptr<BHNode> oct6;
    std::unique_ptr<BHNode> oct7;
    std::unique_ptr<BHNode> oct8;

    Point3D getCentreOfMass();
    float getTotalMass();

    Point3D getLowBound();
    Point3D getHighBound();

    const GravObj* getObject();
    void setObject(const GravObj&);
    void clearObject();
    
    bool contains(const GravObj&);

private:

    const GravObj *_obj;

    // should replace these vertex bounds with a 'Cell'
    // abstraction
    Point3D _lowBound;
    Point3D _highBound;

    // for holding substructure information
    Point3D _centreOfMass;
    float _mass;

};



// Wrapper for the Barnes-Hut tree
class BHTree
{

    public:

        BHTree(const std::vector<GravObj>&);

        // When a BHTree instance goes out of scope
        // the memory deallocation will propagate down the
        // tree
        std::unique_ptr<BHNode> root;

        void insertParticle(std::unique_ptr<BHNode>&, const GravObj& p);

};