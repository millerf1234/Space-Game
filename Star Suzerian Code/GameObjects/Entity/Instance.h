//
//  Instance.h
//
//  This header will eventually be broken apart into each class in its own file
//
//  Created by Forrest Miller on 2/19/18.
//

#ifndef Instance_h
#define Instance_h

#include <stdio.h>
#include "GameParameters.h"
#include <assimp/Importer.hpp> //Gives access to the aiVector3D
#include "Quaternion.h"
#include "CollisionBox.h"
#include "CollisionRectangle.h"
#include "WeaponTracker.h"


enum InstanceType {PLAYERENTITY, WEAPONINSTANCE, ENEMYINSTANCE, SCENEINSTANCE, SCENECOLLISIONINSTANCE, BASIC}; //Used for instance identification
enum WeaponType {HEXAGON_BOMB, LASER, ROCKET, HOMINGROCKET, KINETIC, UNINITIALIZED}; //Used in conjunction with WeapontypeManagers
//Here is a typedef for an instance of what was generated


//------------------------------------------------------------------------------
//                              Hierarchy
//
// top            +---------------------+
//                |     Renderable      |
//                |         -Position   |
//                |         -Velocity   |    ------------------>  Particles
//                |         -Time Alive |
//                |         -Zoom       |
//                +---------------------+
//                           |
//                           |
//                           |
//                           v
//              +---------------------------+
//              |     Instance              |
//              |        -ID Number         |
//              |        -Forward direction |
//              |        -Orientation       |-------------------+
//              |        -Collision Box     |                   |
//              |        -Mass              |                   |
//              +---------------------------+                   |
//                           |                                  |
//                           |                                  |
//                           |                                  |
//                           v                                  |
//              +---------------------------+                   |
//              |       Entity              |                   |
//              |        - Weapon Tracker   |   (Note that WeaponTracker should only be created/destroyed by WeaponManager)
//              +---------------------------+                   |
//                  /
//                 /                                            +------------------------------+
//              \ /                                                                            |
//               V___                                                                          |
//    +-----------------------+                                                                v
//    |   Player Entity       |                                                   +------------------------------+
//    |      -Colors          |                                                   |         Weapon Instance      |
//    |      -Health          |                                                   |             -Weapon Type     |
//    |      -Shields         |                                                   |             -Damage          |
//    |      -Player number   |                                                   |             -Homing          |
//    |      -Animation Frame |                                                   |                              |
//    +-----------------------+                                                   +------------------------------+
//                                                                                     /
//                                                                                    /
//                                                                                 \ /
//                                                                                  V___
//                                                                          +------------+
//                                                                          |   Kinetic  |
//                                                                          |            |
//                                                                          |            |
//                                                                          +------------+
//------------------------------------------------------------------------------



class Renderable {
public:
    virtual ~Renderable() {
        if (PRINT_DESTRUCTOR_CALLS) {std::cout << "\nDEBUG:Renderable destructor called!";}
    }
    aiVector3D position;
    aiVector2D velocity;
    float zoom;
    
    float timeAlive;
    
};

class Instance : public Renderable {
public:
    int identifierNumber; //Unique number attached to each instance of any object
    //These are all basically just values to give the GPU for each instance
    //aiVector3D position;
    //aiVector2D velocity; //Velocity is applied to position in each generator's upkeep function
    aiVector3D * forward; //Moving to have this be on all instances
    //float zoom;
    float thetaX, thetaY, thetaZ; //Euler Rotation Angles
    float mass; //Might use this to make missle/projectile strikes affect player velocity
    
    //float timeAlive;
    InstanceType type;
    
    CollisionBox * colBox;
    
   // bool hasWeaponTracker;
   // WeaponTracker * wepTracker; //To be attached and removed by WeaponManager
    
    //Constructors
    Instance() {//Construct an anonymous instance
        this->identifierNumber = -1;
        this->type = BASIC;
        this->mass = 1.0f;
        this->velocity.x = this->velocity.y = 0.0f;
        this->forward = nullptr;
        this->colBox = nullptr;
        //this->wepTracker = nullptr;
       // this->hasWeaponTracker = false;
    }
    Instance(int id) { //Construct an instance that has an ID number
        this->identifierNumber = id;
        this->type = BASIC;
        this->mass = 1.0f;
        velocity.x = velocity.y = 0.0f;
        this->forward = nullptr;
        this->colBox = nullptr;
       // this->wepTracker = nullptr;
       // this->hasWeaponTracker = false;
    }
    
    virtual ~Instance() {
        if (PRINT_DESTRUCTOR_CALLS) {std::cout << "\nDEBUG:Instance destructor called!";}
        if (this->colBox != nullptr) {delete this->colBox; this->colBox = nullptr;}
    }
    
//    AmmoCount getAmmoCount() const {
//        if (!hasWeaponTracker) {
//            if (PRINT_DEBUG_WARNING_MESSAGES) {
//                std::cout << "\nDEBUG::WARNING! getAmmoCount called on an instance that does not have a weaponTracker attached to it!\n";
//            }
//            AmmoCount temp;
//            return temp; //Return a bogus AmmoCount to keep the function happy and not dereference a nullptr
//        }
//        return this->wepTracker->getAmmoCount();
//    }
//
//    void attachWeaponTracker(WeaponTracker * wp) {
//        if (this->hasWeaponTracker) {
//            if (PRINT_DEBUG_WARNING_MESSAGES) {
//                std::cout << "\nDEBUG::WARNING! ATTEMPTING TO ATTACH WEAPON TRACKER TO AN INSTANCE THAT ALREADY HAS A WEAPON TRACKER!\n";
//            }
//            return;
//        }
//        if (wp != nullptr) {
//            this->wepTracker = wp;
//            this->hasWeaponTracker = true;
//        }
//        else {
//            if (PRINT_DEBUG_WARNING_MESSAGES) {
//                std::cout << "\nDEBUG::Warning! Attempting to attach a null weapon tracker to this instance\n";
//            }
//            this->hasWeaponTracker = false;
//        }
//        configureWeaponTracker();
//    }
//
//    virtual void configureWeaponTracker() {
//        if (this->wepTracker->getHasWeponSpawnPointsSet()) {
//            if (PRINT_DEBUG_WARNING_MESSAGES) {
//                std::cout << "\nDEBUG::OOPS! Are you sure you want to configure this weapons tracker twice?\n";
//                return;
//            }
//        }
//        //Print a message just in case (can suppress this later if need be)
//        std::cout << "\nDEBUG::BASE INSTANCE configureWeaponTracker function called. Are you sure you didn't mean\n";
//        std::cout << "to call the instance-type-specific version of this function?\n";
//
//        //Actually start configuring the WeaponsTracker now:
//        //Give's weaponTracker the weapon spawn point coordinates to launch from
//        aiVector3D spawnPointOffsets [1]; //Since this is just the base Instance class, no special offset points
//        *spawnPointOffsets = aiVector3D(0.0f, 0.0f, 0.0f);
//        this->wepTracker->setNewWeaponSpawnpoints(spawnPointOffsets, 1);
//
//        //Set the rest of the data as well now
//        matchWepTrackerWithInstData();
//    }
//
//    //Call this next function every loop iteration
//    virtual void updateWeaponTracker() {
//        this->wepTracker->resetWeaponsFired(); //Reset all weapons-fired flags
//        matchWepTrackerWithInstData();
//    }
    
    //Get the instance ID
    int getID() const {return this->identifierNumber;}
    
//protected:
//    //This function will
//    void matchWepTrackerWithInstData() {
//        wepTracker->setPosition(aiVector2D(position.x, position.y));
//        wepTracker->setVelocity(velocity);
//        if (this->forward == nullptr) {
//            wepTracker->setForwardDirection(aiVector2D(0.0f, 1.0f)); //Give it a vector
//            std::cout << "\nWarning! Forward may not have been set correctly on this instance!\n";
//        }
//        else {
//            wepTracker->setForwardDirection(aiVector2D((*forward).x, (*forward).y));
//        }
//        wepTracker->setEarlyThetaZ(0.0f); //Note that if EarlyThetaZ is used, it will need to be set seperatly from this function
//        wepTracker->setThetaX(thetaX);
//        wepTracker->setThetaZ(-thetaZ + PI); //For some reason it was off by PI
//        wepTracker->setInstanceZoomAmount(zoom);
//    }
};

class Entity : public Instance {
public:
    bool hasWeaponTracker;
    WeaponTracker * wepTracker; //To be attached and removed by WeaponManager
    
    
    //Constructor
    Entity() : Instance() {
         this->wepTracker = nullptr;
         this->hasWeaponTracker = false;
    }
    
    virtual ~Entity() {
        if (PRINT_DESTRUCTOR_CALLS) {
            std::cout << "\nDEBUG:Entity destructor called! Entity had an attached weapons tracker: ";
            if (hasWeaponTracker) {
                std::cout << "true";
            }
            else {
                std::cout << "false";
            }
        }
    }
    
    AmmoCount getAmmoCount() const {
        if (!hasWeaponTracker) {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! getAmmoCount called on an instance that does not have a weaponTracker attached to it!\n";
            }
            AmmoCount temp;
            return temp; //Return a bogus AmmoCount to keep the function happy and not dereference a nullptr
        }
        return this->wepTracker->getAmmoCount();
    }
    
    void attachWeaponTracker(WeaponTracker * wp) {
        if (this->hasWeaponTracker) {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! ATTEMPTING TO ATTACH WEAPON TRACKER TO AN INSTANCE THAT ALREADY HAS A WEAPON TRACKER!\n";
            }
            return;
        }
        if (wp != nullptr) {
            this->wepTracker = wp;
            this->hasWeaponTracker = true;
        }
        else {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::Warning! Attempting to attach a null weapon tracker to this instance\n";
            }
            this->hasWeaponTracker = false;
        }
        configureWeaponTracker();
    }
    
    virtual void configureWeaponTracker() {
        if (this->wepTracker->getHasWeponSpawnPointsSet()) {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::OOPS! Are you sure you want to configure this weapons tracker twice?\n";
                return;
            }
        }
        //Print a message just in case (can suppress this later if need be)
        std::cout << "\nDEBUG::BASE INSTANCE configureWeaponTracker function called. Are you sure you didn't mean\n";
        std::cout << "to call the instance-type-specific version of this function?\n";
        
        //Actually start configuring the WeaponsTracker now:
        //Give's weaponTracker the weapon spawn point coordinates to launch from
        aiVector3D spawnPointOffsets [1]; //Since this is just the base Instance class, no special offset points
        *spawnPointOffsets = aiVector3D(0.0f, 0.0f, 0.0f);
        this->wepTracker->setNewWeaponSpawnpoints(spawnPointOffsets, 1);
        
        //Set the rest of the data as well now
        matchWepTrackerWithInstData();
    }
    
    //Call this next function every loop iteration
    virtual void updateWeaponTracker() {
        this->wepTracker->resetWeaponsFired(); //Reset all weapons-fired flags
        matchWepTrackerWithInstData();
    }
    
protected:
    //This function will
    void matchWepTrackerWithInstData() {
        wepTracker->setPosition(aiVector2D(position.x, position.y));
        wepTracker->setVelocity(velocity);
        if (this->forward == nullptr) {
            wepTracker->setForwardDirection(aiVector2D(0.0f, 1.0f)); //Give it a vector
            std::cout << "\nWarning! Forward may not have been set correctly on this instance!\n";
        }
        else {
            wepTracker->setForwardDirection(aiVector2D((*forward).x, (*forward).y));
        }
        wepTracker->setEarlyThetaZ(0.0f); //Note that if EarlyThetaZ is used, it will need to be set seperatly from this function
        wepTracker->setThetaX(thetaX);
        wepTracker->setThetaZ(-thetaZ + PI); //For some reason it was off by PI
        wepTracker->setInstanceZoomAmount(zoom);
    }
};

class PlayerEntity : public Entity {
public:
    float health;
    float shields;
    float energy;
    float fuel;
    int playerNumber;
   // int rocketCount;
  ///  int maxRockets;
    
    //Color information
    float red;
    float green;
    float blue;
    
    float rollAmount;
//    aiVector3D * forward; //Moving to have this be on all instances
    
    int currentAnimationFrame;
    int maxAnimationFrame;
    
    //Movement information
    bool accelerate, decelerate, turnLeft, turnRight, rollLeft, rollRight, shoot;
    
    aiVector3D * rear;
    aiVector3D ** translationHistory;
    Quaternion * xRot, * yRot, * zRot;
    
    PlayerEntity() = delete;
//    PlayerInstance() {
//        if (PRINT_DEBUG_MESSAGES) {
//            std::cout << "DEBUG::OOPS! IT looks like the wrong PlayerInstance constructor was called";
//        }
//            rear = nullptr;
//            xRot = yRot = zRot = nullptr;
//
//    }
    
    PlayerEntity(int playerNumber) {
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "DEBUG::PlayerInstance constructor was called!" << std::endl;
        }
        this->playerNumber = playerNumber;
        this->type = PLAYERENTITY;
        health = STARTING_PLAYER_HEALTH;
        shields = STARTING_PLAYER_SHIELDS;
        energy = STARTING_PLAYER_ENERGY;
        fuel = STARTING_PLAYER_FUEL;
        //rocketCount = STARTING_PLAYER_ROCKETS;
        //maxRockets = STARTING_PLAYER_ROCKET_COUNT_MAX;
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
    
    virtual ~PlayerEntity() {
        if (PRINT_DESTRUCTOR_CALLS) {
            std::cout << "\nDEBUG:PlayerEntity destructor called! PlayerEntity had a weapon tracker attached: ";
            if (hasWeaponTracker) {
                std::cout << "true";
            }
            else {
                std::cout << "false";
            }
        }
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
    
    /*virtual */ void configureWeaponTracker() override {
        //I am adding in the second half of this statement (the part after the '&&') because it turns out I give a default weaponSPawnPoint array of size 1 to weapon trackers when they are created.
        if (this->wepTracker->getHasWeponSpawnPointsSet() && this->wepTracker->getNextSpawnPoint() != this->wepTracker->getNextSpawnPoint()) {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::OOPS! Are you sure you want to configure this weapons tracker twice?\n";
                return;
            }
        }
        //Actually start configuring the WeaponsTracker now:
        aiVector3D playerWepSpawnPoints[6];
        float weaponZSpawnCoord = 3.01f; //max positive model Z is 3.85924
        float weaponXSpawnCoordOuter = 2.0f; //max positive model X is 2.4450891
        float weaponXSpawnCoordInner = 0.5f;
        
        //old way
        playerWepSpawnPoints[0] = aiVector3D(-weaponXSpawnCoordOuter, 0.2f, weaponZSpawnCoord); //Outer left side
        playerWepSpawnPoints[1] = aiVector3D(-weaponXSpawnCoordInner, -0.2f, weaponZSpawnCoord); //Inner left side
        playerWepSpawnPoints[2] = aiVector3D(weaponXSpawnCoordOuter, 0.2f, weaponZSpawnCoord); //Outer right side
        playerWepSpawnPoints[3] = aiVector3D(weaponXSpawnCoordInner, -0.2f, weaponZSpawnCoord); //Inner right side
        playerWepSpawnPoints[4] = aiVector3D(weaponXSpawnCoordInner, 0.25f, weaponZSpawnCoord); //Extra inner?
        playerWepSpawnPoints[5] = aiVector3D(-weaponXSpawnCoordInner, -0.35f, weaponZSpawnCoord);
        
        this->wepTracker->setNewWeaponSpawnpoints(playerWepSpawnPoints, 6);
        
        
//        //new way (doesnt work
//        playerWepSpawnPoints[0] = aiVector3D(-weaponXSpawnCoordOuter, 0.2f, weaponZSpawnCoord); //Outer left side
//        playerWepSpawnPoints[1] = aiVector3D(-weaponXSpawnCoordInner, -0.2f, weaponZSpawnCoord); //Inner left side
//        playerWepSpawnPoints[2] = aiVector3D(weaponXSpawnCoordOuter, 0.2f, weaponZSpawnCoord); //Outer right side
//        playerWepSpawnPoints[3] = aiVector3D(weaponXSpawnCoordInner, -0.2f, weaponZSpawnCoord); //Inner right side
//        playerWepSpawnPoints[4] = aiVector3D(weaponXSpawnCoordInner, 0.25f, weaponZSpawnCoord); //Extra inner?
//        playerWepSpawnPoints[5] = aiVector3D(-weaponXSpawnCoordInner, -0.35f, weaponZSpawnCoord);
//
//        playerWepSpawnPoints[0] = aiVector3D(-weaponXSpawnCoordOuter, 0.2f, weaponZSpawnCoord); //Outer left side
//        playerWepSpawnPoints[1] = aiVector3D(-weaponXSpawnCoordInner, -0.2f, weaponZSpawnCoord); //Inner left side
//        playerWepSpawnPoints[2] = aiVector3D(weaponXSpawnCoordOuter, 0.2f, weaponZSpawnCoord); //Outer right side
//        playerWepSpawnPoints[3] = aiVector3D(weaponXSpawnCoordInner, -0.2f, weaponZSpawnCoord); //Inner right side
//        playerWepSpawnPoints[4] = aiVector3D(weaponXSpawnCoordInner, 0.25f, weaponZSpawnCoord); //Extra inner?
//        playerWepSpawnPoints[5] = aiVector3D(-weaponXSpawnCoordInner, -0.35f, weaponZSpawnCoord);
//        this->wepTracker->setNewWeaponSpawnpoints(playerWepSpawnPoints, 12);
        
        //Set the rest of the data as well now
        matchWepTrackerWithInstData();
        this->wepTracker->setEarlyThetaZ(rollAmount);
        
    }
    
    /*virtual */ void updateWeaponTracker() override {
        this->wepTracker->resetWeaponsFired();
        matchWepTrackerWithInstData();
        this->wepTracker->setEarlyThetaZ(rollAmount); //
    }
    
};


//WeaponInstance is managed by weaponManager
class WeaponInstance : public Instance {
public:
    
    float damage;
   // aiVector3D modelOffsetLaunchPoint;
    
    
    bool homing;
    
    bool shouldBeDestroyed;
    
    //aiVector3D velocityModelRelative;
    //aiVector2D velocityScreenSpace;
    
    
    //Constructor
    WeaponInstance() = delete; //No default constructor
    WeaponInstance(int instanceID) : Instance(instanceID) {
        this->type = WEAPONINSTANCE;
        this->wepType = WeaponType::UNINITIALIZED;
        this->shouldBeDestroyed = false; //Probably not the best way to do this...
    }
    
    virtual ~WeaponInstance() {
        if (PRINT_DESTRUCTOR_CALLS) {
            std::cout << "\nDEBUG::WeaponInstance destructor called!\n";
        }
    }
    aiVector2D collisionBoxMidpoint; //This is to be CPU transformed for collision box
    
    WeaponType wepType;
    
    //Add an abstract virtual function to make WeaponInstance abstract
    WeaponType getWeaponType() const {return this->wepType;}
    
};


//class LazerDustParticleEffect : public Instance { //Do glDrawDots or whatever
//    float red, green, blue;
//    int framesRemaining;
//    aiVector2D * position;
//
//    LazerDustParticleEffect() = delete;
//    LazerDustParticleEffect(float xMidpoint, float yMidpoint, float red, float green, float blue) {
//        position = new aiVector2D(xMidpoint, yMidpoint);
//    }
//    //Destructor
//    ~LazerDustParticleEffect() {
//        if (position != nullptr) {
//            delete position;
//            position = nullptr;
//        }
//    }
//};

#endif /* Instance_h */
