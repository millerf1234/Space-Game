 //  Implementation for the Generator class
//  This class should probably be called InstanceGenerator or InstanceFactory instead of just Generator. Oh well...
//  Actually this class should be like a bunch of different classes. Right now this contains pretty much all the filler
//  code that I haven't yet broken into classes. see header for more info
//  Generator.cpp
//  Created by Forrest Miller on 2/17/18.


#include "Generator.h"

static constexpr int WAS_DELETED = -1000; //Used to represent deleted instances
static constexpr int MAX_TRACKED_INSTANCES_LIST_SIZE = 20000; //Maximum limit on renderable objects per frame

static bool drawCollisionDetails = DRAW_COLLISION_DETAILS_ON_GAME_LAUNCH;

//Set static variable
int Generator::nextObjID = 1; //Starts out at 1

//-------------------
// Constructor and Destructor
//-------------------
Generator::Generator() {
    this->wasInitialized = false; //Set bool member to false
    this->vertices = nullptr;
    this->elements = nullptr;
    //Set all ints to 0
    this->vbo = 0;
    this->ebo = 0;
    this->numberOfVertices = 0;
    this->numberOfElements = 0;
    this->shaderArraySize = 0;
    this->textureArraySize = 0;
    //Set pointers to nullptr
    this->shaderArray = nullptr;
    this->textureArray = nullptr;
    this->modelLoader = nullptr;
    this->instances = nullptr;
    this->activeInstances = 0;
   // this->shouldGenerateEntities = false;
    
    //Set specialization type for this generator
    specialization  = specializationType::NOSPECIALIZATION; //No extra data by default
    
    //Set special uniform location variables to -1 (their default not_found code)
    pul.ulocRed = pul.ulocGreen = pul.ulocBlue = -1; //PlayerUniformLocaition.UniformLOCationRED
    pul.ulocRed_Line = pul.ulocGreen_Line = pul.ulocBlue_Line = -1;
    pul.ulocTime_Line = pul.ulocZoom_Line = pul.ulocXTrans_Line = pul.ulocYTrans_Line = pul.ulocZTrans_Line = pul.ulocThetaX_Line = pul.ulocThetaY_Line = pul.ulocThetaZ_Line = -1;
    pul.ulocTimeEngine = pul.ulocZoomEngine = pul.ulocXTransEngine = pul.ulocYTransEngine = pul.ulocZTransEngine = pul.ulocThetaXEngine = pul.ulocThetaYEngine = pul.ulocThetaZEngine = -1;
    pul.ulocTimeEngineSide = pul.ulocZoomEngineSide = pul.ulocXTransEngineSide = pul.ulocYTransEngineSide = pul.ulocZTransEngineSide = pul.ulocThetaXEngineSide = pul.ulocThetaYEngineSide = pul.ulocThetaZEngineSide = -1;
    pul.ulocEngineMainRedinate = pul.ulocEngineMainBlueify =pul.ulocEngineSideRedinate = pul.ulocEngineSideBlueify = -1; //Probably refactor these names eventually...
    pul.ulocPDamage = pul.ulocPHealthMax = -1;
    pul.ulocRoll = pul.ulocPlayerRoll_Line = pul.ulocPlayerRollEngineSide = -1;
    
    ulocStageAspectRatio = -1;
    
}
//Destructor
Generator::~Generator() { //Delete any memory being claimed by this generator
    if (this->vertices != nullptr) {
        delete [] this->vertices;
        this->vertices = nullptr;
    }
    if (this->elements != nullptr) {
        delete [] this->elements;
        this->elements = nullptr;
    }
    if (this->shaderArray != nullptr) {
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "DEBUG::Deleting " << shaderArraySize << " shaders from the ShaderArray.\n";
        }
        for (int i = 0; i < this->shaderArraySize; ++i) {
            if (shaderArray[i] != nullptr) {
                delete shaderArray[i];
                shaderArray[i] = nullptr;
            }
        }
        delete [] this->shaderArray;
        this->shaderArray = nullptr;
    }
    if (this->textureArray != nullptr) {
        for (int i = 0; i < this->textureArraySize; ++i) {
            if (textureArray[i] != nullptr) {
                delete textureArray[i];
                textureArray[i] = nullptr;
            }
        }
        delete [] this->textureArray;
        this->textureArray = nullptr;
    }
    if (this->modelLoader != nullptr) {
        delete this->modelLoader;
        this->modelLoader = nullptr;
    }
    
    if (this->activeInstances > 0 && this->instances != nullptr) {
        for (int i = 0; i < activeInstances; ++i) {
            if (instances[i] != nullptr) {
                //I moved it so instances now delete their own culBox if they have one
                //if (instances[i]->colBox != nullptr) {
                //    delete instances[i]->colBox;
                //    instances[i]->colBox = nullptr;
                //}
                delete instances[i];
                instances[i] = nullptr;
            }
            // if (instances[i].instanceExpansionData != nullptr) {
            //     delete instances[i].instanceExpansionData;
            //     instances[i].instanceExpansionData = nullptr;
            // }
        }
        delete [] instances; //Delete the array of instances
    }
}

void Generator::setDrawCollisionDetails(bool drawCollision) {
    drawCollisionDetails = drawCollision;
}
bool Generator::getDrawCollisionDetails() const {
    return drawCollisionDetails;
}

void Generator::initializeFromTemplate(const InitializationTemplate& t) {
    if (this->wasInitialized) {
        if (PRINT_DEBUG_MESSAGES || PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! Attempting to initialize from template a generator which has already been initialized!\n";
        }
        return;
    }
    this->wasInitialized = true;
    this->specialization = t.typeDataRequired;
    
    //Create a throw-away VAO (actually this VAO winds up getting used anyways) (see vaoWasSetExternally)
    GLuint tempVAO;
    glGenVertexArrays(1, &tempVAO);
    glBindVertexArray(tempVAO);
    
    //Create the Vbo and Ebo
    this->vbo = 0;
    glGenBuffers(1, &(this->vbo));
    
    this->ebo = 0;
    glGenBuffers(1, &(this->ebo));
    
    //Check to see if we need to load a model from a file or it our initialization template came prepackaged with model data
    if (t.hasVertsAlreadyLoaded) {
        this->modelLoader = nullptr; //Make sure vertex data is nullptr
        this->vertices = new GLfloat[t.numVerts];
        this->numberOfVertices = t.numVerts;
        this->elements = new GLuint[t.numElems];
        this->numberOfElements = t.numElems;
        //Load vertices
        for (int i = 0; i < t.numVerts; ++i) {
            this->vertices[i] = t.vertices[i];
            //this->stackVertices[i] = t.vertices[i]; //For debug
            // std::cout << vertices[i] << " "; //FOR DEBUG!
        }
        
        
        //For Now, handle kinetic as a special case of multiple objects being stored in the same vertices
        if (t.typeDataRequired == specializationType::WEAPON) {
            if (t.fragShaderPath == KINETIC_FRAG) { //I'm going to cheat for now and test part of the template to see if it matches what I expect to see for KINETIC
                //Load the remaining Kinetic models into verts
                int vertSizeAllKineticModels = KINETIC_PROJECTILE_VERSION6_VERTSSTART + KINETIC_PROJECTILE_VERSION6_VERTS_COUNT;
                std::vector<GLfloat> allKineticModels;
                allKineticModels.resize(vertSizeAllKineticModels);
                
                //Copy over the first model data that was already set
                for (int i = 0; i < t.numVerts; i++) {
                    
                    allKineticModels.assign(1, vertices[i]);
                    //std::cout << "\n\nf is " << vertices[i];
                }
                delete [] this->vertices; //Delete the old array of vertices
                //Append in the extra models to the data
                float pyramidShrinkFactor = 1.0 / PROJECTILE_SIZE; //Shrink all the additional models
                for (GLfloat f : KINETIC_PROJECTILE_VERSION1_VERTS) {
                    //std::cout << "\n\nf is " << f;
                    f *= pyramidShrinkFactor;
                    allKineticModels.push_back(f);
                }
                for (GLfloat f : KINETIC_PROJECTILE_VERSION2_VERTS) {
                    f *= pyramidShrinkFactor;
                    allKineticModels.push_back(f);
                }
                for (GLfloat f : KINETIC_PROJECTILE_VERSION3_VERTS) {
                    f *= pyramidShrinkFactor;
                    allKineticModels.push_back(f);
                }
                for (GLfloat f : KINETIC_PROJECTILE_VERSION4_VERTS) {
                    f *= pyramidShrinkFactor;
                    allKineticModels.push_back(f);
                }
                for (GLfloat f : KINETIC_PROJECTILE_VERSION5_VERTS) {
                    f *= pyramidShrinkFactor;
                    allKineticModels.push_back(f);
                }
                for (GLfloat f : KINETIC_PROJECTILE_VERSION6_VERTS) {
                    f *= pyramidShrinkFactor;
                    allKineticModels.push_back(f);
                }
                
                this->vertices = new GLfloat[allKineticModels.size()];
                //Copy the data into this generators array of vertices
                std::copy(allKineticModels.begin(), allKineticModels.end(), this->vertices);
                
                ////This code prints out model data
                //short counter = 0;
                //for (GLfloat f : allKineticModels) {
                //    std::cout << " " << f << ", ";
                //    counter++;
                //    if (counter % 3 == 0) {
                //        std::cout << "\n";
                //    }
                //}
            }
            
        }
        
        //Load Element array
        for (int i = 0; i < t.numElems; ++i) {
            this->elements[i] = t.elements[i];
            //this->stackElements[i] = t.elements[i]; //For debug
        }
    }
    else {  //Load from SimpleObjLoader
        //Data to load will depend on the vertex format:
        if (t.vert3 || t.vert3tex2 || t.vert3tex3 || t.vert3norml3 || t.vert3norml3tex2 || t.vert3norml3tex3) {
            //Load Elements from face data
            int faceCounter = 0; //This counter is used inside loop to extract data from objLoader
            this->modelLoader = new SimpleObjLoader((char *)t.modelFilePath.c_str()); //Create the objLoader object
            
            //Copy over elements first (NOTE: Elements are independent of vertex format)
            this->elements = new GLuint[modelLoader->model.faces*3*2];
            int numElementsToPrintForDebug = 9; //Part of debug messages even though it's outside the check for debug (need to do this so code inside will run)
            if (PRINT_DEBUG_MESSAGES) {
                std::cout << "DEBUG::Here are the first " << numElementsToPrintForDebug << " values that were loaded into Elements: ";
            }
            for (int i = 0; i < modelLoader->model.faces * 3; i += 3) {
                this->elements[i] = modelLoader->faces[faceCounter][0] - 1;
                this->elements[i+1] = modelLoader->faces[faceCounter][3] - 1;
                this->elements[i+2] = modelLoader->faces[faceCounter][6] - 1;
                ++faceCounter;
                //Print debug message giving first few Elements that were loaded
                if (PRINT_DEBUG_MESSAGES && i < numElementsToPrintForDebug) {
                    std::cout << "\nElements[" << i << " to " << i+2  << "] were set to " << elements[i] << " " << elements[i+1] << " " << elements[i+2];
                }
            }
    
            this->numberOfElements = modelLoader->model.faces * 3;
            
            //Load vertices
            if (t.vert3) { //This is easy, just copy the position data right over
                vertices = new GLfloat [modelLoader->model.positions*3];
                int vertPosCounter = 0;
                for (int i = 0; i < modelLoader->model.positions; ++i) {
                    vertices[vertPosCounter] = modelLoader->positions[i][0];
                    vertices[vertPosCounter + 1] = modelLoader->positions[i][1];
                    vertices[vertPosCounter + 2] = modelLoader->positions[i][2];
                    vertPosCounter += 3;
                }
                this->numberOfVertices = modelLoader->model.positions*3;
            }
            
            //NOTE ON vert3Normal3: This format does not work the way I thought it would. It looks like doing normals on faces with drawElements might not work at all. Might need to rethink how to send normal data to GPU?
            else if (t.vert3norml3) {
                if (PRINT_DEBUG_MESSAGES) {
                    std::cout << "DEBUG::\nOOPS! DON'T USE THIS FORMAT. Read\nhttp://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/ \nto learn about normal mapping.\n";
                }
                //    vertices = new GLfloat [(vertexData->model.positions + vertexData->model.normals) * 3];
                //    int vertPosCounter = 0;
                //    //for (int i = 0; i < vertexData->model.positions + vertexData->model.normals; ++i) {
                //    for (int i = 0; i < vertexData->model.positions; ++i) {
                //        vertices[vertPosCounter] = vertexData->positions[i][0];
                //        vertices[vertPosCounter + 1] = vertexData->positions[i][1];
                //        vertices[vertPosCounter + 2] = vertexData->positions[i][2];
                //        vertPosCounter += 6;
                //    }
                //    //Load normals into vertex data array
                //    vertPosCounter = 3;
                //    for (int i = 0; i < vertexData->model.normals; ++i) {
                //        vertices[vertPosCounter] = vertexData->normals[i][0];
                //        vertices[vertPosCounter + 1] = vertexData->normals[i][1];
                //        vertices[vertPosCounter + 2] = vertexData->normals[i][2];
                //        vertPosCounter += 6;
                //    }
            }
            else {
                if (PRINT_DEBUG_WARNING_MESSAGES || PRINT_DEBUG_MESSAGES) {
                    std::cout << "\nDEBUG::OOPS! That format not yet implemented in generator's objLoader code!\n";
                }
            }
        }
        else {
            if (PRINT_DEBUG_WARNING_MESSAGES || PRINT_DEBUG_MESSAGES) {
                std::cout << "\nDEBUG::OOPS! That format not yet implemented in generator's objLoader code";
                std::cout << std::endl;
            }
        }
        if (modelLoader->isLoaded) {
            std::cout << "                Model loaded successfully. Model triangle count: " << this->modelLoader->model.faces << std::endl;
        }
        else {
            std::cout << "                OPPS! There was an error loading this model!" << std::endl;
        }
    }
    
    //Might want to delete the objLoader once I have all of the data from it
    //this->numberOfVertices = (vertexData->model.positions + vertexData->model.normals) * 3;
    //this->numberOfElements = vertexData->model.faces;
    
    //Buffer the data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //std::cout << std::endl << "DEBUG:: sizeof(stackVertices) is: " << sizeof(stackVertices) << std::endl;
    
    //Note that sizeof() behaves differently between Stack and Heap memory. Since I am using dynamic memory,
    //I can't rely on sizeof() to accuratly give me the size of the full array. Thus I need to track the size seperatly
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices, vertices, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements, elements, GL_STREAM_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(stackVertices), stackVertices, GL_STATIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stackElements), stackElements, GL_STATIC_DRAW);
    
    //load shader
    this->shaderArraySize = 1;
    this->shaderArray = new ShaderWrapper* [1];
    this->shaderArray[0] = new ShaderWrapper;
    
    this->shaderArray[0]->attachVert((char *)t.vertShaderPath.c_str());
    this->shaderArray[0]->attachFrag((char *)t.fragShaderPath.c_str());
    
    this->shaderArray[0]->link();
    if (t.vert3) {
        this->shaderArray[0]->setVertexAttribName((char *)t.vertAttribName.c_str());
        this->shaderArray[0]->setVAOExternally(tempVAO);
        this->shaderArray[0]->specifyVertexLayout(ShaderWrapper::VERT3, vbo, ebo);
    }
    else if (t.vert3norml3) {
        this->shaderArray[0]->setVertexAttribName((char *)t.vertAttribName.c_str());
        this->shaderArray[0]->setNormalAttribName((char *)t.normalAttribName.c_str());
        this->shaderArray[0]->setVAOExternally(tempVAO);
        this->shaderArray[0]->specifyVertexLayout(ShaderWrapper::VERT3NORML3, vbo, ebo);
    }
    else if (t.vert2col3tex2) {
        this->shaderArray[0]->setVertexAttribName((char *)t.vertAttribName.c_str());
        this->shaderArray[0]->setColorAttribName((char *)t.colAttribName.c_str());
        this->shaderArray[0]->setTextureAttribName((char *)t.texAttribName.c_str());
        
        this->shaderArray[0]->setVAOExternally(tempVAO);
        
        
        //this->shaderArray[0]->specifyVertexLayout(ShaderWrapper::VERT2COLOR3TEXEL2, vbo, ebo);
        this->shaderArray[0]->specifyVertexLayout(ShaderWrapper::VERT2COLOR3TEXEL2, vbo, ebo);
    }
    //else if (other formats) {...}
    else {
        std::cout << "\n\nOOps this format hasn't been implemented yet in Generator!\n";
    }
    
    if (t.hasTexture) {
        //load texture data
        //tex = 0; //These 2 lines are done inside textureWrapper
        //glGenTextures(1, &tex);
        this->textureArraySize = 1; //1 texture for now...
        this->textureArray = new TextureWrapper * [textureArraySize];
        this->textureArray[0] = new TextureWrapper((char *)backgroundTextureFP.c_str());
        //this->tex = //Not needed?
    }
    GLuint shaderID = -1; //Declare this outside the for loop so I can reuse it later
    
    //Set uniform locations in the shader program  (NOTE THAT OPENGL WILL NOT GET LOCATION FOR UNUSED UNIFORMS!!!)
    for (int i = 0; i < this->shaderArraySize; ++i) { //I am assuming all shaders will have these uniforms in common (it's fine it they don't, get uniform location will just return -1 for invalid location)
        shaderID = shaderArray[i]->getID();
        ulocTime = glGetUniformLocation(shaderID, "time");
        ulocZoom = glGetUniformLocation(shaderID, "zoom");
        ulocXTrans = glGetUniformLocation(shaderID, "xTrans");
        ulocYTrans = glGetUniformLocation(shaderID, "yTrans");
        ulocZTrans = glGetUniformLocation(shaderID, "zTrans");
        ulocThetaX = glGetUniformLocation(shaderID, "thetaX");
        ulocThetaY = glGetUniformLocation(shaderID, "thetaY");
        ulocThetaZ = glGetUniformLocation(shaderID, "thetaZ");
    }
    
    //Set type-specific uniform location
    if (this->specialization == STAGE) {
        //Pretend I am printing out this next message earlier in the code (it won't matter)
        std::cout << "            Loading Stage Model...\n";
        std::cout << "                Model Loaded Succcessfully. Model triangle count: 2" << std::endl;
        std::cout << "            Loading Stage Shader...\n";
        
        //Set the one uniform location for stage
        ulocStageAspectRatio = glGetUniformLocation(shaderID, "aspectRatio");
        //std::cout << "\nASPECT RATIO UNIFORM LOCATION SET TO: " << ulocStageAspectRatio << "\n";  //Print a message to make sure a uniform location is actually being grabbed
        std::cout << "                Stage Shader Loaded...\n";
    }
    else if (this->specialization == PLAYER) {
        std::cout << "            Loading Player Shaders..." << std::endl;
        std::cout << "                Creating Body shader...\n";
        //Set the uniform locations for PLAYER
        pul.ulocRed = glGetUniformLocation(shaderID, "red");
        pul.ulocGreen = glGetUniformLocation(shaderID, "green");
        pul.ulocBlue = glGetUniformLocation(shaderID, "blue");
        pul.ulocPDamage = glGetUniformLocation(shaderID, "damage");
        pul.ulocPHealthMax = glGetUniformLocation(shaderID, "maxHealth");
        pul.ulocRoll = glGetUniformLocation(shaderID, "earlyThetaZ");
        
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG:\n        ulocRed = " << pul.ulocRed;
            std::cout << "\n        ulocGreen = " << pul.ulocGreen;
            std::cout << "\n        ulocBlue = " << pul.ulocBlue;
            std::cout << "\n        ulocPDamage = " << pul.ulocPDamage;
            std::cout << "\n        ulocPHealthMax = " << pul.ulocPHealthMax;
        }
        //(I realize what I am doing here in this next bit of code is very sloppy, inadvisiable and may cause many headaches down the road. Sorry in advance, I really wanna just get something drawing to the screen)
        std::cout << "                    Body shader ready!\n";
        
        
        //Add extra shaders for the engine effects to ShaderArray
        const int INDEX_OF_PLAYER_LINE_SHADER = 1;
        const int INDEX_OF_PLAYER_MAIN_ENGINE_SHADER = 2;
        const int INDEX_OF_PLAYER_SIDE_ENGINE_SHADER = 3;
        this->shaderArraySize = 4;
        ShaderWrapper ** temp = new ShaderWrapper * [shaderArraySize];
        temp[0] = this->shaderArray[0]; //ShaderArray was set to size 1, so need to make it bigger
        delete [] this->shaderArray;
        this->shaderArray = temp;
        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER] = new ShaderWrapper;
        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER] = new ShaderWrapper;
        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER] = new ShaderWrapper;

        //Set up the Line Shader:
        std::cout << "                Creating Outline shader...\n";
//        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER] = new ShaderWrapper;
        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->attachVert((char *)PLAYERSHIP_LINE_VERT.c_str());
        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->attachFrag((char *)PLAYERSHIP_LINE_FRAG.c_str());
        
        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->link();
        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->setVertexAttribName((char *)t.vertAttribName.c_str()); //Use same name as body shader name
        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->setVAOExternally(tempVAO); //this way the shaders share the same VAO
        this->shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->specifyVertexLayout(ShaderWrapper::VERT3, vbo, ebo);
        //set uniforms for the line shader:
        shaderID = shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->getID();
        pul.ulocRed_Line = glGetUniformLocation(shaderID, "red");
        pul.ulocGreen_Line = glGetUniformLocation(shaderID, "green");
        pul.ulocBlue_Line = glGetUniformLocation(shaderID, "blue");
        pul.ulocTime_Line = glGetUniformLocation(shaderID, "time");
        pul.ulocZoom_Line = glGetUniformLocation(shaderID, "zoom");
        pul.ulocXTrans_Line = glGetUniformLocation(shaderID, "xTrans");
        pul.ulocYTrans_Line = glGetUniformLocation(shaderID, "yTrans");
        pul.ulocZTrans_Line = glGetUniformLocation(shaderID, "zTrans");
        pul.ulocThetaX_Line = glGetUniformLocation(shaderID, "thetaX");
        pul.ulocThetaY_Line = glGetUniformLocation(shaderID, "thetaY");
        pul.ulocThetaZ_Line = glGetUniformLocation(shaderID, "thetaZ");
        pul.ulocPlayerRoll_Line = glGetUniformLocation(shaderID, "earlyThetaZ");
        
        if (PRINT_DEBUG_MESSAGES) {
        std::cout << "    DEBUG::PlayerLineColorLocations: \n";
        std::cout << "ulocRedLine = " << pul.ulocRed_Line << std::endl;
        std::cout << "ulocGreenLine = " << pul.ulocGreen_Line << std::endl;
        std::cout << "ulocBlueLine = " << pul.ulocBlue_Line << std::endl;
        }
        
        if (shaderArray[INDEX_OF_PLAYER_LINE_SHADER]->checkIfReady()) {
            std::cout << "                    Outline shader ready!\n";
        }
        else {
            std::cout << "                    OOPS! For some reason Outline shader is showing not ready...\n";
        }
        ///Set up the main engine shader
//        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER] = new ShaderWrapper;
        std::cout << "                Creating Main Engine shader...\n";
        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->attachVert((char *)PLAYERSHIP_ENGINE_VERT.c_str());
        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->attachFrag((char *)PLAYERSHIP_ENGINE_FRAG.c_str());
        
        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->link();
        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->setVertexAttribName((char *) "enginePos");
        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->setVAOExternally(tempVAO);
        this->shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->specifyVertexLayout(ShaderWrapper::VERT3, vbo, ebo);
        //Set uniforms for the main engine
        shaderID = shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->getID();
        pul.ulocTimeEngine = glGetUniformLocation(shaderID, "time");
        pul.ulocZoomEngine = glGetUniformLocation(shaderID, "zoom");
        pul.ulocXTransEngine = glGetUniformLocation(shaderID, "xTrans");
        pul.ulocYTransEngine = glGetUniformLocation(shaderID, "yTrans");
        pul.ulocZTransEngine = glGetUniformLocation(shaderID, "zTrans");
        pul.ulocThetaXEngine = glGetUniformLocation(shaderID, "thetaX");
        pul.ulocThetaYEngine = glGetUniformLocation(shaderID, "thetaY");
        pul.ulocThetaZEngine = glGetUniformLocation(shaderID, "thetaZ");
        //no ulocPlayerRoll = glGetUniformLocation(shaderID, "earlyThetaZ"); //Because main engine is symmetric about roll axis
        pul.ulocEngineMainRedinate = glGetUniformLocation(shaderID, "redinate");
        pul.ulocEngineMainBlueify = glGetUniformLocation(shaderID, "blueify");
        
        if (shaderArray[INDEX_OF_PLAYER_MAIN_ENGINE_SHADER]->checkIfReady()) {
            std::cout << "                    Main Engine shader ready!\n";
        }
        else {
            std::cout << "                    OOPS! For some reason Main Engine shader is showing not ready...\n";
        }
        
        ///Set up the side engine shader
        std::cout << "                Creating Side Engine shader...\n";
        //        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER] = new ShaderWrapper;
        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->attachVert((char *)PLAYERSHIP_ENGINE_VERT.c_str());
        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->attachFrag((char *)PLAYERSHIP_SIDE_ENGINE_FRAG.c_str());
        
        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->link();
        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->setVertexAttribName((char *) "enginePos");
        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->setVAOExternally(tempVAO);
        this->shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->specifyVertexLayout(ShaderWrapper::VERT3, vbo, ebo);
        shaderID = shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->getID();
        pul.ulocTimeEngineSide = glGetUniformLocation(shaderID, "time");
        pul.ulocZoomEngineSide = glGetUniformLocation(shaderID, "zoom");
        pul.ulocXTransEngineSide = glGetUniformLocation(shaderID, "xTrans");
        pul.ulocYTransEngineSide = glGetUniformLocation(shaderID, "yTrans");
        pul.ulocZTransEngineSide = glGetUniformLocation(shaderID, "zTrans");
        pul.ulocThetaXEngineSide = glGetUniformLocation(shaderID, "thetaX");
        pul.ulocThetaYEngineSide = glGetUniformLocation(shaderID, "thetaY");
        pul.ulocThetaZEngineSide = glGetUniformLocation(shaderID, "thetaZ");
        pul.ulocPlayerRollEngineSide = glGetUniformLocation(shaderID, "earlyThetaZ");
        
        pul.ulocEngineSideRedinate = glGetUniformLocation(shaderID, "redinate");
        pul.ulocEngineSideBlueify = glGetUniformLocation(shaderID, "blueify");
        //std::cout << "\nFound Blueify at location: " << pul.ulocEngineSideBlueify << std::endl;
        
        if (shaderArray[INDEX_OF_PLAYER_SIDE_ENGINE_SHADER]->checkIfReady()) {
            std::cout << "                    Side Engine shader ready!\n";
        }
        else {
            std::cout << "                    OOPS! For some reason Side Engine shader is showing not ready...\n";
        }
        
        //Check all shaders to see if ready
        bool allShadersGood = true;
        for (int i = 0; i < this->shaderArraySize; ++i) {
            if (!shaderArray[i]->checkIfReady()) {
                allShadersGood = false;
            }
        }
        
        if (allShadersGood) {
            std::cout << "            All shaders for Player finished building!\n";
            std::cout << "        Player finished loading... \n";
        }
        else {
            std::cout << "            OOPS! One or more Shaders for player did not load correctly!" << std::endl;
            std::cout << "        Player failed to load completely... \n";
        }
        
        //std::cout << "        Player finished loading... \n";
    }
    
    else if (this->specialization == WEAPON) {
        //Set the uniform locations for WEAPON
        
        //No special uniforms currently for kinetic...
    }
   
     //GLint ulocTime, ulocZoom, ulocXTrans, ulocYTrans, ulocZTrans, ulocThetaX, ulocThetaY, ulocThethaZ;
    glBindVertexArray(0);
    
}

void Generator::doUpkeep() {
    if (instances == nullptr) {return;}
    for (int i = 0; i < activeInstances; ++i) {
        //instances[i]->timeAlive += TIME_TICK_RATE; //I moved this to it's own function so all objects have their age incremented together
        bool isPlayerInstance = (instances[i]->type == InstanceType::PLAYERENTITY); //Set a bool variable since I need to do this check multiple times
        
        if (isPlayerInstance) { //Do additional checking on velocity if instance is a playerInstance
            aiVector2D tempForMaxSpeedCheck(instances[i]->velocity.x, instances[i]->velocity.y);
            //Make it so the player's max movement speed doesn't exceed PLAYER_MOVEMENT_MAX_SPEED
            if (tempForMaxSpeedCheck.Length() > PLAYER_MOVEMENT_MAX_SPEED) { //If length of velocity vector is greater than max
                tempForMaxSpeedCheck.Normalize(); //Set the vector to length 1
                tempForMaxSpeedCheck.operator*=((PLAYER_MOVEMENT_MAX_SPEED) * (TIME_TICK_RATE / 0.01f)); //Scale length to max
                instances[i]->velocity.x = tempForMaxSpeedCheck.x;
                instances[i]->velocity.y = tempForMaxSpeedCheck.y;
            }
        }
        
        //Update every instances position with it's velocity
        instances[i]->position.x += instances[i]->velocity.x;
        instances[i]->position.y += instances[i]->velocity.y;
        
        //Keep player instances inbounds
        if (isPlayerInstance) {
            //Check to see if out of bounds, and if so, then move back inbounds
            if (instances[i]->position.x > XLIMIT) {
                instances[i]->position.x = XLIMIT;
                instances[i]->velocity.x = 0.0f;//Set velocity in that direction to 0
            }
            else if (instances[i]->position.x < -XLIMIT) {
                instances[i]->position.x = -XLIMIT;
                instances[i]->velocity.x = 0.0f;
            }
            //Check Y boundary as well
            if (instances[i]->position.y > YLIMIT) {
                instances[i]->position.y = YLIMIT;
                instances[i]->velocity.y = 0.0f;
            }
            else if (instances[i]->position.y < -YLIMIT) {
                instances[i]->position.y = -YLIMIT;
                instances[i]->velocity.y = 0.0f;
            }
        }
        
    }
}



void Generator::generateSingle() {
    if (!this->readyToGenerate()) {
        std::cout << "\nDEBUG::Oops! It looks like you are not yet ready to generate objects of this type!\n";
        return;
    }
    ++activeInstances;
    if (this->instances == nullptr) { //If this is the first instance
        this->instances = new Instance*[activeInstances];
    }
    else { //Else Need to make a new array of instances that's 1 larger than before
        Instance ** temp = new Instance*[activeInstances];
        for (int i = 0; i < activeInstances - 1; ++i) {
            temp[i] = (this->instances[i]);
        }
        delete [] this->instances;
        this->instances = temp; //Set instances to temp
    }
    
    int newInstanceIndex = activeInstances - 1;
    //Create an instance based off generator type (note that special types will have their values set correctly by their constructors) (at least in theory they will)
    if (specialization == specializationType::PLAYER) {//Generate a player instance
        instances[newInstanceIndex] = new PlayerEntity(nextObjID);
        instances[newInstanceIndex]->type = PLAYERENTITY;
        instances[newInstanceIndex]->identifierNumber = (nextObjID); //Not sure why this isn;t getting set...or is it...
        //Since the instance is a PLAYER instance, create the collisionBox for the instance
        //instances[newInstanceIndex]->colBox =  new CollisionRectangle(this->vertices, this->numberOfVertices);
        instances[newInstanceIndex]->colBox = new CollisionBox(this->vertices, this->numberOfVertices);
        instances[newInstanceIndex]->colBox->setScale(1.0f/(GAME_SCALE));
        //I will put the colBox rotations into place later, once they are actually set
//        //Put the Player specific rotation order into place within CollisionRectangle
//        instances[newInstanceIndex]->colBox->addToRotationOrder()
//        instances[newInstanceIndex]->colBox->
//        instances[newInstanceIndex]->colBox->
        
        ++nextObjID;
    }
    else if (specialization == specializationType::WEAPON) {//Generate a weapon instance
        
        //I need to have some way for Generator to know right here what specific type of weapon instance it is generating. Going to just assume it is a kinetic for now because Kinetic is the only type I have working...
        instances[newInstanceIndex] = new Kinetic(nextObjID);
        Instance * newInstance = instances[newInstanceIndex];
        
        /*
        //I am going to have the kinetic projectiles have a random spawn angle
        //Method 1  (doesn't really work, gives the same number each iteraion)
        //std::default_random_engine randGen;
        //std::uniform_real_distribution<float> distribution(0.0, 2.0 * PI);
        //instances[newInstanceIndex]->thetaZ = distribution(randGen);
        //instances[newInstanceIndex]->thetaX = distribution(randGen);
        //instances[newInstanceIndex]->thetaY = distribution(randGen);
        
        //Method 2 (Doesn't really Work, and random_device might not be available on every platform)
        std::uniform_real_distribution<float> distribution(0.0f, 2.0f * (PI) );
        std::random_device rd; //see: https://stackoverflow.com/questions/15461140/stddefault-random-engine-generate-values-between-0-0-and-1-0?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
        std::default_random_engine generator( rd() );
        instances[newInstanceIndex]->thetaZ = distribution(generator);
        instances[newInstanceIndex]->thetaX = distribution(generator);
        instances[newInstanceIndex]->thetaY = distribution(generator);
        */
        
        /*
        ////Method 3 (That actually works)
        newInstance->thetaZ = MathFunc::getRandomInRange(0.0f, PI);
        newInstance->thetaX = MathFunc::getRandomInRange(0.0f, PI);
        newInstance->thetaY = MathFunc::getRandomInRange(0.0f, PI);
        */
        
        ///Method 4 (No random spawn angle)
        newInstance->thetaZ = newInstance->thetaX = newInstance->thetaY = 0.0f;
        
        
        if (PRINT_WEAPON_SPAWN_ANGLES) {
            std::cout << "thetaZ is " << newInstance->thetaZ <<
            " and thetaX is " << newInstance->thetaX << " and thetaY is  " << newInstance->thetaY << std::endl;
        }
        
        //I have the weaponManagers do special configuration themselves off the new value generated by generator...
        
        ////instances[newInstanceIndex] = new Instance(nextObjID);
        ////instances[newInstanceIndex]->type = WEAPONINSTANCE;
        ////Instead of those 2 lines above, do this
        //instances[newInstanceIndex] = new WeaponInstance(nextObjID);
        //instances[newInstanceIndex]->identifierNumber = (nextObjID); //This should be set inside the constructor already
        
        
        
        
        //Give it a colBox, right off the bat. The collisionBox will be managed by the weapon managers though later...
        
        //Shrink the vertices slightly
        float verticeShrinkFactor = 0.75f;
        GLfloat tempSmallerVerticesForCollisionBox [this->numberOfVertices];
        for (int i = 0; i < this->numberOfVertices; ++i) {
            tempSmallerVerticesForCollisionBox[i] = verticeShrinkFactor * this->vertices[i];
        }
      //  instances[newInstanceIndex]->colBox = new CollisionBox(this->vertices, this->numberOfVertices);
        instances[newInstanceIndex]->colBox = new CollisionBox(tempSmallerVerticesForCollisionBox, this->numberOfVertices);
        //instances[newInstanceIndex]->colBox->setScale(1.0f/(PLAYER_SIZE));
        
        nextObjID++;
    }
    else if (specialization == specializationType::STAGE) {
        instances[newInstanceIndex] = new Instance;
        instances[newInstanceIndex]->identifierNumber = nextObjID++;
    }
    else if (specialization == specializationType::NOSPECIALIZATION) {
        instances[newInstanceIndex] = new Instance; //Just a generic instance
        instances[newInstanceIndex]->identifierNumber = nextObjID++;
    }
    
    else {
        std::cout << "\nDEBUG::ERROR!! Attempting to generate a new type of instance that wasn't\nrecognized by the generator when function \"generateSingle\" was called!" << std::endl;
        return;
    }
    //Keep track of all identifier numbers that are in use and were created by this generator
    instancesCreatedByThisGenerator.push_back(instances[newInstanceIndex]->identifierNumber);
    
    //Set up the rest of the new instance (this code executes for all instances no matter their type)
    this->instances[newInstanceIndex]->position = aiVector3D(0.0f, 0.0f, 0.0f);
    this->instances[newInstanceIndex]->zoom = 3.5f; //Higher zoom means farther from camera
    this->instances[newInstanceIndex]->timeAlive = 0.0f;
    this->instances[newInstanceIndex]->thetaX = 0.0f;
    this->instances[newInstanceIndex]->thetaZ = 0.0f;
    this->instances[newInstanceIndex]->thetaY = 0.0f;
    
}

void Generator::drawInstances() {
    if (!wasInitialized || this->activeInstances == 0) {return;} //Make sure we can actually draw
    

    
   // glGetActiveAttrib;
    //this->textureArray[0]->tex[0] = 0; //This might fix it? No, I had 2 different tex's
   // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureArray[0]->tex[0], 0); //Might need to do this?
    
    
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
//    GLfloat tempVerts[2000];
//    GLfloat tempElements[2000];

//    for (int i = 0; i < numberOfVertices; ++i) {
//        tempVerts[i] = vertices[i];
//    }
//    for (int i = 0; i < numberOfElements; ++i) {
//        tempElements[i] = elements[i];
//    }
//
//    glBufferData(GL_ARRAY_BUFFER, sizeof(tempVerts), tempVerts, GL_STREAM_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tempElements), tempElements, GL_STREAM_DRAW);

    
    //glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices + 100, this->vertices, GL_STATIC_DRAW);
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements + 100, elements, GL_STATIC_DRAW);
    
    for (int i = 0; i < activeInstances; ++i) {
        this->shaderArray[0]->use(); //By default, activate the first shader effect
        //If the instance has a texture, then activate the texture
        if (this->hasTexture()){
            this->textureArray[0]->activate();
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
       // if (specialization != STAGE) {
        ///This glBufferData call currently does not set the size correctly. It should be set off the number of positions (for the array buffer), not the number of vertices. Or maybe it should. Investigate this if ever get time...
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices, this->vertices, GL_STREAM_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements*2, this->elements, GL_STREAM_DRAW);
       // }
        //else { //Do static_draw with stage because it's data will not be updated frequently (at all) //Actually I thought about it more and this is not true/shouldn't matter
       //     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices*2, this->vertices, GL_STATIC_DRAW);
      //      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements*2, this->elements, GL_STATIC_DRAW);
       // }
        
        //Set Uniforms for this instance
        glUniform1f(ulocTime, instances[i]->timeAlive);
        glUniform1f(ulocZoom, instances[i]->zoom);
        glUniform1f(ulocXTrans, instances[i]->position.x);
        glUniform1f(ulocYTrans, instances[i]->position.y);
        glUniform1f(ulocZTrans, instances[i]->position.z);
        glUniform1f(ulocThetaX, instances[i]->thetaX);
        glUniform1f(ulocThetaY, instances[i]->thetaY);
        glUniform1f(ulocThetaZ, instances[i]->thetaZ);
        
        //If being asked to draw a player, then draw PLAYER
        if (specialization == specializationType::PLAYER) {
            //Do extra stuff required for drawing the player models
            doDrawPlayerShipInstance(i);
        }
        else if (specialization == specializationType::WEAPON) {
            //Do the weapon drawing stuff
            //I am going to break the way I had been doing things to draw all the weapons at the same time
            doDrawWeaponInstance(i);
        }
        else if (specialization == specializationType::STAGE) {
            glUniform1f(ulocStageAspectRatio, STAGE_ASPECT_RATIO);
            if (STAGE_POSITION_CENTER_AND_FULLSCREEN_IMAGE) {
                float zoomAmount = 1.175f - 0.0005f * instances[i]->timeAlive;
                zoomAmount = (zoomAmount > 1.05f ? zoomAmount : 1.05f);
                glUniform1f(ulocZoom, zoomAmount);
                glUniform1f(ulocXTrans, -0.5f);
            }
            glDrawElements(GL_TRIANGLES, this->numberOfElements, GL_UNSIGNED_INT, 0);
        }
        
        //Generic draw for unspecified instances (in theory this code will never run if I did everything right)
        else if (specialization == specializationType::NOSPECIALIZATION) {
            if (PRINT_DEBUG_MESSAGES || PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::Draw command called for an instance with no specialization type! Instance ID: " << instances[i]->getID() << std::endl;
            }
            //if (drawTriangles) {
                glDrawElements(GL_TRIANGLES, this->numberOfElements, GL_UNSIGNED_INT, 0);
           // }
           // else if (drawLines) {
           //     glDrawElements(GL_LINES, this->numberOfElements * 2, GL_UNSIGNED_INT, 0);
           // }
           // else {
           //     glDrawElements(GL_POINTS, this->numberOfElements, GL_UNSIGNED_INT, 0);
           // }
        }
        else {
            if (PRINT_DEBUG_WARNING_MESSAGES || PRINT_DEBUG_MESSAGES) {
                std::cout << "\nDEBUG::ERROR(sorta)! Generator's drawInstances command was called with an unrecognized (or just\ncurrently unimplemented) type! Please implement this type or fix error!\n\n";
            }
        }
    }
    //It's generally a good idea to set these next two things to 0 after having used them
    glBindVertexArray(0); //Vertex attribute array
    glUseProgram(0);
}

//Note that this function will not be efficient if called in a loop to delete a bunch of instances
//Thus particle effects have their own creation/deletion logic
void Generator::removeInstance(const int & instanceID) {
    //Check to make sure instanceID matches an instance generated by this generator
    std::vector<int>::iterator iter;
    iter = find (instancesCreatedByThisGenerator.begin(), instancesCreatedByThisGenerator.end(), instanceID);
    if (iter == instancesCreatedByThisGenerator.end()) { //If iterator reached the end
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\n    DEBUG::OOPS! removeInstance called for instance with id # ";
            std::cout << instanceID << " but that instance was not found\nwithin this generator!";
            std::cout << std::endl;
        }
        return;
    }
    else { //Else that instance exists in the instancesCreatedByThisGenerator list, so delete it from the list
        (*iter) = WAS_DELETED;
        
        //Check to see how big the list of instanceID's that are being tracked is
        //see http://www.cplusplus.com/reference/iterator/distance/
         std::vector<int>::iterator front = instancesCreatedByThisGenerator.begin();
          std::vector<int>::iterator rear = instancesCreatedByThisGenerator.end();
        
        //for debug:
        //std::cout << "\nThe number of instance ID's being tracked by this generator: " << std::distance(front, rear) << std::endl;
        if (std::distance(front, rear) >= MAX_TRACKED_INSTANCES_LIST_SIZE) {
            //Delete all the extra dead instances from the vector
            instancesCreatedByThisGenerator.erase(std::remove(front, rear, WAS_DELETED), rear);
            //std::cout << "Vector size after deleting: ";
            front = instancesCreatedByThisGenerator.begin();
            rear = instancesCreatedByThisGenerator.end();
            std::cout << std::distance(front, rear) << std::endl;
        }
    }
    
    if (this->activeInstances == 1) { //If there is just a single instance
        this->activeInstances = 0;
        if (instances[0] != nullptr) {
            delete instances[0];
            instances[0] = nullptr;
        }
        delete [] instances;
        instances = nullptr;
        return;
    }
    //else the instance is located in the array somewhere besides the end (and there are multiple instances)
    bool found = false; //Need 'found' to keep array indexes lined up
    Instance ** temp = new Instance*[activeInstances - 1];
    for (int i = 0; i < activeInstances; ++i) {
        if (found) {
            if (i == activeInstances - 1) {continue;}
            temp[i] = instances[i+1];
        }
        //else if (instances[i] != nullptr && instances[i]->getID() == instantID)
        else if (instances[i]->getID() == instanceID) {
            found = true;
            delete instances[i];
            
            //Check to see if i is not the last instance
            if (i != activeInstances - 1) {
                temp[i] = instances[i+1]; //Shift the next instance over if i is not last instance
            }
            //Debug message:
            if (PRINT_DEBUG_MESSAGES ) { //I have a feeling my algorithm here could be wrong, so print out messages anyways
                std::cout << "\nDEBUG::Deleted instance #" << instanceID << " at position " << i << std::endl;
            }
        }
        else {
            temp[i] = instances[i];
        }
        
       
        
//        if (instances[i] != nullptr && instances[i]->getID() == instantID) {
//            found = true;
//            //delete the instance that is being removed:
//            delete instances[i];
//            instances[i] = nullptr;
//            //DEBUG:
//            if (PRINT_DEBUG_MESSAGES || true) { //I have a feeling my algorithm here could be wrong, so print out messages anyways
//                std::cout << "\nDEBUG::Deleted instance #" << instantID << std::endl;
//            }
//        }
//        else if (found) { //If we already deleted the instance, then just move the rest of the array over
//            temp[i] = instances[i+1];
//        }
//        else {
//            temp[i] = instances[i];
//        }
    }
    --activeInstances;
    delete [] this->instances; //delete the old instance array
    this->instances = temp; //set instance array to the new array
}

void Generator::removeInstance(Instance * instPtr) {
    removeInstance(instPtr->getID());
}


//bool Generator::hasVbo() {
//    return (this->vbo != 0 ); //No, remove these two checks for now...
//}
//bool Generator::hasEbo() {
//    return (this->ebo != nullptr);
//}
bool Generator::hasVerts() {
    return (this->vertices != nullptr);
}
bool Generator::hasElems() {
    return (this->elements != nullptr);
}
bool Generator::hasShader() {
    return (this->shaderArray != nullptr);
}
bool Generator::hasTexture(){
    return (this->textureArray != nullptr);
}
bool Generator::hasModel() {
    return (this->numberOfVertices != 0);
}
bool Generator::readyToGenerate() {
    return (hasShader() && hasModel());
}

void Generator::doDrawPlayerShipInstance(int i) {
    //Currently I am drawing players using 4 passes, the first is using triangles to draw the playership's body,
    //the second is to draw the lines along each triangle in the body, and the final 2 are for the engine flames.
    //All of the code for the engine flames (aside from the translationHistory array managment) is contained within
    //this function.
    
    PlayerEntity * player = static_cast<PlayerEntity*>(instances[i]);
    if (player->isDead) {
        return; //Don't draw anything
    }
    
    //Get playerBody specific uniforms (see shader code for what uniforms are in use for each program)
    glUniform1f(pul.ulocRed, player->red);
    glUniform1f(pul.ulocGreen, player->green);
    glUniform1f(pul.ulocBlue, player->blue);
    glUniform1f(pul.ulocRoll, player->rollAmount);
    glUniform1f(pul.ulocPHealthMax, STARTING_PLAYER_HEALTH);
    glUniform1f(pul.ulocPDamage, STARTING_PLAYER_HEALTH - player->health); //Subtract actual player health from expected health to get damage
    
    //Draw the player body (using shader in shadderArray[0]
    if (DRAW_MODELS) {
        glDrawElements(GL_TRIANGLES, this->numberOfElements, GL_UNSIGNED_INT, 0);
    }
    
    //Set up to Draw Lines
    shaderArray[1]->use();
    glUniform1f(pul.ulocTime_Line, instances[i]->timeAlive);
    glUniform1f(pul.ulocZoom_Line, instances[i]->zoom);
    glUniform1f(pul.ulocXTrans_Line, instances[i]->position.x);
    glUniform1f(pul.ulocYTrans_Line, instances[i]->position.y);
    glUniform1f(pul.ulocZTrans_Line, instances[i]->position.z);
    glUniform1f(pul.ulocThetaX_Line, instances[i]->thetaX);
    glUniform1f(pul.ulocThetaY_Line, instances[i]->thetaY);
    glUniform1f(pul.ulocThetaZ_Line, instances[i]->thetaZ);
    glUniform1f(pul.ulocRed_Line, player->red * PLAYER_LINE_COLOR_BOOST_FACTOR);
    glUniform1f(pul.ulocGreen_Line, player->green * PLAYER_LINE_COLOR_BOOST_FACTOR);
    glUniform1f(pul.ulocBlue_Line, player->blue * PLAYER_LINE_COLOR_BOOST_FACTOR);
    glUniform1f(pul.ulocPlayerRoll_Line, player->rollAmount);
    
    //Some DEBUG code------------------------------
    if (PRINT_DEBUG_MESSAGES) {
    bool printPlayerUniformValuesForDebug = false; //Don't print messages, I finished debugging
    if (printPlayerUniformValuesForDebug) {
        std::cout << "\nUniform Values for player " << player->playerNumber << ":\n";
        std::cout << "  Zoom  = " << player->zoom << "\n  Position x,y,z = " << player->position.x << ", " << player->position.y << ", " << player->position.z << std::endl;
        std::cout << "  Roll amount = " << player->rollAmount << std::endl;
        std::cout << "  thetaX = " << player->thetaX << "\n  thetaY = " << player->thetaY << "\n  thetaZ = " << player->thetaZ;
        std::cout << std::endl;
    }
    
        ////I could never get these collisions to work well... Skip past this commented out code
//    if (PRINT_DEBUG_MESSAGES) {
//        bool playersColliding = false;
//        for (int otherPlayerIndx = 0; otherPlayerIndx < this->getInstanceCount(); ++otherPlayerIndx) {
//            //This current way I am doing this only works with 2 players
//            if (otherPlayerIndx != i) { //Only check collisions between this player and other players
//                playersColliding = player->colBox->isOverlapping(*(instances[otherPlayerIndx]->colBox));
//               // goto COLLISION_DETECTED; //I guess 'break' could work too
//            }
//        }
    //COLLISION_DETECTED:
      //  if (playersColliding) {
        //    std::cout << "\nDEBUG::Collision Detected beween players!\n";
       // }
    //}
        
        
//        //Get the collision box rotation information
//        std::cout << "\n  Collision Box information for this player: rotation values are:\n";
//        for (int i = 0; i < player->colBox->getNumberOfRotations(); ++i) {
//            std::cout << player->colBox->getRotationThetaAt(i) << "  ";
//        }
//
    }
    /*
    //Repeat some more debug code (delete all this later plz)
    bool playersColliding = false;
    for (int otherPlayerIndx = 0; otherPlayerIndx < this->getInstanceCount(); ++otherPlayerIndx) {
        //This current way I am doing this only works with 2 players
        if (otherPlayerIndx != i) { //Only check collisions between this player and other players
            playersColliding = player->colBox->isOverlapping(*(instances[otherPlayerIndx]->colBox));
            //goto COLLISION_DETECTED2; //I guess 'break' could work too
        }
    }
     */
//COLLISION_DETECTED2:
    //if (playersColliding) {
        //std::cout << "\nDEBUG::Collision Detected beween players!\n";
    //}
    //----------------------------------------------------
    //std::cout << "\nDEBUG::ColBox area is: " << player->colBox->getQuadrilateralArea() << std::endl;
    
    convertTrianglesIntoLines();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements*2, elements, GL_STREAM_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfElements*2, elements, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, numberOfVertices*2, vertices, GL_STATIC_DRAW);
    
    glad_glEnable(GL_DEPTH_CLAMP); //Gets read of near/far plane culling (but not top/bottom plane, if this makes sense)
    glad_glEnable(GL_DITHER); //Does something
    glad_glEnable(GL_LINE_SMOOTH); //Makes the lines smooth //Don't use, see:https://www.khronos.org/opengl/wiki/Multisampling  (history section)
    glad_glEnable(GL_MULTISAMPLE); //Turns on additional anti-aliasing
    if (DRAW_MODELS) {
        if (DRAW_LINES_PLAYER_MODELS) { //I probably can move this check out so that setting up for lines doesn't happen if false
            //Draw the lines:
            glDrawElements(GL_LINES, sizeof(GLuint) * this->numberOfElements*2, GL_UNSIGNED_INT, 0);
        }
    }
    
    //Now convert the lines back into triangles:
    convertLinesIntoTriangles();
    
    //if (TOGGLE_DRAW_COLLISION_DETAILS || DRAW_COLLISION_DETAILS_ON_GAME_LAUNCH) {
    //if (DRAW_COLLISION_DETAILS) {
    if (getDrawCollisionDetails()) {
        int verticesToReplace = 108;
        //Draw the collision box now
        GLfloat tempFirstVerticesBackup [verticesToReplace];
        for (int i = 0; i < verticesToReplace; ++i) {
            tempFirstVerticesBackup[i] = vertices[i];
        }
        
        //        //ALL DEBUG CODE: ------------------------------------
        //        //Draw a point at the center of the screen
        //        vertices[0] = 0.0f;
        //        vertices[1] = 0.0f;
        //        vertices[2] = -0.9f;
        //        glBufferData(GL_ARRAY_BUFFER, numberOfVertices*2, vertices, GL_STREAM_DRAW);
        //        glDrawArrays(GL_POINT, 0, 1);
        //        aiVector2D temp(0.0f, 0.0f);
        //        player->colBox->setMidpointTo(temp);
        //        //------------------------------------------------------------
        
        
        
        player->colBox->getRectCornersLines3D(vertices);
        
        
        
        //Debug test to see if the point (0,0) is within a collisionBox
        //std::cout << "\nDEBUG::Is within for (0.0, 0.0) is: " << player->colBox->isWithin(0.0f, 0.0f) << "\n\n";
        
        
        
        
        //    if ((int)instances[i]->timeAlive % 4 == 0) {
        ////    if (PRINT_DEBUG_MESSAGES) {
        ////        std::cout << "\nDEBUG::First vertices: \n";
        ////        for (int i = 0; i < 24; ++i) {
        ////            std::cout << vertices[i] <<" ";
        ////        }
        ////    }
        //    }
        
        //see: https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
        glBufferData(GL_ARRAY_BUFFER, numberOfVertices*2, vertices, GL_STREAM_DRAW);
        
        glUniform1f(pul.ulocZoom_Line, 1.0f);
        glUniform1f(pul.ulocXTrans_Line, 0.0f);
        glUniform1f(pul.ulocYTrans_Line, 0.0f);
        glUniform1f(pul.ulocZTrans_Line, 0.0f);
        glUniform1f(pul.ulocPlayerRoll_Line, 0.0f);
        glUniform1f(pul.ulocThetaX_Line, 0.0f);
        glUniform1f(pul.ulocThetaY_Line, 0.0f);
        glUniform1f(pul.ulocThetaZ_Line, 0.0f);
        glUniform1f(pul.ulocRed_Line, 0.0f);
        glUniform1f(pul.ulocGreen_Line, 1.0f);
        glUniform1f(pul.ulocBlue_Line, 0.6f);
        
        
        //Draw the 2D collision box
        glDrawArrays(GL_LINE_LOOP, 0, 8); //Just draw arrays here, no need to mess with an element buffer
        
        //        //Draw the collision box axes (plural of axis)
        //        player->colBox->getRotatedMajorMinor3D(vertices);
        //        glBufferData(GL_ARRAY_BUFFER, numberOfVertices*2, vertices, GL_STREAM_DRAW);
        //        glDrawArrays(GL_LINE_LOOP, 0, 12);
        
        glad_glDisable(GL_LINE_SMOOTH);
        
        //Draw the 3D collision box around the player model
        glUniform1f(pul.ulocRed_Line, 0.3f);
        glUniform1f(pul.ulocGreen_Line, 0.3f);
        glUniform1f(pul.ulocBlue_Line, 0.95f);
        player->colBox->getCubiodTriangles3D(vertices);
        glBufferData(GL_ARRAY_BUFFER, numberOfVertices*2, vertices, GL_STREAM_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0, 36);
        
        glad_glEnable(GL_LINE_SMOOTH);
        
        
        //Put vertices back to normal
        for (int i = 0; i < verticesToReplace; ++i) {
            vertices[i] = tempFirstVerticesBackup[i];
        }
        
    } //End draw collision details check
    
    
    //DRAW MAIN ENGINE FLAME
    shaderArray[2]->use();
    //Note that I don't have 100% of the uniforms for the main engine that I have for the ship body and lines
    glUniform1f(pul.ulocTimeEngine, instances[i]->timeAlive);
    glUniform1f(pul.ulocZoomEngine, instances[i]->zoom);
    glUniform1f(pul.ulocXTransEngine, instances[i]->position.x);
    glUniform1f(pul.ulocYTransEngine, instances[i]->position.y);
    glUniform1f(pul.ulocZTransEngine, instances[i]->position.z);
    glUniform1f(pul.ulocThetaXEngine, instances[i]->thetaX);
    glUniform1f(pul.ulocThetaYEngine, instances[i]->thetaY);
    glUniform1f(pul.ulocThetaZEngine, instances[i]->thetaZ);
    glUniform1f(pul.ulocEngineMainRedinate, player->red);
    glUniform1f(pul.ulocEngineMainBlueify, player->blue);
    
    //            if (PRINT_DEBUG_MESSAGES) {
    //                printf("\nAll of these uloc's are for the main Engine uniforms:\nulocTime is %d\nulocZoom is %d\nulocXTrans is %d\nulocYTrans is %d\nulocZTrans is %d\nulocThetaX is %d\nulocThetaY is %d\nulocThetaX is %d\n", ulocTimeEngine, ulocZoomEngine, ulocXTransEngine, ulocYTransEngine, ulocZTransEngine, ulocThetaXEngine, ulocThetaYEngine, ulocThetaZEngine);
    //            }
    
    //Need to quickly set a dummy value for earlyThetaZ (because main engine won't be affected by roll)
    
    // GLint earlyThetaZMainEngLocation = glGetUniformLocation(shaderArray[2]->getID(), "earlyThetaZ");
    // glUniform1f(earlyThetaZMainEngLocation, 0.0f);
    //            if (PRINT_DEBUG_MESSAGES) {
    //                std::cout << "\nDEBUG::In main engine effect, earlyThetaZ was found and given location " << earlyThetaZMainEngLocation << std::endl;
    //            }
    
    //Make a copy of the first three vertices, because I am going to replace them temporarily to do the engine flames
    aiVector3D first3VertsCopy(vertices[0], vertices[1], vertices[2]);
    
    //Set vertices to a value representing the engine flame rear center point
    //aiVector3D rear(0.0f, 0.0f, -2.94231f);
    aiVector3D rear = *(player->rear);
    rear -= *(player->translationHistory[0]);
    
    // Model data layout (for the spaceship model I have been using)
    //Layout of main engine vertices:
    //             138   136   52
    //               _----------_
    //        140 _-˜            ˜-_ 54
    //           /                  \
    //      142 /                    \ 56
    //         |                      |
    //         |                      | 57
    //      143 \                    /
    //           \                  /
    //        141 ˜-_            _-˜  55
    //               ˜----------˜
    //           139     137     53
    //
    
    //Replace the first three elements of vertices with the components of 'rear'
    vertices[0] = rear.x;
    vertices[1] = rear.y;
    vertices[2] = rear.z;
    
    GLuint tempElementsCopy[16];
    for (int i = 0; i < 16; ++i) {
        tempElementsCopy[i] = elements[i];
    }
    
    elements[0] = 0;
    elements[1] = 52;
    elements[2] = 54;
    elements[3] = 56;
    elements[4] = 57;
    elements[5] = 55;
    elements[6] = 53;
    elements[7] = 137;   //
    elements[8] = 139;
    elements[9] = 141;
    elements[10] = 143;
    elements[11] = 142;
    elements[12] = 140;
    elements[13] = 138;
    elements[14] = 136;
    elements[15] = 52;
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements*2, elements, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices, vertices, GL_STREAM_DRAW);
    
    glDrawElements(GL_TRIANGLE_FAN, 16, GL_UNSIGNED_INT, 0);
    
    
    
    //Draw side Engines
    shaderArray[3]->use();
    //get uniforms for side engines
    glUniform1f(pul.ulocTimeEngineSide, player->timeAlive);
    glUniform1f(pul.ulocZoomEngineSide, player->zoom);
    glUniform1f(pul.ulocXTransEngineSide, player->position.x);
    glUniform1f(pul.ulocYTransEngineSide, player->position.y);
    glUniform1f(pul.ulocZTransEngineSide, player->position.z);
    glUniform1f(pul.ulocThetaXEngineSide, player->thetaX);
    glUniform1f(pul.ulocThetaYEngineSide, player->thetaY);
    glUniform1f(pul.ulocThetaZEngineSide, player->thetaZ);
    glUniform1f(pul.ulocPlayerRollEngineSide, player->rollAmount);

    glUniform1f(pul.ulocEngineSideRedinate, player->red);
    glUniform1f(pul.ulocEngineSideBlueify, player->blue);
    
    //Draw left engine (gonna keep reusing first 3 positions in vertices)
    rear.x = -2.00213f; //From model data for space_ship2.obj  (I did this by hand for just that model)
    rear.y = 0.0f;
    if (player->turnRight) {
        rear.z = -2.91f; //-2.19111 is the most rear side engine z component
    }
    else if (player->turnLeft && !(player->turnRight)) {
        rear.z = -2.01f;
    }
    else { //Not turning
        rear.z = -2.51f;
    }
    
    rear -= *(player->translationHistory[0]);
    
    vertices[0] = rear.x;
    vertices[1] = rear.y;
    vertices[2] = rear.z;
    
    elements[0] = 0;
    elements[1] = 130u;
    elements[2] = 131u;
    elements[3] = 132u;
    elements[4] = 133u;
    elements[5] = 130u;
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements * 2, elements, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices*2, vertices, GL_STREAM_DRAW);
    
    glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, 0);
    
    //Draw right engine
    rear.x = 2.00213f;
    rear.y = 0.0f;
    
    if (player->turnLeft) {
        rear.z = -2.91f; //-2.19111 is the most rear side engine z component
    }
    else if (player->turnRight && !player->turnLeft) {
        rear.z = -2.21f;
    }
    else { //Not turning
        rear.z = -2.51f;
    }
    rear.x += player->translationHistory[0]->x;
    rear.y += player->translationHistory[0]->y;
    rear.z -= player->translationHistory[0]->z; //-z to make flames keep coming out back (instead of going into the ship)
    
    vertices[0] = rear.x;
    vertices[1] = rear.y;
    vertices[2] = rear.z;
    
    //  Right engine element location
    //
    //               50
    //               /\_                     (note that the side engines are
    //              /   \_                    much wider than the one I drew
    //          51 /  0   \ 49                here)
    //             \     _/
    //              \  _/
    //               \/
    //               48
    
    elements[0] = 0;
    elements[1] = 51;
    elements[2] = 50;
    elements[3] = 49;
    elements[4] = 48;
    elements[5] = 51;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements*2, elements, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices*2, vertices, GL_STREAM_DRAW);
    glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, 0);
    
    //Copy back elements to their original arrangement
    for (int i = 0; i < 16; ++i) {
        elements[i] = tempElementsCopy[i];
    }
    //Put vertices back to normal
    vertices[0] = first3VertsCopy.x;
    vertices[1] = first3VertsCopy.y;
    vertices[2] = first3VertsCopy.z;
    
    glad_glDisable(GL_DITHER); //Does something
    glad_glDisable(GL_LINE_SMOOTH); //Makes the lines smooth //Don't use, see history section of: https://www.khronos.org/opengl/wiki/Multisampling
    glad_glDisable(GL_MULTISAMPLE); //Turns on additional anti-aliasing (though I don't really see a difference)
    glad_glDisable(GL_DEPTH_CLAMP);
    
}
//Some helper functions for drawing the player ship:
void Generator::convertTrianglesIntoLines() {
    int elemtsPos = 0;
    GLuint * temp = new GLuint[this->numberOfElements * 9];
    for (int i = 0; i < numberOfElements*2; i+=6) {
        //if elements is:
        //   1    2    3
        //   4    5    6
        //we want the new elements to be
        //  1 2    2 3   3 1
        //  4 5    5 6   6 4
        temp[i] = elements[elemtsPos];
        temp[i+1] = elements[elemtsPos+1];
        temp[i+2] = elements[elemtsPos+1];
        temp[i+3] = elements[elemtsPos+2];
        temp[i+4] = elements[elemtsPos+2];
        temp[i+5] = elements[elemtsPos];
        elemtsPos += 3;
    }
    delete [] this->elements;
    this->elements = temp; //move pointer to head of array 'elements' to the new heap data
    //drawLines = true;
    //drawTriangles = false;
    //Notice in next line that we are buffering (about) double the number of indicies
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements * 2 + 100, elements, GL_STATIC_DRAW);
}

void Generator::convertLinesIntoTriangles() {
    //Just change the ordering in elements[] back to triangle ordering
    //i.e. if elements was:
    //  1 2    2 3   3 1               (or    0 1    1 2    2 0    if indexed from 0 )
    //  4 5    5 6   6 4               (      3 4    4 5    5 3                      )
    //  ...    ...   ...               (      ...    ...    ...                      )
    //then put elements in the trianle vertex ordering:
    //   1    2    3
    //   4    5    6
    int elemtsPos = 0;
    //GLuint temp[this->numberOfElements * 9]; //Stack memory here was causing my bug
    GLuint * temp = new GLuint[this->numberOfElements * 9]; //temp is used to store the new ordering
    for (int i = 0; i < numberOfElements; i+=3) {
        temp[i] = elements[elemtsPos]; //Vert 1
        temp[i+1] = elements[elemtsPos + 1]; //Vert 2
        temp[i+2] = elements[elemtsPos + 3]; //Vert 3
        elemtsPos += 6;
    }
    delete [] this->elements;
    this->elements = temp; //move pointer to head of array 'elements' to the new heap data
    //drawTriangles = true;
    //drawLines = false;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements + 100, elements, GL_STATIC_DRAW);
}

void Generator::doDrawWeaponInstance(int i) {
    WeaponInstance * wep = static_cast<WeaponInstance *>(instances[i]);
    //Figure out what type of weapon instance we have
    switch (wep->getWeaponType()) {
        case KINETIC:
            drawKineticInstance(static_cast<Kinetic *>(wep));
            break;
        case HEXAGON_BOMB:
            //drawHexagonBombInstance(static_cast<HexagonBomb *>(instances[i]));
            break;
        case LASER:
            //drawLaserBombInstance(static_cast<Laser *>(instances[i]));
            break;
        case ROCKET:
        case HOMINGROCKET:
            break;
        case UNINITIALIZED:
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! BEING ASKED TO PRINT AN UNINITIALIZED WEAPON TYPE!\n";
            }
            break;
        default: //Default should never happen
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! BEING ASKED TO PRINT AN UNKNOWN WEAPON TYPE!\n";
            }
            break;
    }
    
}

void Generator::drawKineticInstance(Kinetic * kin) {
    //Set any kinetic-specific parameters...
    //std::cout << "Draw KineticInstance was called by the code!\n";
    
    if (DRAW_MODELS) {
        switch (kin->getModelVersion()) {
            default:
            case 0:
                glDrawArrays(GL_TRIANGLES, 0,  KINETIC_PROJECTILE_VERSION0_VERTS_COUNT / 3);
                break;
            case 1:
                //Change the size of the data being buffered
                glBufferData(GL_ARRAY_BUFFER, KINETIC_PROJECTILE_VERSION2_VERTSSTART, vertices, GL_STREAM_DRAW);
                glDrawArrays(GL_TRIANGLES, KINETIC_PROJECTILE_VERSION1_VERTSSTART, KINETIC_PROJECTILE_VERSION1_VERTS_COUNT / 3);
                break;
            case 2:
                glBufferData(GL_ARRAY_BUFFER, KINETIC_PROJECTILE_VERSION3_VERTSSTART, vertices, GL_STREAM_DRAW);
                glDrawArrays(GL_TRIANGLES, KINETIC_PROJECTILE_VERSION2_VERTSSTART, KINETIC_PROJECTILE_VERSION2_VERTS_COUNT / 3);
                break;
            case 3:
                glBufferData(GL_ARRAY_BUFFER, KINETIC_PROJECTILE_VERSION4_VERTSSTART, vertices, GL_STREAM_DRAW);
                glDrawArrays(GL_TRIANGLES, KINETIC_PROJECTILE_VERSION3_VERTSSTART, KINETIC_PROJECTILE_VERSION3_VERTS_COUNT / 3);
                break;
            case 4:
                glBufferData(GL_ARRAY_BUFFER, KINETIC_PROJECTILE_VERSION5_VERTSSTART, vertices, GL_STREAM_DRAW);
                glDrawArrays(GL_TRIANGLES, KINETIC_PROJECTILE_VERSION4_VERTSSTART, KINETIC_PROJECTILE_VERSION4_VERTS_COUNT / 3);
                break;
            case 5:
                glBufferData(GL_ARRAY_BUFFER, KINETIC_PROJECTILE_VERSION6_VERTSSTART, vertices, GL_STREAM_DRAW);
                glDrawArrays(GL_TRIANGLES, KINETIC_PROJECTILE_VERSION5_VERTSSTART, KINETIC_PROJECTILE_VERSION5_VERTS_COUNT / 3);
                break;
            case 6:
                glBufferData(GL_ARRAY_BUFFER, KINETIC_PROJECTILE_VERSION2_VERTSSTART + KINETIC_PROJECTILE_VERSION6_VERTS_COUNT, vertices, GL_STREAM_DRAW);
                glDrawArrays(GL_TRIANGLES, KINETIC_PROJECTILE_VERSION6_VERTSSTART, KINETIC_PROJECTILE_VERSION6_VERTS_COUNT / 3) ;
                break;
        }
    }
    //Draw the collision box as well if drawCollisionDetails is true
    //if (DRAW_COLLISION_DETAILS || TOGGLE_DRAW_COLLISION_DETAILS) {
    if (getDrawCollisionDetails()) {
        int verticesToReplace = 24;
        
        GLfloat tempFirstVerticesBackup [verticesToReplace];
        for (int i = 0; i < verticesToReplace; ++i) {
            tempFirstVerticesBackup[i] = vertices[i];
        }
        
        kin->colBox->getRectCornersLines3D(vertices);
        
        glBufferData(GL_ARRAY_BUFFER, numberOfVertices*2, vertices, GL_STREAM_DRAW);
        
        //glUniform1f(ulocTime, 0.0f);
        glUniform1f(ulocZoom, 1.0f);
        glUniform1f(ulocXTrans, 0.0f);
        glUniform1f(ulocYTrans, 0.0f);
        glUniform1f(ulocZTrans, 0.0f);
        glUniform1f(ulocThetaX, 0.0f);
        glUniform1f(ulocThetaY, 0.0f);
        glUniform1f(ulocThetaZ, 0.0f);
        
        glDrawArrays(GL_LINE_LOOP, 0, 8);
        
        //Put the vertices back to how they were before drawing collisions
        for (int i = 0; i < verticesToReplace; ++i) {
            vertices[i] = tempFirstVerticesBackup[i];
        }
    }
}


//This was code for drawing multiple instances at once, which didn't work because it messes up
//how I am doing uniforms currently. If instances didn't need to be transformed on the gpu, this
//function could work.
//   One thing to look into: Can transformation information be linked with vertices? So a bunch of models can
//                           be given to the GPU and then the GPU translates them also?
//void Generator::doDrawWeaponInstances() {
//    if (this->activeInstances == 0) {return;}
//    WeaponInstance ** wepInstances = new WeaponInstance * [this->activeInstances];
//    for (unsigned long i = 0ul; i < (unsigned long) this->activeInstances; i++) {
//        wepInstances[i] = static_cast<WeaponInstance*> (instances[i]) ;
//    }
//
//    //all weapons in this generator will be of the same type, so
//    switch (wepInstances[0]->getWeaponType()) {
//        case KINETIC:
//            drawKineticInstances(wepInstances);
//            break;
//        case HEXAGON_BOMB:
//        case LASER:
//        case ROCKET:
//        case HOMINGROCKET:
//            break;
//        case UNINITIALIZED:
//            if (PRINT_DEBUG_WARNING_MESSAGES) {
//                std::cout << "\nDEBUG::WARNING! BEING ASKED TO PRINT AN UNINITIALIZED WEAPON TYPE!\n";
//            }
//            break;
//        default: //Default should never happen
//            if (PRINT_DEBUG_WARNING_MESSAGES) {
//                std::cout << "\nDEBUG::WARNING! BEING ASKED TO PRINT AN UNKNOWN WEAPON TYPE!\n";
//            }
//            break;
//
//    }
//    delete [] wepInstances;
//}

//void Generator::drawKineticInstances(WeaponInstance ** insts) {
//    Kinetic ** kineticInstances = new Kinetic * [this->activeInstances];
//    for (unsigned long i = 0ul; i < (unsigned long) this->activeInstances; i++) {
//        kineticInstances[i] = static_cast<Kinetic *>(insts[i]);
//    }
//
//    //draw each kinetic instance 1 at a time because I forgot that my Vert shader does a lot of the transformation work
//    Kinetic * instToDraw;
//    for (unsigned long i = 0ul; i < (unsigned long)this->activeInstances; i++) {
//        instToDraw = kineticInstances[i];
//    }
//}

