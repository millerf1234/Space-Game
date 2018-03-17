//
//  WeaponManager.cpp
//
//  Created by Forrest Miller on 3/8/18.
//

#include "WeaponOverseer.h"


WeaponOverseer::WeaponOverseer() {
    //Set the array of pointer to weapon tracker to null
    this->wepTrackers = nullptr;
    this->numWepTrackers = 0; //0 weapon trackers in existance initially
    
    this->KineticWepManager = nullptr; //Will be nullptr until initializeWeapons() is called
    
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
    this->KineticWepManager = new KineticWeaponManager;
    if (this->KineticWepManager->getIsReady()) {
        std::cout << "                Kinetic weapon-type Ready!\n";
    }
    else {
        std::cout << "OOPS! THERE WAS AN ISSUE WHILE LOADING KINETIC WEAPON_TYPE!\n";
    }
}

void WeaponOverseer::ageWeaponInstances()  {
    KineticWepManager->ageObjects();
    //other wepManagers->ageObjects;
    
}

void WeaponOverseer::doUpkeep() {
    KineticWepManager->doUpkeep();
    //other wepManagers->doUpkeep();
}
