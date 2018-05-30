// This whole death animation thing is poorly implemented and breaks a whole bunch of rules for
// good form and really should not be looked at by anyone. I feel very bad for each line of code
// written here to get this to work properly...
//
//  DeathAnimation.h
//
//  This class is intended to temporarily take over control of the game to play a death animation
//
//  Created by Forrest Miller on 5/26/18.
//

#ifndef DEATH_ANIMATION
#define DEATH_ANIMATION

#include <iostream>
#include <random>
#include <chrono> //requires C++11
#include "GameParameters.h"
#include "Quaternion.h"
#include "PlayerManager.h"
#include "WeaponOverseer.h"
#include "Stage.h"
//#include "PlayerParticle.h" //comes with the manager
#include "PlayerParticleManager.h"

namespace DeathAnimation {
typedef struct GameStateInfoForDeathAnimation {
    Stage * stage;
    PlayerManager * playerManager;
    WeaponOverseer * weaponOverseer;
    std::vector<GameEntityManager *> gEntities; //Add this in as well
    //std::vector<WeaponInstance*> activeWepInstances; //This is part of weapon overseer
    GLFWwindow * mWindow; //Need this to handle rendering within the death animation
} GameStateInfoForDeathAnimation;

void playDeathAnimation(GameStateInfoForDeathAnimation *, PlayerEntity *, PlayerParticleManager *);

} //Namespace DeathAnimation

#endif // DEATH_ANIMATION
