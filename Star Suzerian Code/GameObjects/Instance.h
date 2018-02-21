//
//  Instance.h
//
//  Created by Forrest Miller on 2/19/18.
//

#ifndef Instance_h
#define Instance_h

#include <stdio.h>
#include "GameParameters.h"
#include <assimp/Importer.hpp> //Gives access to the aiVector3D


enum InstanceType {PLAYERINSTANCE, WEAPONINSTANCE, ENEMYINSTANCE, SCENEINSTANCE, BASIC};

//Here is a typedef for an instance of what was generated
typedef struct Instance {
//public:
    int identifierNumber; //Unique number attached to each instance of any object
    //These are all basically just values to give the GPU for each instance
    aiVector3D position;
    float zoom;
    float thetaX, thetaY, thetaZ; //Euler Rotation Angles
    //collisionBox (eventually...)
    float timeAlive;
    InstanceType type;
    
    //Constructors
    Instance() {this->identifierNumber = -1;  this->type = BASIC; }//Construct an anonymous instance
    Instance(int id) {this->identifierNumber = id; this->type = BASIC;}
    
    //One function attached to getID()
    int getID() const {return this->identifierNumber;}
    
} Instance;
#endif /* Instance_h */
