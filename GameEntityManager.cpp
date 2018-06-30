//
//  GameEntityManager.cpp
//
//  Created by Forrest Miller on 2/19/18.
//

#include "GameEntityManager.h"


void GameEntityManager::ageObjects() { //Function to make it so all game objects have their age incremented at the same time
    if (this->generator != nullptr) {
        Instance ** instances = generator->getArrayOfInstances();
        for (int i = 0; i < generator->getInstanceCount(); ++i) {
            instances[i]->timeAlive += TIME_TICK_RATE;
        }
    }
    else if (PRINT_DEBUG_WARNING_MESSAGES) {
        std::cout << "\nDEBUG::Warning! ageObjects() in gameEntityManager was called within a gameEntityManager-type\nobject that has a null Generator!";
        std::cout << std::endl;
    }
}
