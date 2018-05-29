//
//  PlayerParticleManager.cpp
//
//  Created by Forrest Miller on 5/29/18.
//

#include "PlayerParticleManager.h"





//Code I started writing from when PlayerParticleManager inherited from EntityManager
/*
PlayerParticleManager::PlayerParticleManager() {
    this->initTemplate = new InitializationTemplate;
    this->generator = new Generator;
    this->hasGenerator = true;
    this->hasCollision = true;
    this->requiresAIInput = this->requiresUserInput = false;
    
    this->specType = PARTICLE;
    
    this->generateInitializationTemplate();
    
    this->initializeFromTemplate();
}


PlayerParticleManager::~PlayerParticleManager() {
    if (PRINT_DESTRUCTOR_CALLS) {
        std::cout << "\nDEBUG:PlayerParticleManager destructor called!";
    }
    if (this->initTemplate != nullptr) {
        if (this->initTemplate->hasVertsAlreadyLoaded) {
            //Then delete heap data that was used
            if (this->initTemplate->vertices != nullptr) {
                delete [] initTemplate->vertices;
                initTemplate->vertices = nullptr;
            }
            if (initTemplate->elements != nullptr) {
                delete [] initTemplate->elements;
                initTemplate->elements = nullptr;
            }
        }
        delete this->initTemplate;
        this->initTemplate = nullptr;
    }
    if (this->generator != nullptr) {
        delete this->generator;
        this->generator = nullptr;
    }
}
*/
