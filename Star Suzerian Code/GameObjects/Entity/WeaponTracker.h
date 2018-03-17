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

constexpr int MAX_WEAPON_SPAWN_POINTS = 4;


class WeaponTracker {
private:
    
public:
    
    WeaponTracker() {
        
    }
    
    ~WeaponTracker() {
        
    }
    
    
    
};



#endif /* WeaponTracker_h */
