////
////  Object.cpp
////
////  Created by Forrest Miller on 2/14/18.
//
////
//
//#include "Object.h"
//
////class Object {
////public:
////    //Fields
////    int objID; //Unique ObjectID for identification
////    float thetaX, thetaY, thetaZ; //Euler Rotation Angles
////    //collisionBox
////
////    virtual void draw() = 0;
////
////    //Constructor
////    Object() = delete; //Need an object ID to construct
////    Object(int objID);
////
////    //Destructor
////    virtual ~Object();
////
////    //Getters
////    aiVector3D getPosition() {return aiVector3D(x, y, z);}
////    aiVector3D getForward() {return this->forward;}
////    float getZoom() {return this->w;}
////    float getPositionx() {return this->x;}
////    float getPositiony() {return this->y;}
////    float getPositionz() {return this->z;}
////    bool isStationary() {return this->stationary;}
////
////    //Setters
////    virtual void changeThetax(float newThetaX);
////    virtual void changeThetay(float newThetaY);
////    virtual void changeThetaz(float newThetaZ);
////    virtual void changeZoom(float newZoom);
////    void makeStationary() {this->stationary = true;}
////    void undoMakeStationary(){this->stationary = false;}
////    void toggleStationary() {this->stationary = !(this->stationary);}
////
////    //Translation
////    virtual void translate(aiVector3D); //Translates model by vector
////    aiVector3D translateForward(float distance); //Translates model in its forward direction (might want to scale distance with value of zoom, or maybe not)
////
////    //doUpkeey is called for every object, most probably won't do anything here besides call updateTimeAlive
////    virtual void doUpkeep() {updateTimeAlive();}
////
////    virtual void recalculateForward() = 0;
////    virtual void rotate() = 0;
////
////private:
////    ObjectGenerator * generatorPrototype;
////    float x,y,z,w;
////    aiVector3D forward; //points in the forward direction of the model
////    bool stationary;
////    float timeAlive;
////
////
////
////    void updateTimeAlive();
////    void updateTimeAlive(float timeElapsed); //This should be called each loop iteration
////
////};
//
//Object::Object(int objID) {
//    this->stationary = false;
//    this->forward = aiVector3D(0.0f, 0.0f, 0.0f); //Initialize to the 0 vector
//    this->timeAlive = 0.0f;
//    this->objID = objID;
//    this->x = this->y = this->z = this->w = 0.0f;
//    this->thetaX = this->thetaY = this->thetaZ = 0.0f;
//    this->objData = nullptr;
//}
//
//Object::~Object() {
//    
//}
//
//void Object::translate(const aiVector3D& transl) {
//    if (this->stationary) {return;} //If Object is stationary, return without doing anything
//    this->x += transl.x;
//    this->y += transl.y;
//    this->z += transl.z;
//}
//
//
//void Object::updateTimeAlive() {
//    this->timeAlive += TIME_TICK_RATE;
//}
//
//void Object::updateTimeAlive(float timeElapsed) {
//    this->timeAlive += timeElapsed; //Move object's existance time forward
//}

