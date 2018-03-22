//  This is a (hopefully) simple class that any in-game entity (aka instance) that
//  has the need to spawn weapon-type objects will have its own version of. This
//  class is responsible for tracking ammo count, for spawning weapons and
//  (possibly) for facilitaing weapon-entity collisions (or at least for passing
//  position data from manager object to manager object)
//
//  WeaponTracker.h

//  Created by Forrest Miller on 3/8/18.
//

#ifndef WeaponTracker_h
#define WeaponTracker_h

#include <iostream>

//#include "PlayerManager"
//#include "GameEntityManager.h"
//#include "WeaponManager.h"
//#include "CollisionRectangle.h"
#include "GameParameters.h"


//This is a struct for tracking ammo counts all in the same place
typedef struct AmmoCount{
public:
    //Ammo counts
    int kinetic;
    int missles;
    int hexagonBombs;
    //Energy
    float energy; //Use for shields/lasers
    //Shield health
    float shieldHealth;
    //Add a boost ability by double tapping a direction?
    
    //Constructor (just pull paramters from GameParamters)
    AmmoCount() {
        this->kinetic = STARTING_PLAYER_KINETIC_AMMO;
        this->missles = STARTING_PLAYER_ROCKETS;
        this->hexagonBombs = STARTING_PLAYER_HEXAGON_BOMBS;
        this->energy = STARTING_PLAYER_ENERGY;
        this->shieldHealth = STARTING_PLAYER_SHIELDS;
    }
} AmmoCount;

//This weaponTracker class will be attached to instances to help faciliate their weapons capabilities
class WeaponTracker {
private:
    int nextSpawnPoint;
    int weaponSpawnPointsCount;
    aiVector3D * weaponSpawnPoints; //Coordinate positions of where the weapon should spawn relative to the model orgin. This is
                                    //the array of vectors that should rotate with the model
    aiVector2D position; //player's position
    aiVector2D forwardDirection; //direction player is facing
    aiVector2D velocity; //player velocity
    float earlyThetaZ;
    float thetaX;
    float thetaZ;
    float instanceZoomAmount;
    
    bool kineticActive;
    bool kineticWasFired;
    
    AmmoCount ammo;
    
    void rotateWeaponSpawnPoint(aiVector3D & spawnPoint) { //To be called as rotations are updated
        //Since rotations happen in each type of weapon entity manager, don't do rotations here...
        //std::cout << "Words\n";
    }
public:
    WeaponTracker() {
        //Initialize everything
        nextSpawnPoint = 0;
        weaponSpawnPointsCount = 0;
        weaponSpawnPoints = nullptr;
        
        position = aiVector2D(0.0f, 0.0f);
        forwardDirection = aiVector2D(0.0f, 1.0f);
        velocity = aiVector2D(0.0f, 0.0f);
        earlyThetaZ = 0.0f;
        thetaX = 0.0f;
        thetaZ = 0.0f;
        
        instanceZoomAmount = 1.0f;
        
        //Set default starting active weapon
        kineticActive = true;
        kineticWasFired = false;
        
    }
    ~WeaponTracker() {
        if (weaponSpawnPoints != nullptr) {
            delete [] weaponSpawnPoints;
            weaponSpawnPoints = nullptr;
        }
    }
    
    //Getters
    int getWeaponSpawnPointsCount() const {return this->weaponSpawnPointsCount;}
    
    aiVector3D getNextSpawnPoint() {
        if (this->weaponSpawnPoints != nullptr) {
            aiVector3D wepSpawnPosition = (weaponSpawnPoints[nextSpawnPoint]);//For some reason this isn't setting the vector correctly
//            //Try to do it manually...  (THE PROBLEM IS WITH XCODE's debugger)
//            wepSpawnPosition.x = weaponSpawnPoints[nextSpawnPoint].x;
//            wepSpawnPosition.y = weaponSpawnPoints[nextSpawnPoint].y;
//            wepSpawnPosition.z = weaponSpawnPoints[nextSpawnPoint].z;
            nextSpawnPoint = (nextSpawnPoint + 1) % weaponSpawnPointsCount; //cycle through the spawn points
            
            rotateWeaponSpawnPoint(wepSpawnPosition);
            return wepSpawnPosition;
        }
        else {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! No weapon spawn points have been set for this instance's weaponTracker!\n";
            }
            return aiVector3D(0.0f, 0.0f, 0.0f);
        }
    }
    bool getHasWeponSpawnPointsSet() const {return (this->weaponSpawnPoints != nullptr);}
    AmmoCount viewAmmoCount() const {return this->ammo;} //This function copies ammo
    AmmoCount& getAmmoCount() {return this->ammo;} //This function gives a modifiable reference to ammo
    aiVector2D getPosition() const {return this->position;}
    aiVector2D getForwardDirection() const {return this->forwardDirection;}
    aiVector2D getVelocity() const {return this->velocity;}
    float getEarlyThetaZ() const {return this->earlyThetaZ;}
    float getThetaX() const {return this->thetaX;}
    float getThetaZ() const {return this->thetaZ;}
    bool getKineticActive() const {return this->kineticActive;}
    bool getKineticWasFired() const {return this->kineticWasFired;}
    //bool get<otherWeaponType>Active() const {return otherWeaponTypeActive;} //Once more weapon types are implemented
    
    //Setter functions to be used by player instances
    void setNewWeaponSpawnpoints(aiVector3D * spwnPointsArray, int numSpawnPoints) {
        if (numSpawnPoints <= 0) {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! Attempting to set WeaponTracker up with an invalid number of spawn points!\n";
            }
            //return;
        }
        else if (this->weaponSpawnPoints == nullptr) {
            weaponSpawnPointsCount = numSpawnPoints;
            this->weaponSpawnPoints = new aiVector3D[weaponSpawnPointsCount];
            for (int i = 0; i < weaponSpawnPointsCount; i++) {
                weaponSpawnPoints[i] = spwnPointsArray[i];
                std::cout << "Weapon spawn point set to: " << weaponSpawnPoints[i].x << ", " << weaponSpawnPoints[i].y << ", " << weaponSpawnPoints[i].z << std::endl;
            }
        }
        else {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! TRYING TO CHANGE WeaponSpawnPointArray on a weapon tracker that already had this set!\n";
            }
        }
    }
    void setPosition(const aiVector2D & position) {this->position = position;}
    void setVelocity(const aiVector2D & velocity) {this->velocity = velocity;}
    void setForwardDirection(const aiVector2D & fwrd) {this->forwardDirection = fwrd;}
    void setEarlyThetaZ(float earlyThetaZ) {this->earlyThetaZ = earlyThetaZ;}
    void setThetaX(float thetaX) {this->thetaX = thetaX;}
    void setThetaZ(float thetaZ) {this->thetaZ = thetaZ;}
    void setInstanceZoomAmount(float zoom) {this->instanceZoomAmount = zoom;}
    
    
    
    //Set weapons that were fired
    void setKineticWasFired() {this->kineticWasFired = true;}
    
    void resetWeaponsFired() {
        this->kineticWasFired = false;
        //set others as well...
    }
    
    void switchActive() {
        //Turn off currently active
        //Turn on the next one to be activated
    }
    
};



#endif /* WeaponTracker_h */
