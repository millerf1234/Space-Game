//
//  WeaponManager.cpp
//
//  Created by Forrest Miller on 3/8/18.
//

#include "WeaponManager.h"

WeaponManager::WeaponManager() : GameEntityManager() {
    //GameEntityManager's constructer will handle setting everything to null
    
    //from stage:
    //GameEntity's constructor will set everything to 0 or Nullptr
//    this->initTemplate = new InitializationTemplate;
//    this->generator = new Generator();
//    this->hasGenerator = true;
//    this->hasCollision = false;
//    this->stageNumber = ++numberOfStages;
//
//    //Have the stage initialize it's generator from it's template
//    generateInitializationTemplate(); //Generate the initialization template for Stage
//    initializeFromTemplate(); //Initialize Generator from the template
    
    
    //from Player:
//    this->initTemplate = new InitializationTemplate;
//    this->generator = new Generator;
//    this->hasGenerator = true;
//    this->hasCollision = true;
//    this->requiresUserInput = true; //Player controlled characters require user input
//    //Set specialization type
//    this->specType = specializationType::PLAYER;
//    //Set-up the initialization template
//    this->generateInitializationTemplate();
//    //    this->generator->setSpecialization(PLAYER);
//    //this->generator->specialization = specializationType::PLAYER;
//    //Initialize the generator from the initialization template
//    this->initializeFromTemplate(); //generator->setSpecialization now called inside this function

}

void WeaponManager::generateInitializationTemplate() {
    
}

void WeaponManager::initializeFromTemplate() {
    
}


void WeaponManager::doUpkeep() {
    
}
