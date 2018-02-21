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
    this->specType = specializationType::PLAYER;
    
    //Set-up the initialization template
    this->generateInitializationTemplate();
    //Initialize the generator from the initialization template
    this->initializeFromTemplate();
    
}

PlayerManager::~PlayerManager() {
    if (this->initTemplate != nullptr) {
        delete this->initTemplate;
        this->initTemplate = nullptr;
    }
    if (this->generator != nullptr) {
        delete this->generator;
        this->generator = nullptr;
    }
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
}

void PlayerManager::generateInitializationTemplate() {
    initTemplate->vertShaderPath = PLAYERSHIP_BODY_VERT;
    initTemplate->hasVert = true;
    initTemplate->fragShaderPath = PLAYERSHIP_BODY_FRAG;
    initTemplate->hasFrag = true;
    
    initTemplate->vertAttribName = "position";
    initTemplate->normalAttribName = "norml";
    
    initTemplate->vert3norml3;
    
    /*
     initTemplate->textureFilePath = backgroundTextureFP;
     initTemplate->vertShaderPath = BACKGROUND_VERT;
     initTemplate->fragShaderPath = BACKGROUND_FRAG;
     
     initTemplate->hasVert = true;
     initTemplate->hasFrag = true;
     
     initTemplate->vertAttribName = "position";
     initTemplate->colAttribName = "colr";
     initTemplate->texAttribName = "texCoord";
     
     //Set format of vertex data
     initTemplate->vert2col3tex2 = true;
     
     //Since backgrounds will not be loading from a model, set them up to load directly from
     //provided data
     initTemplate->hasVertsAlreadyLoaded = true;
     //Load vert data directly into struct to be loaded into generator
     initTemplate->numVerts = STAGE_VERTS_SIZE;
     initTemplate->vertices = new GLfloat[STAGE_VERTS_SIZE];
     //Load the element data into the struct that will be used to initalize generator
     initTemplate->numElems = STAGE_ELEMENTCOUNT;
     initTemplate->elements = new GLuint[STAGE_ELEMENTCOUNT];
     
     //Copy vertex data
     for (int i = 0; i < initTemplate->numVerts; ++i) {
     initTemplate->vertices[i] = STAGE_VERTS[i];
     }
     //Copy element data too
     for (int i = 0; i < initTemplate->numElems; ++i) {
     initTemplate->elements[i] = STAGE_ELEMENTS[i];
     }
     
     */
}
