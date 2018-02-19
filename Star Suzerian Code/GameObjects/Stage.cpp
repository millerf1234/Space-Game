//  This is the implementation for the class 'Stage'
//  Stage.cpp
//
//  Created by Forrest Miller on 2/17/18.
//

#include "Stage.h"


Stage::Stage(int textureToLoadInParameterArray ) {
    stageInitFormat = nullptr;
    setStageInitFormat(textureToLoadInParameterArray); //Generate the stage init format
    this->generator = new Generator;
    this->generator->initializeFromTemplate(*(this->stageInitFormat));
    
}

Stage::~Stage() {
    if (this->stageInitFormat != nullptr) {
        delete this->stageInitFormat;
        this->stageInitFormat = nullptr;
    }
    if (this->generator != nullptr) {
        delete this->generator;
        this->generator = nullptr;
    }
    if (this->stageInitFormat->vertices != nullptr) {
        delete this->stageInitFormat->vertices;
        this->stageInitFormat->vertices = nullptr;
    }
    if (this->stageInitFormat->elements != nullptr) {
        delete this->stageInitFormat->elements;
        this->stageInitFormat->elements = nullptr;
    }
}

void Stage::setStageInitFormat(int textureToLoadInParameterArray) {
    
    //Set texture filepath
    this->stageInitFormat = new InitializationTemplate;
   // stageInitFormat->textureFilePath = (char *) BACKGROUND_TEXTURE_IMAGES[textureToLoadInParameterArray];
    //stageInitFormat->textureFilePath[FILEPATH_BUFFER] = backgroundTextFP.c_str();
    stageInitFormat->textureFilePath = backgroundTextureFP;
    stageInitFormat->vertShaderPath = BACKGROUND_VERT_SHADER;
    stageInitFormat->fragShaderPath = BACKGROUND_FRAG_SHADER;
    
    stageInitFormat->hasVert = true;
    stageInitFormat->hasFrag = true;
    
    stageInitFormat->vertAttribName = "position";
    stageInitFormat->colAttribName = "colr";
    stageInitFormat->texAttribName = "texCoord";
    
    //Set format of vertex data
    stageInitFormat->vert2col3tex2 = true;
    
    //Since backgrounds will not be loading from a model, set them up to load directly from
    //provided data
    stageInitFormat->hasVertsAlreadyLoaded = true;
    //Load vert data directly into struct to be loaded into generator
    stageInitFormat->numVerts = STAGE_VERTS_SIZE;
    stageInitFormat->vertices = new GLfloat[STAGE_VERTS_SIZE];
    //Load the element data into the struct that will be used to initalize generator
    stageInitFormat->numElems = STAGE_ELEMENTCOUNT;
    stageInitFormat->elements = new GLuint[STAGE_ELEMENTCOUNT];
    
    //Copy vertex data
    for (int i = 0; i < stageInitFormat->numVerts; ++i) {
       stageInitFormat->vertices[i] = STAGE_VERTS[i];
    }
    //Copy element data too
    for (int i = 0; i < stageInitFormat->numElems; ++i) {
        stageInitFormat->elements[i] = STAGE_ELEMENTS[i];
    }
    
//    stageInitFormat->textureFilePath = NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD[textureToLoadInParameterArray];
//    stageInitFormat->vertShaderFilePath = BACKGROUND_VERT_SHADER;
//    stageInitFormat->fragShaderFilePath = BACKGROUND_FRAG_SHADER;
    
}



//void Stage::drawInstance() { }
    

