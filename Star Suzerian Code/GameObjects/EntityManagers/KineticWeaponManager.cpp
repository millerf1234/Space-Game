//
//  KineticWeaponManager.cpp
//
//  Created by Forrest Miller on 3/16/18.
//

#include "KineticWeaponManager.h"

KineticWeaponManager::KineticWeaponManager() : GameEntityManager() {
    //GameEntityManager's constructer will handle setting everything to null
    
    
    this->initTemplate = new InitializationTemplate;
    this->generator = new Generator();
    this->hasGenerator = true;
    this->hasCollision = true;
    this->requiresUserInput = false;
    this->requiresAIInput = false;
    this->specType = specializationType::WEAPON;
    this->generator->specialization = specializationType::WEAPON;
    
    this->generateInitializationTemplate();
    
    //Configure the generator based off the initialization Template()
    this->initializeFromTemplate();
    
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

KineticWeaponManager::~KineticWeaponManager() {
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
void KineticWeaponManager::generateInitializationTemplate() {
    //Set up the initializationTemplate for kinetic
    initTemplate->hasTexture = false;
    initTemplate->vertAttribName = "position";
    initTemplate->vert3 = true;
    
    initTemplate->vertShaderPath = KINETIC_VERT;
    initTemplate->hasVert = true;
    initTemplate->fragShaderPath = KINETIC_FRAG;
    initTemplate->hasFrag = true;
    
    
    initTemplate->typeDataRequired = WEAPON;
    
    //Model data
    initTemplate->hasVertsAlreadyLoaded = true;
    initTemplate->numVerts = KINETIC_PROJECTILE_VERTS_COUNT * 2;
    initTemplate->vertices = new GLfloat[initTemplate->numVerts];
    //This should be done with draw arrays, but going to use an element buffer anyways
    initTemplate->numElems = (KINETIC_PROJECTILE_VERTS_COUNT * 2) / 3;
    initTemplate->elements = new GLuint[initTemplate->numElems];
    
    //Give initTemplate the actual vertex data
    int halfPyrimidVertCount = initTemplate->numVerts / 2;
    for (int i = 0; i < halfPyrimidVertCount / 2; i++) {
        initTemplate->vertices[i] = KINETIC_PROJECTILE_VERTS[i];
    }
    //Flip to draw the other half of the pyrimid
    for (int i = halfPyrimidVertCount; i < initTemplate->numVerts; i++) {
        initTemplate->vertices[i] = KINETIC_PROJECTILE_VERTS[i - halfPyrimidVertCount] * -1.0f;
    }
    
    //Fill in the element buffer as well
    for (int i = 0; i < initTemplate->numElems; i++) {
        initTemplate->elements[i] = (GLuint)i;
    }
    
}

void KineticWeaponManager::initializeFromTemplate() {
    generator->initializeFromTemplate(*initTemplate);
}


void KineticWeaponManager::doUpkeep() {
    this->generator->doUpkeep(); //Handle object upkeep
    processCollisions(); //
}
