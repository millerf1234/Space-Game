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
#include "Quaternion.h"
#include "CollisionRectangle.h"


enum InstanceType {PLAYERINSTANCE, WEAPONINSTANCE, ENEMYINSTANCE, SCENEINSTANCE, BASIC};
enum WeaponType {HEXAGON_BOMB, LAZER, ROCKET, HOMINGROCKET, UNINITIALIZED};
//Here is a typedef for an instance of what was generated

class Instance{
public:
    int identifierNumber; //Unique number attached to each instance of any object
    //These are all basically just values to give the GPU for each instance
    aiVector3D position;
    aiVector2D velocity; //Velocity is applied to position in each generator's upkeep function
    float zoom;
    float thetaX, thetaY, thetaZ; //Euler Rotation Angles
    //collisionBox (eventually...)
    float timeAlive;
    InstanceType type;
    //bool hasCollision; //This is set within EntityManager instead of being attached repeatedly to each instance
    CollisionRectangle * colBox;
    
    //Constructors
    Instance() {//Construct an anonymous instance
        this->identifierNumber = -1;
        this->type = BASIC;
        this->velocity.x = this->velocity.y = 0.0f;
        this->colBox = nullptr;
    }
    Instance(int id) { //Construct an instance that has an ID number
        this->identifierNumber = id;
        this->type = BASIC;
        velocity.x = velocity.y = 0.0f;
        this->colBox = nullptr;
    }
    
    ~Instance() {
        if (this->colBox != nullptr) {delete this->colBox; this->colBox = nullptr;}
    }
    
    //One function attached to getID()
    int getID() const {return this->identifierNumber;}
};


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
    
    float rollAmount;
    aiVector3D * forward;
    
    int currentAnimationFrame;
    int maxAnimationFrame;
    
    //Movement information
    bool accelerate, decelerate, turnLeft, turnRight, rollLeft, rollRight, shoot;
    
    aiVector3D * rear;
    aiVector3D ** translationHistory;
    Quaternion * xRot, * yRot, * zRot;
    
    PlayerInstance() = delete;
//    PlayerInstance() {
//        if (PRINT_DEBUG_MESSAGES) {
//            std::cout << "DEBUG::OOPS! IT looks like the wrong PlayerInstance constructor was called";
//        }
//            rear = nullptr;
//            xRot = yRot = zRot = nullptr;
//
//    }
    
    PlayerInstance(int playerNumber) {
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "DEBUG::PlayerInstance constructor was called!" << std::endl;
        }
        this->playerNumber = playerNumber;
        health = STARTING_PLAYER_HEALTH;
        shields = STARTING_PLAYER_SHIELDS;
        energy = STARTING_PLAYER_ENERGY;
        fuel = STARTING_PLAYER_FUEL;
        rocketCount = STARTING_PLAYER_ROCKETS;
        maxRockets = STARTING_PLAYER_ROCKET_COUNT_MAX;
        red = green = blue = 0.75f; //Setting all 3 equal will give a shade of gray (A good default color)
        rollAmount = 0.0f;
        currentAnimationFrame = maxAnimationFrame = 0;
        rear = new aiVector3D(0.0f, 0.0f, PLAYER_ENGINE_FLAME_REAR_POSITION);
        forward = new aiVector3D(0.0f, 0.0f, 1.0f);
        xRot = new Quaternion(1.0f, 0.0f, 0.0f);
        yRot = new Quaternion(0.0f, 1.0f, 0.0f);
        zRot = new Quaternion(0.0f, 0.0f, 1.0f);
        translationHistory = new aiVector3D* [PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES];
        //the values in translation history will be set in PlayerManager inside initializeFromTemplate() [unless I move it]
        for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES; ++i) {
            translationHistory[i] = nullptr;
        }
    }
    
    ~PlayerInstance() {
        if (this->rear != nullptr) {
            delete this->rear;
            this->rear = nullptr;
        }
        if (this->xRot != nullptr) {
            delete this->xRot;
            xRot = nullptr;
        }
        if (this->yRot != nullptr) {
            delete this->yRot;
            yRot = nullptr;
        }
        if (this->zRot != nullptr) {
            delete this->zRot;
            this->zRot = nullptr;
        }
        if (this->forward != nullptr) {
            delete this->forward;
            this->forward = nullptr;
        }
        //delete the translation history vector
        if (translationHistory != nullptr) {
            for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES; ++i) {
                delete translationHistory[i];
            }
            delete [] translationHistory;
        }
    }
};


//WeaponInstance is managed by weaponManager
class WeaponInstance : public Instance {
public:
    float damage;
    //collision box
    
    bool homing;
    
    WeaponType wepType;
    
};


class LazerDustParticleEffect : public Instance { //Do glDrawDots or whatever
    float red, green, blue;
    int framesRemaining;
    aiVector2D * position;
    
    LazerDustParticleEffect() = delete;
    LazerDustParticleEffect(float xMidpoint, float yMidpoint, float red, float green, float blue) {
        position = new aiVector2D(xMidpoint, yMidpoint);
    }
    //Destructor
    ~LazerDustParticleEffect() {
        if (position != nullptr) {
            delete position;
            position = nullptr;
        }
    }
};

#endif /* Instance_h */
