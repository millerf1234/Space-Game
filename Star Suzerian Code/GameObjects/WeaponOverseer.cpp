//
//  WeaponOverseer.cpp
//
//  Created by Forrest Miller on 3/8/18.
//

#include "WeaponOverseer.h"


WeaponOverseer::WeaponOverseer() {
    //Set the array of pointer to weapon tracker to null
    this->wepTrackers = nullptr;
    this->numWepTrackers = 0; //0 weapon trackers in existance initially
    
    this->kineticWepManager = nullptr; //Will be nullptr until initializeWeapons() is called
    
}

WeaponOverseer::~WeaponOverseer() {
    if (this->numWepTrackers > 0 && this->wepTrackers != nullptr) {
        for (int i = 0; i < numWepTrackers; i++) {
            if (wepTrackers[i] != nullptr) {
                delete wepTrackers[i];
                wepTrackers[i] = nullptr;
            }
        }
    }
}


void WeaponOverseer::initializeWeapons() {
    //Game prints: "        Loading weapons...\n"
    //and then Game calls WeaponManager.initializeWeapons()
    
    //Load Kinetic weapon type
    std::cout << "            Loading Kinetic...\n";
    this->kineticWepManager = new KineticWeaponManager;
    if (this->kineticWepManager->getIsReady()) {
        std::cout << "                Kinetic weapon-type Ready!\n";
    }
    else {
        std::cout << "OOPS! THERE WAS AN ISSUE WHILE LOADING KINETIC WEAPON_TYPE!\n";
    }
    
    //Load more weapon types
    
    
}

void WeaponOverseer::ageWeaponInstances()  {
    kineticWepManager->ageObjects();
    //other wepManagers->ageObjects;
    
}

void WeaponOverseer::doUpkeep() {
    kineticWepManager->doUpkeep();
    //other wepManagers->doUpkeep();
}

void WeaponOverseer::generateAndAttachWeaponTrackerToInstance(Entity * entity) {
    //Check to see if the instance already has a weaponTracker
    if (entity->hasWeaponTracker) {
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::\nWARNING! This instance already has a Weapons Tracker attached!\n";
        }
        return;
    }
    
    //Print a debug message:
    std::cout << "\nWeaponTracker attached to Instance " << entity->getID() << ".\n";
    if (entity->type == InstanceType::PLAYERENTITY) {
        PlayerEntity * pTemp = static_cast<PlayerEntity *>(entity);
        std::cout << "Instance " << entity->getID() << " is Player " << pTemp->playerNumber << std::endl;
    }
    
    //Else check to see if this WeaponOverseer already has weaponTrackers in existance
    if (this->wepTrackers != nullptr) {
        //Need to make a bigger array to hold the additional weaponTracker
        WeaponTracker ** temp = new WeaponTracker* [numWepTrackers + 1];
        //copy all of the weaponTrackers in the old array over
        for (int i = 0; i < numWepTrackers ; i++) {
            temp[i] = wepTrackers[i];
        }
        
        //Delete the no-longer-needed array
        delete [] wepTrackers;
        wepTrackers = temp; //Set the tracked array to be the new array
        
        wepTrackers[numWepTrackers] = new WeaponTracker;
        
        entity->attachWeaponTracker(wepTrackers[numWepTrackers]); //Give the WeaponTracker to inst, and have inst handle configuring/attaching the WeaponTracker to itself
         ++numWepTrackers; //Last but not least, increment the count of weapon trackers
        
        //Just a reality check message
        std::cout << "\nThere are now " << numWepTrackers << " weapon trackers in existance and\n";
        std::cout << " being managed by the WeaponOverseer!\n";
        
    }
    
    //Else there must not have been any Weapons Trackers generated yet, so set up the tracking of Weapon Trackers
    else {
        this->numWepTrackers = 1;
        this->wepTrackers = new WeaponTracker* [numWepTrackers];
        this->wepTrackers[0] = new WeaponTracker;
        entity->attachWeaponTracker(wepTrackers[0]);
    }
}

void WeaponOverseer::processWeaponTrackers() {
    if (this->numWepTrackers == 0) {return;}
    
    //Without checking for time between firing just do the following:
    for (int i = 0; i < numWepTrackers; ++i) {
        WeaponTracker * currentWT = this->wepTrackers[i];
        //Check events on the currentWT
        if (currentWT->getKineticWasFired()) {
            this->kineticWepManager->spawnNewKineticInstance(currentWT);
        }
    }
    
    /*
    //To check for time between firing (This code really should be done somewhere else)...
    //I hacked this together quickly to get it so players can only shoot so much so often.
    //This is really NOT the way to implement this because its confusing and is bad design
    //(I should replace this with a better solution if I ever get time)
    static short player1ShotsFired = 0;
    static short player1ShotsCooldownTimer = 0;
    static short player2ShotsFired = 0;
    static short player2ShotsCooldownTimer = 0;
    static bool player1OnCooldown = false;
    static bool player2OnCooldown = false;
    
    for (int i = 0; i < numWepTrackers; ++i) {
        //Increment the timers
        if (i == 0) {
            player1ShotsCooldownTimer++;
            if (player1ShotsCooldownTimer >= 14) {
                player1OnCooldown = false;
                player1ShotsFired = 0;
            }
        }
        else if (i == 1) {
            player2ShotsCooldownTimer++;
            if (player2ShotsCooldownTimer >= 14) {
                player2OnCooldown = false;
                player2ShotsFired = 0;
            }
        }
        
        //get the weapon's tracker
        WeaponTracker * currentWT = this->wepTrackers[i];
        
        
        if (i == 0) {
            if (player1OnCooldown) {
                continue;
            }
            else {
                if (currentWT->getKineticWasFired()) {
                    this->kineticWepManager->spawnNewKineticInstance(currentWT);
                    player1ShotsFired++;
                    player1ShotsCooldownTimer = 0;
                    if (player1ShotsFired > 2) {
                        player1OnCooldown = true;
                    }
                }
                else {
                    if (player1ShotsFired > 0) {
                        player1ShotsFired--;
                    }
                }
            }
        }
        
        if (i == 1) {
            if (player2OnCooldown) {
                continue;
            }
            else {
                if (currentWT->getKineticWasFired()) {
                    this->kineticWepManager->spawnNewKineticInstance(currentWT);
                    player2ShotsFired++;
                    player2ShotsCooldownTimer = 0;
                    if (player2ShotsFired > 2) {
                        player2OnCooldown = true;
                    }
                }
                else {
                    if (player2ShotsFired > 0) {
                        player2ShotsFired--;
                    }
                }
            }
        }
     
     }
     */
    
    
    
    //Check currentWT for additional flags of weapons being fired...
    
}
