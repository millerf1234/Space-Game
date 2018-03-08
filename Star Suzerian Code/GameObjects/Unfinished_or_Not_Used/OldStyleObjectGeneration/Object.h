////  Abstract base class that any object in my game will need to inherit from
////  Object.h
////
////  Created by Forrest Miller on 2/14/18.
////
////  2/16/18  version 0.051 Rewrote entire class to match with ObjectGenerator
////  2/15/18  version 0.05 Added WAY more stuff to class header. Need to implement still
////  2/14/18  version 0.01 Started class
//
//#ifndef Object_h
//#define Object_h
//
//#include <iostream>
//#include "glad.h"
//#include "SimpleObjLoader.h"
//#include "ShaderWrapper.h"
//#include "Quaternion.h"
//#include "ObjectGenerator.h"
//
//class ObjectGenerator; //I need to do a circular declaration here. See: https://stackoverflow.com/questions/994253/two-classes-that-refer-to-each-other
//
//
//class Object {
//public:
//    //Fields
//    int objID; //Unique ObjectID for identification
//    float thetaX, thetaY, thetaZ; //Euler Rotation Angles
//    //collisionBox
//    
//    virtual void draw() = 0;
//    
//    //Constructor
//    Object() = delete; //Need an object ID to construct
//    Object(int objID);
//    
//    //Destructor
//    virtual ~Object();
//    
//    //Getters
//    aiVector3D getPosition() {return aiVector3D(x, y, z);}
//    aiVector3D getForward() {return this->forward;}
//    float getZoom() {return this->w;}
//    float getPositionx() {return this->x;}
//    float getPositiony() {return this->y;}
//    float getPositionz() {return this->z;}
//    bool isStationary() {return this->stationary;}
//    
//    //Setters
//    void changeThetax(float newThetaX);
//    void changeThetay(float newThetaY);
//    void changeThetaz(float newThetaZ);
//    void changeZoom(float newZoom); //Change W
//    void makeStationary() {this->stationary = true;}
//    void unMakeStationary(){this->stationary = false;}
//    void toggleStationary() {this->stationary = !(this->stationary);}
//    
//    //Translation
//    virtual void translate(const aiVector3D &); //Translates model by vector
//    //virtual void translateForward(float distance); //Translates model in its forward direction (might want to scale distance with value of zoom, or maybe not)
//    
//    //doUpkeey is called for every object, most probably won't do anything here besides call updateTimeAlive
//    virtual void doUpkeep() {updateTimeAlive();}
//    
//    virtual void recalculateForward() = 0;
//    virtual void rotateXYZ() = 0;
//    
//protected:
//    ObjectGenerator * objData;
//    float x,y,z,w; //Position
//    aiVector3D forward; //Points in the forward direction of the model
//    bool stationary;
//    float timeAlive;
//    
//    void updateTimeAlive();
//    void updateTimeAlive(float timeElapsed); //This should be called each loop iteration
//    
//};
//    
//    
////private:
////    float x,y,z,w; //Coordinates of centerpoint
////    //Collision box * collisionBox //Add this in later
////
////    GLfloat * vbo, * vertices;
////    GLuint * ebo, * elements;
////    SimpleObjLoader * objFileData;
////
////    ShaderWrapper * shaders;
////
////    Quaternion *xRot, *yRot, *zRot; //rotation quaternions
////
////    bool hasModelData, modelValid;
////    bool hasShaderWrapper, shaderValid;
////    bool stationary; //Says whether this object should move or not
////    int shaderCount;
////
////
////    //collisionBox * collisionBox
////
////public:
////    Object();
////    Object(const SimpleObjLoader &);
////    Object(const ShaderWrapper &);
////    Object(const SimpleObjLoader &, const ShaderWrapper &);
////    Object(const char * objFile);
////    Object(const char * objFile, const ShaderWrapper &);
////
////
////    //Functions for building model
////    virtual void loadObjFile(const char * file);
////    virtual void attachObjFile();
////    virtual void attachShaderWrapper();
////
////    //Getters
////    bool hasModel();
////    bool CheckModelValid();
////    bool hasShader();
////    bool checkShaderValid();
////    bool isStationary(); //Returns true if an object is not supposed to move
////    //getCollisionBox
////
////    //Model functions
////    virtual void translate(aiVector2D) = 0;
////    virtual void translate(aiVector3D) = 0;
////
////    virtual void drawSelf() = 0;
////    virtual void wasDestroyed() = 0;
////
//
////
//#endif /* Object_h */
////
////
//////Just in case I forgot what an abstract base class looks like:
//////
//////  Shape.hpp
//////  Tetris_Using_NCurses
//////
//////  This is the abstract base class Shape from which all other
//////  Shape objects inherit from.
//////
//////  Created by Forrest Miller on 12/12/17.
//////  Copyright © 2017 Forrest Miller. All rights reserved.
//////
////
//////#define POS0 0 //base
//////#define POS1 1 //Rotate right 1 or left 3
//////#define POS2 2 //Rotate right 2 or left 2
//////#define POS3 3 //Rotate right 3 or left 1
//////
//////class Shape {
//////private:
//////    int colorPair;
//////protected:
//////    int size;
//////    int xOrgin;
//////    int yOrgin;
//////    int orientation;
//////    int previousOrientation;
//////    //Rotation helper functions:
//////    virtual void translateOrgin() = 0; //To make sure the shape stays in
//////    //the same general area when rotating, make sure cycling through all 4 positions in order returns the orgin to it's starting position (i.e. the sum of all the translations results in 0) and also make sure each translation has it's counterpart as an inverse (i.e make sure that translation from 1->2 and 2->1 are inverse)
//////    virtual void computeNewOffsets() = 0;
//////public:
//////    //Fields
//////    int * xCoorOffsets;
//////    int * yCoorOffsets;
//////    //Constructors
//////    Shape(int colPair);
//////    Shape(int colPair, int orginY, int orginX);
//////    //Destructor
//////    //virtual ~Shape() = 0; //Not sure why this doesnt work: http://www.geeksforgeeks.org/pure-virtual-destructor-c/
//////    virtual ~Shape() {;} //This works though
//////
//////    //Member Functions
//////    void setOrgin(int y, int x);
//////    int getOrginX() {return this->xOrgin;}
//////    int getOrginY() {return this->yOrgin;}
//////    void moveFromGravity(int amount); //Moves in the y direction 'amount' amount
//////    void moveShapeUpOne();
//////
//////    void moveLeft(int amount) {this->xOrgin -= amount;}
//////    void moveRight(int amount) {this->xOrgin += amount;}
//////
//////    virtual void rotateLeft() = 0;
//////    virtual void rotateRight() = 0;
//////    int getOrientation() {return this->orientation;}
//////
//////    int getColorPair() {return this->colorPair;}
//////    int getSize() {return this->size;}
//////    virtual int getHeight() = 0;
//////    virtual int getWidth() = 0;
//////
//////};
////

