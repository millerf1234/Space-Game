//  This is the implementation for the class 'Stage'
//  Stage.cpp
//
//  Rewritten to inherit from GameEntity on 2/19/2018
//  Created by Forrest Miller on 2/17/18.
//

#include "Stage.h"

int Stage::numberOfStages = 0;

Stage::Stage() : GameEntityManager() {
    //GameEntity's constructor will set everything to 0 or Nullptr
    this->initTemplate = new InitializationTemplate;
    this->generator = new Generator();
    this->hasGenerator = true;
    this->hasCollision = false;
    this->stageNumber = ++numberOfStages;
    
    //Have the stage initialize it's generator from it's template
    generateInitializationTemplate(); //Generate the initialization template for Stage
    initializeFromTemplate(); //Initialize Generator from the template
}

Stage::~Stage() {
    //Since I have stage's vertices and elements hardcoded into the headder, need to
    //delete the dynamic memory that was used to hold these values
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
    if (this->initTemplate != nullptr) {
        delete this->initTemplate;
        this->initTemplate = nullptr;
    }
//    if (this->generator->getInstanceCount() > 1) {
//        this->geneerator->removeInstance(this->)
//    }
    if (this->generator != nullptr) {
        delete this->generator;
        this->generator = nullptr;
    }
}

void Stage::doUpkeep() {
    generator->doUpkeep(); //Have generator update the
    Instance * tmp = (generator->getArrayOfInstances())[0]; //Since there really is only on stage being drawn at a time, the array of instances should only contain a single element (unless I change something later on down the line once I start worrying about implementing multiple stages)
    for (int i = 0; i < generator->getInstanceCount(); ++i) {
        tmp->zoom -= 0.0001f * (TIME_TICK_RATE / 0.01f); //-= 0.0003f seems like a good speed...
        if (tmp->zoom < 1.25f) {tmp->zoom = 1.25f;} //Put a limit on closeness of zooming in
    }
}

void Stage::drawInstances() {
    generator->drawInstances();
}

//Protected:
void Stage::initializeFromTemplate() {
    generator->initializeFromTemplate(*initTemplate);
    //Create a new instance of the stage within the generator
    generator->generateSingle();
}

void Stage::generateInitializationTemplate() {
    initTemplate->hasTexture = true; //Since the background will have a texture
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
    
    initTemplate->typeDataRequired = specializationType::STAGE;
    
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
}


//  OLDE STAGE CODE:
//
//Stage::Stage(int textureToLoadInParameterArray ) {
//    stageInitFormat = nullptr;
//    setStageInitFormat(textureToLoadInParameterArray); //Generate the stage init format
//    this->generator = new Generator;
//    this->generator->initializeFromTemplate(*(this->stageInitFormat));
//
//}
//
//Stage::~Stage() {
//    if (this->stageInitFormat != nullptr) {
//        delete this->stageInitFormat;
//        this->stageInitFormat = nullptr;
//    }
//    if (this->generator != nullptr) {
//        delete this->generator;
//        this->generator = nullptr;
//    }
//    if (this->stageInitFormat->vertices != nullptr) {
//        delete this->stageInitFormat->vertices;
//        this->stageInitFormat->vertices = nullptr;
//    }
//    if (this->stageInitFormat->elements != nullptr) {
//        delete this->stageInitFormat->elements;
//        this->stageInitFormat->elements = nullptr;
//    }
//}
//
//void Stage::setStageInitFormat(int textureToLoadInParameterArray) {
//
//    //Set texture filepath
//    this->stageInitFormat = new InitializationTemplate;
//   // stageInitFormat->textureFilePath = (char *) BACKGROUND_TEXTURE_IMAGES[textureToLoadInParameterArray];
//    //stageInitFormat->textureFilePath[FILEPATH_BUFFER] = backgroundTextFP.c_str();
//    stageInitFormat->textureFilePath = backgroundTextureFP;
//    stageInitFormat->vertShaderPath = BACKGROUND_VERT;
//    stageInitFormat->fragShaderPath = BACKGROUND_FRAG;
//
//    stageInitFormat->hasVert = true;
//    stageInitFormat->hasFrag = true;
//
//    stageInitFormat->vertAttribName = "position";
//    stageInitFormat->colAttribName = "colr";
//    stageInitFormat->texAttribName = "texCoord";
//
//    //Set format of vertex data
//    stageInitFormat->vert2col3tex2 = true;
//
//    //Since backgrounds will not be loading from a model, set them up to load directly from
//    //provided data
//    stageInitFormat->hasVertsAlreadyLoaded = true;
//    //Load vert data directly into struct to be loaded into generator
//    stageInitFormat->numVerts = STAGE_VERTS_SIZE;
//    stageInitFormat->vertices = new GLfloat[STAGE_VERTS_SIZE];
//    //Load the element data into the struct that will be used to initalize generator
//    stageInitFormat->numElems = STAGE_ELEMENTCOUNT;
//    stageInitFormat->elements = new GLuint[STAGE_ELEMENTCOUNT];
//
//    //Copy vertex data
//    for (int i = 0; i < stageInitFormat->numVerts; ++i) {
//       stageInitFormat->vertices[i] = STAGE_VERTS[i];
//    }
//    //Copy element data too
//    for (int i = 0; i < stageInitFormat->numElems; ++i) {
//        stageInitFormat->elements[i] = STAGE_ELEMENTS[i];
//    }
//
////    stageInitFormat->textureFilePath = NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD[textureToLoadInParameterArray];
////    stageInitFormat->vertShaderFilePath = BACKGROUND_VERT_SHADER;
////    stageInitFormat->fragShaderFilePath = BACKGROUND_FRAG_SHADER;
//
//}
//
//
//
////void Stage::drawInstance() { }
//
//
