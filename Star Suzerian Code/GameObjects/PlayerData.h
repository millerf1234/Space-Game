//
//  PlayerData.h
//  Created by Forrest Miller on 2/19/18.
//

#ifndef PlayerData_h
#define PlayerData_h

#include "GameParameters.h"

typedef struct PlayerData {
    float health;
    float shields;
    float energy;
    float fuel;
    int playerNumber;
    int rocketCount;
    int maxRockets;
    
    
    
    
    PlayerData(int playerNumber) {
        this->playerNumber = playerNumber;
        health = STARTING_PLAYER_HEALTH;
        shields = STARTING_PLAYER_SHIELDS;
        energy = STARTING_PLAYER_ENERGY;
        rocketCount = STARTING_PLAYER_ROCKETS;
        maxRockets = STARTING_PLAYER_ROCKET_COUNT_MAX;
    }
    
} PlayerData;

#endif /* PlayerData_h */
