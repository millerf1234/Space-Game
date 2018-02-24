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
//typedef struct Instance {
class Instance{
public:
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
};
//} Instance;

//typedef struct PlayerInstance : public Instance {
class PlayerInstance : public Instance {
public:
    float health;
    float shields;
    float energy;
    float fuel;
    int playerNumber;
    int rocketCount;
    int maxRockets;
    
    //Color information
    float red;
    float green;
    float blue;
    
    //Movement information
    bool accelerate, decelerate, turnLeft, turnRight, rollLeft, rollRight, shoot;
    
    PlayerInstance(int playerNumber) {
        this->playerNumber = playerNumber;
        health = STARTING_PLAYER_HEALTH;
        shields = STARTING_PLAYER_SHIELDS;
        energy = STARTING_PLAYER_ENERGY;
        fuel = STARTING_PLAYER_FUEL;
        rocketCount = STARTING_PLAYER_ROCKETS;
        maxRockets = STARTING_PLAYER_ROCKET_COUNT_MAX;
        red = green = blue = 0.75f; //Setting all 3 equal will give a shade of gray
    }
};
//} PlayerInstance;

class WeaponInstance : public Instance {
public:
    
};


#endif /* Instance_h */
