//  This is a class to represent a 2-dimensional dynamically sized circle that
//  can be used to encapsulate 2- and 3-dimensional models. With 3D models,
//  rotating about an axis which isn't orthogonal (normal) to the plane will
//  cause the sphere to be dynamically resized.
//
//
//
//  Right now this class just piggybacks off of CollisionBox to figure out
//  how big to make the circle radius
//
//  CollisionCircle.h
//
//  Created by Forrest Miller on 3/29/18.
//

#ifndef CollisionCircle_h
#define CollisionCircle_h

#include <iostream>

#include "NGon.h"
#include "CollisionBox.h"

//static constexpr float PI = 3.1415927f; //If PI is not defined externally, uncomment this line

class CollisionCircle {
private:
    CollisionBox * underlyingCollisionBox; //I realize doing this is creating an unnecessary dependency that I will later regret down the road
    float radius;
    aiVector2D centerPoint;
    
    bool hasBeenSet;
    bool modelIs3D;
    
    void recalculateRadius();
    
    void initialize(); //This function to be called by constructors
    
public:
    //Deleted functionality (These functions can not be called)
    CollisionCircle() = delete; //Default constructor is deleted
    CollisionCircle(const CollisionCircle &) = delete; //Copy Constructor is deleted
    CollisionCircle& operator= (const CollisionCircle &) = delete; //Copy assignment operator is deleted
    
    //Constructors
    CollisionCircle(float * modelData, int dataPoints, bool is3D);
    CollisionCircle(aiVector2D * modelData, int arraySize);
    CollisionCircle(aiVector3D * modelDara, int arraySize);
    CollisionCircle(const CollisionBox &); //Can construct a circle from a collisionBox
    
    //Destructor
    ~CollisionCircle();
    
    
    //Getters/Setters
    float getRadius() const {return this->radius;}
    aiVector2D getCenterPoint() const {return this->centerPoint;}
    bool isSet() const {return this->hasBeenSet;}
    bool getModelIs3D() const {return this->modelIs3D;}
    
    void setMidpoint(aiVector2D midpoint) {this->centerPoint = midpoint;}
    void setMidpoint(float x, float y) {this->centerPoint = aiVector2D(x,y);}
    void translate(aiVector2D translation) {this->centerPoint += translation;}
    void translate(float x, float y) {this->centerPoint.x += x; this->centerPoint.y += y;}
    
    
    //Get the vertices of the circle using the following functions
    //IMPORTANT: see this:
    //It says:
    //If you have a std::vector<T> v, you may obtain a T* pointing to the start of the contiguous data (which is what OpenGL is after) with the expression &v[0]. Thus OpenGL can buffer directly from a vector of GLfloat
    std::unique_ptr<std::vector<GLfloat>> getVerticesTriStrip() const;
    std::unique_ptr<std::vector<GLfloat>> getVerticesTriangles() const;
    
    
    //Collision Detection Functions
    bool isInside(const aiVector2D &) const;
    bool isInside(float x, float y) const;
    bool isInsideOrOnEdge(float x, float y) const;
    
    bool checkIfLineIntersectsCircle(aiVector2D pointA, aiVector2D pointB) const;
    bool checkIfLineIntersectsCircle(float x0, float y0, float x1, float y1) const;
    
    bool isOverlapping(const CollisionCircle &) const;
    bool isOverlapping(const CollisionBox &) const;
    
    void moveApartAlongLineBetweenMidpoints(CollisionCircle &);
    void moveApartAlongLineBetweenMidpoints(CollisionBox &);
    void moveApartAdvanced(CollisionBox &); //Does a more in-depth calculation when moving apart
    
};




#endif /* CollisionCircle_h */
