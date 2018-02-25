 //  Implementation for the Generator class
//  Generator.cpp
//  Created by Forrest Miller on 2/17/18.


#include "Generator.h"

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
    this->vertexData = nullptr;
    this->instances = nullptr;
    this->activeInstances = 0;
    
    //Set specialization type for this generator
    specialization  = specializationType::NOSPECIALIZATION; //No extra data by default
    
    //Set special uniform location variables to -1 (their default not_found code)
    ulocRed = ulocGreen = ulocBlue = -1;
    ulocRedLine = ulocGreenLine = ulocBlueLine = -1;
    ulocTimeLine = ulocZoomLine = ulocXTransLine = ulocYTransLine = ulocZTransLine = ulocThetaXLine = ulocThetaYLine = ulocThetaZLine = -1;
    ulocTimeEngine = ulocZoomEngine = ulocXTransEngine = ulocYTransEngine = ulocZTransEngine = ulocThetaXEngine = ulocThetaYEngine = ulocThetaZEngine = -1;
    ulocTimeEngineSide = ulocZoomEngineSide = ulocXTransEngineSide = ulocYTransEngineSide = ulocZTransEngineSide = ulocThetaXEngineSide = ulocThetaYEngineSide = ulocThetaZEngineSide = -1;
    ulocPDamage = ulocPHealthMax = -1;
    ulocPlayerRoll = ulocPlayerRollLine = ulocPlayerRollEngineSide = -1;
    
    this->drawTriangles = true;
    this->drawLines = false;
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
    if (this->vertexData != nullptr) {
        delete this->vertexData;
        this->vertexData = nullptr;
    }
    
    if (this->activeInstances > 0 && this->instances != nullptr) {
        for (int i = 0; i < activeInstances; ++i) {
            if (instances[i] != nullptr) {
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

void Generator::initializeFromTemplate(const InitializationTemplate& t) {
    if (this->wasInitialized) {
        std::cout << "\nDEBUG::OOPS! Attempting to initialize from template a generator which has already been initialized!\n";
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
        this->vertexData = nullptr; //Make sure vertex data is nullptr
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
            this->vertexData = new SimpleObjLoader((char *)t.modelFilePath.c_str()); //Create the objLoader object
            
            //Copy over elements first (NOTE: Elements are independent of vertex format)
            this->elements = new GLuint[vertexData->model.faces*3*2];
            int numElementsToPrintForDebug = 9; //Part of debug messages even though it's outside the check for debug (need to do this so code inside will run)
            if (PRINT_DEBUG_MESSAGES) {
                std::cout << "DEBUG::Here are the first " << numElementsToPrintForDebug << " values that were loaded into Elements: ";
            }
            for (int i = 0; i < vertexData->model.faces * 3; i += 3) {
                this->elements[i] = vertexData->faces[faceCounter][0] - 1;
                this->elements[i+1] = vertexData->faces[faceCounter][3] - 1;
                this->elements[i+2] = vertexData->faces[faceCounter][6] - 1;
                ++faceCounter;
                //Print debug message giving first few Elements that were loaded
                if (PRINT_DEBUG_MESSAGES && i < numElementsToPrintForDebug) {
                    std::cout << "\nElements[" << i << " to " << i+2  << "] were set to " << elements[i] << " " << elements[i+1] << " " << elements[i+2];
                }
            }
    
            this->numberOfElements = vertexData->model.faces * 3;
            
            //Load vertices
            if (t.vert3) { //This is easy, just copy the position data right over
                vertices = new GLfloat [vertexData->model.positions*3];
                int vertPosCounter = 0;
                for (int i = 0; i < vertexData->model.positions; ++i) {
                    vertices[vertPosCounter] = vertexData->positions[i][0];
                    vertices[vertPosCounter + 1] = vertexData->positions[i][1];
                    vertices[vertPosCounter + 2] = vertexData->positions[i][2];
                    vertPosCounter += 3;
                }
                this->numberOfVertices = vertexData->model.positions*3;
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
                std::cout << "\nDEBUG::OOPS! That format not yet implemented in generator's objLoader code!\n";
            }
        }
        else {
            std::cout << "\nDEBUG::OOPS! That format not yet implemented in generator's objLoader code";
            std::cout << std::endl;
        }
    }
    
    //Might want to delete the objLoader once I have all of the data from it
    //this->numberOfVertices = (vertexData->model.positions + vertexData->model.normals) * 3;
    //this->numberOfElements = vertexData->model.faces;
    
    //Buffer the data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //std::cout << std::endl << "DEBUG:: sizeof(stackVertices) is: " << sizeof(stackVertices) << std::endl;
    
    //Note that sizeof() behaves differently between Stack and Heap memory
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
        //Currently STAGE has no unique uniforms that need to be set.
    }
    else if (this->specialization == PLAYER) {
        //Set the uniform locations for PLAYER
        ulocRed = glGetUniformLocation(shaderID, "red");
        ulocGreen = glGetUniformLocation(shaderID, "green");
        ulocBlue = glGetUniformLocation(shaderID, "blue");
        ulocPDamage = glGetUniformLocation(shaderID, "damage");
        ulocPHealthMax = glGetUniformLocation(shaderID, "maxHealth");
        ulocPlayerRoll = glGetUniformLocation(shaderID, "earlyThetaZ");
        
        std::cout << "\nDEBUG:\n        ulocRed = " << ulocRed;
        std::cout << "\n        ulocGreen = " << ulocGreen;
        std::cout << "\n        ulocBlue = " << ulocBlue;
        std::cout << "\n        ulocPDamage = " << ulocPDamage;
        std::cout << "\n        ulocPHealthMax = " << ulocPHealthMax;
        
        //(I realize what I am doing here in this next bit of code is very sloppy, inadvisiable and may cause many headaches down the road. Sorry in advance)
        
        //Add extra shaders for the engine effects to ShaderArray
        std::cout << "\nDEBUG::Creating extra shaders required for PLAYER..." << std::endl;
        this->shaderArraySize = 4;
        ShaderWrapper ** temp = new ShaderWrapper * [shaderArraySize];
        temp[0] = this->shaderArray[0]; //ShaderArray was set to size 1, so need to make it bigger
        delete [] this->shaderArray;
        this->shaderArray = temp;
        this->shaderArray[1] = new ShaderWrapper;
        this->shaderArray[2] = new ShaderWrapper;
        this->shaderArray[3] = new ShaderWrapper;

        //Set up the Line Shader:
        std::cout << "    Creating outline shader...\n";
//        this->shaderArray[1] = new ShaderWrapper;
        this->shaderArray[1]->attachVert((char *)PLAYERSHIP_LINE_VERT.c_str());
        this->shaderArray[1]->attachFrag((char *)PLAYERSHIP_LINE_FRAG.c_str());
        
        this->shaderArray[1]->link();
        this->shaderArray[1]->setVertexAttribName((char *)t.vertAttribName.c_str()); //Use same name as body shader name
        this->shaderArray[1]->setVAOExternally(tempVAO); //this way the shaders share the same VAO
        this->shaderArray[1]->specifyVertexLayout(ShaderWrapper::VERT3, vbo, ebo);
        //set uniforms for the line shader:
        shaderID = shaderArray[1]->getID();
        ulocRedLine = glGetUniformLocation(shaderID, "red");
        ulocGreenLine = glGetUniformLocation(shaderID, "green");
        ulocBlueLine = glGetUniformLocation(shaderID, "blue");
        ulocTimeLine = glGetUniformLocation(shaderID, "time");
        ulocZoomLine = glGetUniformLocation(shaderID, "zoom");
        ulocXTransLine = glGetUniformLocation(shaderID, "xTrans");
        ulocYTransLine = glGetUniformLocation(shaderID, "yTrans");
        ulocZTransLine = glGetUniformLocation(shaderID, "zTrans");
        ulocThetaXLine = glGetUniformLocation(shaderID, "thetaX");
        ulocThetaYLine = glGetUniformLocation(shaderID, "thetaY");
        ulocThetaZLine = glGetUniformLocation(shaderID, "thetaZ");
        ulocPlayerRollLine = glGetUniformLocation(shaderID, "earlyThetaZ");
        
        std::cout << "    DEBUG::PlayerLineColorLocations: \n";
        std::cout << "ulocRedLine = " << ulocRedLine << std::endl;
        std::cout << "ulocGreenLine = " << ulocGreenLine << std::endl;
        std::cout << "ulocBlueLine = " << ulocBlueLine << std::endl;

        
        if (shaderArray[1]->checkIfReady()) {
            std::cout << "\n       Outline shader ready!\n";
        }
        else {
            std::cout << "\n      OOPS! For some reason Outline shader is showing not ready...\n";
        }
        //Set up the main engine shader
//        this->shaderArray[2] = new ShaderWrapper;
        std::cout << "    Creating Main engine shader...\n";
        this->shaderArray[2]->attachVert((char *)PLAYERSHIP_ENGINE_VERT.c_str());
        this->shaderArray[2]->attachFrag((char *)PLAYERSHIP_ENGINE_FRAG.c_str());
        
        this->shaderArray[2]->link();
        this->shaderArray[2]->setVertexAttribName((char *) "enginePos");
        this->shaderArray[2]->setVAOExternally(tempVAO);
        this->shaderArray[2]->specifyVertexLayout(ShaderWrapper::VERT3, vbo, ebo);
        //Set uniforms for the main engine
        shaderID = shaderArray[2]->getID();
        ulocTimeEngine = glGetUniformLocation(shaderID, "time");
        ulocZoomEngine = glGetUniformLocation(shaderID, "zoom");
        ulocXTransEngine = glGetUniformLocation(shaderID, "xTrans");
        ulocYTransEngine = glGetUniformLocation(shaderID, "yTrans");
        ulocZTransEngine = glGetUniformLocation(shaderID, "zTrans");
        ulocThetaXEngine = glGetUniformLocation(shaderID, "thetaX");
        ulocThetaYEngine = glGetUniformLocation(shaderID, "thetaY");
        ulocThetaZEngine = glGetUniformLocation(shaderID, "thetaZ");
        //no ulocPlayerRoll = glGetUniformLocation(shaderID, "earlyThetaZ"); //Because main engine is symmetric about roll axis
        if (shaderArray[2]->checkIfReady()) {
            std::cout << "\n       Main Engine shader ready!\n";
        }
        else {
            std::cout << "\n      OOPS! For some reason Main Engine shader is showing not ready...\n";
        }
        
        std::cout << "    Creating Side engine shader...\n";
        //        this->shaderArray[3] = new ShaderWrapper;
        this->shaderArray[3]->attachVert((char *)PLAYERSHIP_ENGINE_VERT.c_str());
        this->shaderArray[3]->attachFrag((char *)PLAYERSHIP_ENGINE_FRAG.c_str());
        
        this->shaderArray[3]->link();
        this->shaderArray[3]->setVertexAttribName((char *) "enginePos");
        this->shaderArray[3]->setVAOExternally(tempVAO);
        this->shaderArray[3]->specifyVertexLayout(ShaderWrapper::VERT3, vbo, ebo);
        shaderID = shaderArray[3]->getID();
        ulocTimeEngineSide = glGetUniformLocation(shaderID, "time");
        ulocZoomEngineSide = glGetUniformLocation(shaderID, "zoom");
        ulocXTransEngineSide = glGetUniformLocation(shaderID, "xTrans");
        ulocYTransEngineSide = glGetUniformLocation(shaderID, "yTrans");
        ulocZTransEngineSide = glGetUniformLocation(shaderID, "zTrans");
        ulocThetaXEngineSide = glGetUniformLocation(shaderID, "thetaX");
        ulocThetaYEngineSide = glGetUniformLocation(shaderID, "thetaY");
        ulocThetaZEngineSide = glGetUniformLocation(shaderID, "thetaZ");
        ulocPlayerRollEngineSide = glGetUniformLocation(shaderID, "earlyThetaZ");
        
        if (shaderArray[3]->checkIfReady()) {
            std::cout << "\n       Side Engine shader ready!\n";
        }
        else {
            std::cout << "\n      OOPS! For some reason Side Engine shader is showing not ready...\n";
        }
        std::cout << "\nAll shaders for Player finished building!\n";
    }
    else if (this->specialization == WEAPON) {
        //Set the uniform locations for WEAPON
    }
   
     //GLint ulocTime, ulocZoom, ulocXTrans, ulocYTrans, ulocZTrans, ulocThetaX, ulocThetaY, ulocThethaZ;
    glBindVertexArray(0);
    
}

void Generator::doUpkeep() {
    if (instances == nullptr) {return;}
    for (int i = 0; i < activeInstances; ++i) { //Update every instances position with it's velocity
        instances[i]->timeAlive += TIME_TICK_RATE;
        bool isPlayerInstance = (instances[i]->type == InstanceType::PLAYERINSTANCE); //Set a bool variable since I need to do this check multiple times
        if (isPlayerInstance) {
        aiVector2D tempForMaxSpeedCheck(instances[i]->velocity.x, instances[i]->velocity.y);
        if (tempForMaxSpeedCheck.Length() > PLAYER_MOVEMENT_MAX_SPEED) {
            tempForMaxSpeedCheck.Normalize();
            tempForMaxSpeedCheck.operator*=(PLAYER_MOVEMENT_MAX_SPEED);
            instances[i]->velocity.x = tempForMaxSpeedCheck.x;
            instances[i]->velocity.y = tempForMaxSpeedCheck.y;
        }
        }
        instances[i]->position.x += instances[i]->velocity.x;
        instances[i]->position.y += instances[i]->velocity.y;
        
        //Keep player instances inbounds
        if (isPlayerInstance) {
            //Check to see if out of bounds, and if so, then move back inbounds
            if (instances[i]->position.x > XLIMIT) {
                instances[i]->position.x = XLIMIT;
                instances[i]->velocity.x = 0.0f;
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

//NOTICE:: setSpecialization is now deprecated and should not be used at all
void Generator::setSpecialization(specializationType expansionType) {
    //I now set an expansion type within the initializerTemplate, so the generator knows its type much earlier on in its setup process. As such, I moved the glGetUniformLocation calls to be all next to each other near the end of the function initializeFromTemplate(). So the generic calls are made first then uniform locations are set based off the type, which is known since it was given as part of the intializer template.
    //TL,DR: This function's functionality has been moved to within initializeFromTemplate()
//    //Don't change specialization type if instances have already been generated
//    if (this->activeInstances > 0) {
//        std::cout << "\nDEBUG::OOPS! Unable to set expansion type once instances have been generated!";
//        return;
//    }
//    //Don't change specialization type if a specialization type has already been specified
//    if (this->specialization != NOSPECIALIZATION) {
//        std::cout << "\nDEBUG::OOPS! SpecializationExpansion type has already been set for this generator, unable to change once set!\n";
//        return;
//    }
//    if (expansionType == specializationType::PLAYER) {
//        //Set the Uniform Locations for the player type
//        this->specialization = expansionType;
//        GLuint shaderID = shaderArray[0]->getID();
//        ulocRed = glGetUniformLocation(shaderID, "red");
//        ulocGreen = glGetUniformLocation(shaderID, "green");
//        ulocBlue = glGetUniformLocation(shaderID, "blue");
//
//        std::cout << "\nDEBUG:\n        ulocRed = " << ulocRed;
//        std::cout << "\n        ulocGreen = " << ulocGreen;
//        std::cout << "\n        ulocBlue = " << ulocBlue;
//
//        //Add another shader for Engine Shaders? Or just have the template passed to generator know to
//        //create multiple shaders. Both will work...
//
//    }
//    else if (expansionType == specializationType::WEAPON) {
//        //Set the Uniform Locations for the player type
//        this->specialization  = expansionType;
//    }
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
    //Create an instance based off generator type (note that special types will have their values set correctly by their constructors (at least in theory they will)
    if (specialization == specializationType::PLAYER) {//Generate a player instance
        instances[newInstanceIndex] = new PlayerInstance(nextObjID);
        instances[newInstanceIndex]->type = PLAYERINSTANCE;
        instances[newInstanceIndex]->identifierNumber = (nextObjID); //Not sure why this isn;t getting set...
        ++nextObjID;
    }
    else if (specialization == specializationType::WEAPON) {//Generate a weapon instance
        instances[newInstanceIndex] = new WeaponInstance;
        instances[newInstanceIndex]->identifierNumber = nextObjID++;
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
    
    //Set up the rest of the new instance
    this->instances[newInstanceIndex]->position = aiVector3D(0.0f, 0.0f, 0.0f);
    this->instances[newInstanceIndex]->zoom = 3.5f; //Higher zoom means farther from camera
    this->instances[newInstanceIndex]->timeAlive = 0.0f;
    this->instances[newInstanceIndex]->thetaX = 0.0f;
    this->instances[newInstanceIndex]->thetaY = 0.0f;
    this->instances[newInstanceIndex]->thetaZ = 0.0f;
    
    //Set up the specialized Instance bits
//    if (this->currentExpansionType != NOEXTRADATA) {
//        switch (currentExpansionType) {
//            case PLAYERDATA:
//                this->instances[newInstanceIndex].instanceExpansionData = (void *) new PlayerData(activeInstances);
//                break;
//            case WEAPONDATA:
//                this->instances[newInstanceIndex].instanceExpansionData = (void *) new WeaponData;
//                break;
//        }
//    }
//    else {
//        this->instances[newInstanceIndex].instanceExpansionData = nullptr;
//    }
}


void Generator::drawInstances() {
    if (!wasInitialized || this->activeInstances == 0) {return;}
    //Turn on shader effect (actually turn it on inside the loop
//    this->shaderArray[0]->use();
//    if (this->hasTexture()){
//        this->textureArray[0]->activate();
//    }
    
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
    
   // glUinform1f
    
    for (int i = 0; i < activeInstances; ++i) {
//        //DEBUG:
//        for (int i = 0; i < 28; ++i) {
//            //printf("%3.2f ", vertices[i]);
//            //std::cout << vertices[i] << " ";
//            //if (i%7 == 6) {std::cout << std::endl;}
//        }
        this->shaderArray[0]->use();
        if (this->hasTexture()){
            this->textureArray[0]->activate();
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices + 100, this->vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements + 100, elements, GL_STATIC_DRAW);
        
        //Set Uniforms for this instance
        glUniform1f(ulocTime, instances[i]->timeAlive);
        glUniform1f(ulocZoom, instances[i]->zoom);
        glUniform1f(ulocXTrans, instances[i]->position.x);
        glUniform1f(ulocYTrans, instances[i]->position.y);
        glUniform1f(ulocZTrans, instances[i]->position.z);
        glUniform1f(ulocThetaX, instances[i]->thetaX);
        glUniform1f(ulocThetaY, instances[i]->thetaY);
        glUniform1f(ulocThetaZ, instances[i]->thetaZ);
        
        if (specialization == specializationType::PLAYER) { //Do extra stuff required for drawing the player models
            PlayerInstance * player = static_cast<PlayerInstance*>(instances[i]);
            //For debug (back when i was doing dynamic_cast still):
            //std::cout << "\nDEBUG::CHECK TO SEE IF player is nullptr: " << player << std::endl;
            glUniform1f(ulocRed, player->red);
            glUniform1f(ulocGreen, player->green);
            glUniform1f(ulocBlue, player->blue);
            glUniform1f(ulocPlayerRoll, player->rollAmount);
            
            //Draw the player body (using shader in shadderArray[0]
            glDrawElements(GL_TRIANGLES, this->numberOfElements, GL_UNSIGNED_INT, 0);
            
            //Draw Lines
            shaderArray[1]->use();
            glUniform1f(ulocTimeLine, instances[i]->timeAlive);
            glUniform1f(ulocZoomLine, instances[i]->zoom);
            glUniform1f(ulocXTransLine, instances[i]->position.x);
            glUniform1f(ulocYTransLine, instances[i]->position.y);
            glUniform1f(ulocZTransLine, instances[i]->position.z);
            glUniform1f(ulocThetaXLine, instances[i]->thetaX);
            glUniform1f(ulocThetaYLine, instances[i]->thetaY);
            glUniform1f(ulocThetaZLine, instances[i]->thetaZ);
            glUniform1f(ulocRedLine, player->red * PLAYER_LINE_COLOR_BOOST_FACTOR);
            glUniform1f(ulocGreenLine, player->green * PLAYER_LINE_COLOR_BOOST_FACTOR);
            glUniform1f(ulocBlueLine, player->blue * PLAYER_LINE_COLOR_BOOST_FACTOR);
            glUniform1f(ulocPlayerRollLine, player->rollAmount);
            
            //Debug code:
//            for (int i = 0; i < 18; ++i) {
//                std::cout << "\nElements[" << i << "] = " << elements[i];
//            }
            convertTrianglesIntoLines(); //This line NOT debug code
//            std::cout << "\nConverting...";
//            for (int i = 0; i < 36; ++i) {
//                std::cout << "\nElements[" << i << "] = " << elements[i];
//            }
//            std::cout << std::endl << std::endl;
            //std::cout << "\n    DEBUG::sizeof(elements) = " << sizeof(elements) << std::endl;
            //End debug code
            
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements * 2, elements, GL_STREAM_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, numberOfVertices, vertices, GL_STREAM_DRAW);
            
            glad_glEnable(GL_DEPTH_CLAMP); //Gets read of near/far plane culling (but not top/bottom plane, if this makes sense)
            glad_glEnable(GL_DITHER); //Does something
            glad_glEnable(GL_LINE_SMOOTH); //Makes the lines smooth
            glad_glEnable(GL_MULTISAMPLE); //Turns on additional anti-aliasing
            //Draw the lines:
             glDrawElements(GL_LINES, this->numberOfElements*2, GL_UNSIGNED_INT, 0);
            
            //Now convert the lines back into triangles:
            convertLinesIntoTriangles();
            glad_glDisable(GL_DITHER); //Does something
            glad_glDisable(GL_LINE_SMOOTH); //Makes the lines smooth
            glad_glDisable(GL_MULTISAMPLE); //Turns on additional anti-aliasing (though I don't really see a difference)
            glad_glDisable(GL_DEPTH_CLAMP);
            
            //Draw main Engine
             glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements * 2, elements, GL_STREAM_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
            
            
            //Draw side Engines
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements * 2, elements, GL_STREAM_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
            
            
        }
        else if (specialization == specializationType::WEAPON) {
            //Do the weapon drawing stuff
        }
        else if (specialization == specializationType::STAGE) {
            glDrawElements(GL_TRIANGLES, this->numberOfElements, GL_UNSIGNED_INT, 0);
        }
        else if (specialization == specializationType::NOSPECIALIZATION) {
            if (drawTriangles) {
                glDrawElements(GL_TRIANGLES, this->numberOfElements, GL_UNSIGNED_INT, 0);
            }
            else if (drawLines) {
                glDrawElements(GL_LINES, this->numberOfElements * 2, GL_UNSIGNED_INT, 0);
            }
            else {
                glDrawElements(GL_POINTS, this->numberOfElements, GL_UNSIGNED_INT, 0);
            }
        }
        else {
            std::cout << "\nDEBUG::ERROR(sorta)! Generator's drawInstances command was called with an unrecognized (or just\ncurrently unimplemented) type! Please implement this type or fix error!\n\n";
        }
    }
    //It's generally a good idea to set these next two things to 0 after having used them
    glBindVertexArray(0); //Vertex attribute array
    glUseProgram(0);
}

//Note that this function will not be efficient if called in a loop to delete a bunch of instances
void Generator::removeInstance(const int & instantID) {
    //Check to make sure instanceID matches an instance generated by this generator
    std::vector<int>::iterator iter;
    iter = find (instancesCreatedByThisGenerator.begin(), instancesCreatedByThisGenerator.end(), instantID);
    if (iter == instancesCreatedByThisGenerator.end()) { //If iterator reached the end
        //if ((*iter) != instantID) { //if the end value is not equal to the instanceID
        std::cout << "\n    DEBUG::OOPS! removeInstance called for instance with id # ";
        std::cout << instantID << " but that instance was not found\nwithin this generator!";
        std::cout << std::endl;
        return;
        //}
        //IT TURNS OUT THAT VECTOR.END() is the element past the last element in the vector
//        //else instance is the final value in the vector, so just remove the final value from the array
//        --activeInstances; //Decrement the number of active instances by 1
//        Instance ** temp = new Instance*[activeInstances];
//        for (int i = 0; i < activeInstances - 1; ++i) {
//            temp[i] = this->instances[i];
//        }
//        delete [] this->instances;
//        this->instances = temp; //Set instances to temp
    }
    //else the instance is located in the array somewhere besides the end
    bool found = false; //Need 'found' to keep array indexes lined up
    Instance ** temp = new Instance*[activeInstances];
    for (int i = 0; i < activeInstances; ++i) {
        if (instances[i] != nullptr && instances[i]->getID() == instantID) {
            found = true;
            //delete the instance that is being removed:
            delete instances[i];
            instances[i] = nullptr;
            //DEBUG:
            std::cout << "\nDEBUG::Deleted intance #" << instantID << std::endl;
        }
        else if (found) { //If we already deleted the instance, then just move the rest of the array over
            temp[i] = instances[i+1];
        }
        else {
            temp[i] = instances[i];
        }
    }
    --activeInstances;
    delete [] this->instances; //delete the old instance array
    this->instances = temp; //set instance array to the new array
}

void Generator::removeInstance(Instance * instPtr) {
    removeInstance(instPtr->getID());
}

void Generator::convertTrianglesIntoLines() {
    int elemtsPos = 0;
    GLuint * temp = new GLuint[this->numberOfElements * 9];
    for (int i = 0; i < numberOfElements*2; i+=6) {
        //if elements is:
        //   1   2    3
        //   4   5    6
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
    drawLines = true;
    drawTriangles = false;
    //Notice in next line that we are buffering (about) double the number of indicies
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements * 2 + 100, elements, GL_STATIC_DRAW);
}

void Generator::convertLinesIntoTriangles() {
    int elemtsPos = 0;
    //GLuint temp[this->numberOfElements * 9]; //Stack memory here was causing my bug
    GLuint * temp = new GLuint[this->numberOfElements * 9];
    for (int i = 0; i < numberOfElements; i+=3) {
        temp[i] = elements[elemtsPos]; //Vert 1
        temp[i+1] = elements[elemtsPos + 1]; //Vert 2
        temp[i+2] = elements[elemtsPos + 3]; //Vert 3
        elemtsPos += 6;
    }
    delete [] this->elements;
    this->elements = temp; //move pointer to head of array 'elements' to the new heap data
    drawTriangles = true;
    drawLines = false;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements + 100, elements, GL_STATIC_DRAW);
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
