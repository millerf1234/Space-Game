//
//  Player.cpp
//
//  Created by Forrest Miller on 2/13/18.


#include "PlayerManager.h"

PlayerManager::PlayerManager() : GameEntityManager() {
    this->initTemplate = new InitializationTemplate;
    this->generator = new Generator;
    this->hasGenerator = true;
    this->hasCollision = true;
    this->requiresUserInput = true; //Player controlled characters require user input
    //Set specialization type
    this->specType = specializationType::PLAYER;
    this->generator->specialization = specializationType::PLAYER;
    
    
    //Set-up the initialization template
    this->generateInitializationTemplate();
    //Initialize the generator from the initialization template
    this->initializeFromTemplate();
}

PlayerManager::~PlayerManager() {
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

void PlayerManager::doUpkeep() {
    generator->doUpkeep();
}

void PlayerManager::handleInput() {
    //For each instance in generator, handle input
}

void PlayerManager::drawInstances() {
    
    generator->drawInstances();
}

void PlayerManager::initializeFromTemplate() {
    generator->initializeFromTemplate(*initTemplate);
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        generator->generateSingle();
    }
    //I'm going to be super risky and do static_cast, because I know that these Instance *'s are to PlayerInstances. for more information, see: https://stackoverflow.com/questions/28002/regular-cast-vs-static-cast-vs-dynamic-cast
    Instance * players = generator->getArrayOfInstances();
    if (MAX_PLAYERS < 2) {
        std::cout << "\nDEBUG::GAME WILL BE PLAYED WITH A SINGLE PLAYER\n";
        
        players->zoom = 40.5f;
        
    }
    else if (MAX_PLAYERS < 3) {
        std::cout << "\nDEBUG::GAME WILL BE PLAYED WITH 2 PLAYERS. LOADED 2 PLAYERS!\n";
        players[0].zoom = 40.5f;
        players[1].zoom = 40.5f;
    }
    else {
        std::cout << "\n\n!!!! OOOPS!!! THIS NUMBER OF PLAYERS IS NOT YET SUPPORTED!\nTO ADD CODE TO SUPPORT MORE PLAYERS, SEE WHERE THIS MESSAGE IS PRINTED\nIN \"PLAYERMANAGER::INITIALIZE_FROM_TEMPLATE\"";
    }
    //std::cout << "\n\nFirst 30 vertices are: \n";
    //for (int i = 0; i < 30; ++i) {
    //    std::cout << "vert[" << i << "] = " << generator->vertices[i] << std::endl;
    //}
    //std::cout << "\n\nFirst 30 elements are: \n";
    //for (int i = 0; i < 30; ++i) {
    //    std::cout << "elem[" << i << "] = " << generator->elements[i] << std::endl;
    //}
}

void PlayerManager::generateInitializationTemplate() {
    initTemplate->vertShaderPath = PLAYERSHIP_BODY_VERT;
    initTemplate->hasVert = true;
    initTemplate->fragShaderPath = PLAYERSHIP_BODY_FRAG;
    initTemplate->hasFrag = true;
    
    initTemplate->vertAttribName = "position";
    
    //If doing format vert3normal3 (NOTE: DON"T DO THIS FORMAT) then uncomment these lines:
    //initTemplate->normalAttribName = "norml";
    //initTemplate->vert3norml3 = true;
    
    //If doing format vert3, then do this:
    initTemplate->vert3 = true;
    
    //Player models will be loading from an obj file, so set this to false
    initTemplate->hasVertsAlreadyLoaded = false;
    initTemplate->modelFilePath = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/space_ship2.obj";
    
}
