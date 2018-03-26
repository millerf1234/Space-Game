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
    for (int i = 0; i < numWepTrackers; ++i) {
        WeaponTracker * currentWT = this->wepTrackers[i];
        
        //Check events on the currentWT
        if (currentWT->getKineticWasFired()) {
            this->kineticWepManager->spawnNewKineticInstance(currentWT);
        }
        //Check currentWT for additional flags of weapons being fired...
    }
}
