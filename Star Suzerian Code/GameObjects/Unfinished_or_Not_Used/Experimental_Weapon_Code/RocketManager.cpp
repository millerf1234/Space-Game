//
//  RocketManager.cpp
//
//  Created by Forrest Miller on 2/26/18.
//

#include "RocketManager.h"


RocketManager::RocketManager() {
    generator = new Generator();
}

RocketManager::~RocketManager() {
    if (generator != nullptr) {
        delete generator;
        generator = nullptr;
    }
}

void RocketManager::initializeFromTemplate() {
    
}

void RocketManager::generateInitializationTemplate() {
    
}

void RocketManager::doUpkeep() {
    
}

void RocketManager::handleInput(GLFWwindow *) {
    
}

void RocketManager::drawInstances() {
    this->generator->drawInstances();
}
