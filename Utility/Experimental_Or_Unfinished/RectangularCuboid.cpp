//
//  RectangularCuboid.cpp
//
//  Created by Forrest Miller on 2/27/18.
//

#include "RectangularCuboid.h"

static constexpr bool USE_DEBUG_MESSAGES = true;


//Public RectangularCuboid Functionality Implementation:

//Constructors
RectangularCuboid::RectangularCuboid() {
    this->length = 0.0f;
    this->width = 0.0f;
    this->height = 0.0f;
    
    this->earlyZRot = new Quaternion(0.0f, 0.0f, 1.0f);
    this->xRot = new Quaternion(1.0f, 0.0f, 0.0f);
    this->yRot = new Quaternion(0.0f, 1.0f, 0.0f);
    this->zRot = new Quaternion(0.0f, 0.0f, 1.0f);
    this->cPlaneRotation = 0.0f;
    this->midpoint = aiVector3D(0.0f, 0.0f, 0.0f);
    
    cPlane.vec1 = aiVector3D(1.0f, 0.0f, 0.0f);
    cPlane.vec2 = aiVector3D(0.0f, 1.0f, 0.0f);
    
    for (int i = 0; i < NUM_CUBE_VERTICES; ++i) {
        vertices[i] = 0.0f;
        originalVerticies[i] = 0.0f;
    }
}



//Destructors



//Getters
float RectangularCuboid::getEarlyZRotAngle() const {
    if (earlyZRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getEarlyZRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return earlyZRot->getTheta();
}

float RectangularCuboid::getXRotAngle() const {
    if (xRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getXRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return xRot->getTheta();
}

float RectangularCuboid::getYRotAngle() const {
    if (yRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getYRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return yRot->getTheta();
}

float RectangularCuboid::getZRotAngle() const {
    if (zRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getzRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return zRot->getTheta();
}



/*
 static constexpr int NUM_CUBE_VERTICES = 8;
 
 class RectangularCuboid {
 private:
 float length, width, height; //Convention: Length follows x-Axis, Height follows y-Axis, Width follows z-Axis
 
 aiVector3D midpoint;
 
 
 float vertices[NUM_CUBE_VERTICES];
 float originalVerticies[NUM_CUBE_VERTICES];
 
 void rotate();
 
 Quaternion * earlyZRot, * xRot, * yRot, * zRot;
 
 public:
 //Constructors
 RectangularCuboid();
 RectangularCuboid(float * objData, int size); //Construct a cube from model data, model will be contained in cube.
 RectangularCuboid(const aiVector3D &);
 //Destructors
 ~RectangularCuboid();
 
 //Public functions
 bool isInside(float x, float y); //Checks to see if the given x,y coordinates are inside the cube
 bool isInside(const RectangularCuboid &); //Checks to see if two rectangular cuboid's are overlapping
 RectangularCuboid(float * objData, int posIndexOffset, int vertComponents, int numDatapoints);
 
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
 */
 

