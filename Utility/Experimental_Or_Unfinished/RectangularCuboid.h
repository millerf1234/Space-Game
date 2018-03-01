//UNFINISHED!!!!

//  Class that represents a rectuangular cube. I am intending this to be used to
//  do collisions of 3D models restricted to movement in a 2D plane (but the 3D
//  objects can rotate in 3D).
//
//  I'm not sure yet if I am going to treat this as a box that is constructed once
//  and then rotated with the models, or if I am going to reconstruct the box for each
//  model on each frame based off model orientation. It will probably come down to
//  how many vertices are in the model.
//
//  RectangularCuboid.h
//
//  Created by Forrest Miller on 2/27/18.
//

#ifndef RectangularCuboid_h
#define RectangularCuboid_h

#include <iostream>
#include <assimp/Importer.hpp> //Gives access to the aiVector3D

#include "Quaternion.h"

static constexpr int NUM_CUBE_VERTICES = 8;

typedef struct collisionPlane {
    aiVector3D vec1;
    aiVector3D vec2;
} CollisionPlane;



class RectangularCuboid {
private:
    float length, width, height; //Convention: Length follows x-Axis, Height follows y-Axis, Width follows z-Axis
    
    aiVector3D midpoint;
    CollisionPlane cPlane; 
    
    float cPlaneRotation;
    
    //float thetaX, thetaY, thetaZ;
    
    float vertices[NUM_CUBE_VERTICES];
    float originalVerticies[NUM_CUBE_VERTICES];
    
    void rotate();
    
    Quaternion * earlyZRot, * xRot, * yRot, * zRot;
    
    void computeSelfFromModel(float * modelVerts, int numDatapoints);
    
    
    
    
public:
    //Constructors
    RectangularCuboid();
    RectangularCuboid(float * objData, int numDatapoints); //Construct a cube from model data, model will be contained in cube.
    RectangularCuboid(float * objData, int posIndexOffset, int vertComponents, int numDatapoints);
    RectangularCuboid(const aiVector3D &);
    //Destructors
    ~RectangularCuboid();
    
    //Public functions
    bool isInside(float x, float y); //Checks to see if the given x,y coordinates are inside the cube
    bool isInside(const RectangularCuboid &); //Checks to see if two rectangular cuboid's are overlapping
    
    float getCenterDistances(const RectangularCuboid &); //Returns the distance between the midpoints of 2 cuboids
    float getCenterDistanceTo(const aiVector2D &); //Assumes that aiVector2D lies in the same depth plane
    float getCenterDistanceTo(const aiVector3D &);
    
    void updateRotationAngles(float earlyZ, float x, float y, float z);
    void updateRotationAngles(float earlyZ, const aiVector3D &);
    void updateRotationAngleX(float x);
    void updateRotationAngleY(float y);
    void updateRotationAngleZ(float z);
    void updateRotationAngleEarlyZ(float ez);
    
    void initFromModel(float *, int);
    
    float getEarlyZRotAngle() const;
    float getXRotAngle() const;
    float getYRotAngle() const;
    float getZRotAngle() const;
    
};

#endif /* RectangularCuboid_h */
