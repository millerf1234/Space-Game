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
            
           // if (instances[i] != nullptr) {
//                if (instances[i].instanceExpansionData != nullptr) {
//                    delete instances[i].instanceExpansionData;
//                    instances[i].instanceExpansionData = nullptr;
//                }
           // }
        }
    }
    delete [] instances; //Delete the array of instances
}

void Generator::initializeFromTemplate(const InitializationTemplate& t) {
    if (this->wasInitialized) {
        std::cout << "\nDEBUG::OOPS! Attempting to initialize from template a generator which has already been initialized!\n";
        return;
    }
    this->wasInitialized = true;
    
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
        this->numberOfElements = t.numElems;
        this->elements = new GLuint[t.numElems];
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
            //            //Load Elements from face data
            int faceCounter = 0;
            this->vertexData = new SimpleObjLoader((char *)t.modelFilePath.c_str());
            //Copy over elements
            this->elements = new GLuint[vertexData->model.faces*3];
            for (int i = 0; i < vertexData->model.faces * 3; i += 3) {
                this->elements[i] = vertexData->faces[faceCounter][0] - 1;
                this->elements[i+1] = vertexData->faces[faceCounter][3] - 1;
                this->elements[i+2] = vertexData->faces[faceCounter][6] - 1;
                ++faceCounter;
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
            //else if (t.vert3norml3) {
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
            //}
            else {
                std::cout << "\nDEBUG::OOPS! That format not yet implemented in generator's objLoader code!\n";
            }
        }
        else {
            std::cout << "\nDEBUG::OOPS! That format not yet implemented in generator's objLoader code";
            std::cout << std::endl;
        }
        //Might want to delete the objLoader once I have all of the data from it
        //this->numberOfVertices = (vertexData->model.positions + vertexData->model.normals) * 3;
        //this->numberOfElements = vertexData->model.faces;
    }
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
    
    //Set uniform locations in the shader program
    GLuint shaderID = shaderArray[0]->getID();
    ulocTime = glGetUniformLocation(shaderID, "time");
    ulocZoom = glGetUniformLocation(shaderID, "zoom");
    ulocXTrans = glGetUniformLocation(shaderID, "xTrans");
    ulocYTrans = glGetUniformLocation(shaderID, "zTrans");
    ulocZTrans = glGetUniformLocation(shaderID, "zTrans");
    ulocThetaX = glGetUniformLocation(shaderID, "thetaX");
    ulocThetaY = glGetUniformLocation(shaderID, "thetaY");
    ulocThetaZ = glGetUniformLocation(shaderID, "thetaZ");
   
     //GLint ulocTime, ulocZoom, ulocXTrans, ulocYTrans, ulocZTrans, ulocThetaX, ulocThetaY, ulocThethaZ;
    glBindVertexArray(0);
    
}

void Generator::doUpkeep() {
    if (instances == nullptr) {return;}
    for (int i = 0; i < activeInstances; ++i) {
        instances[i].timeAlive += TIME_TICK_RATE;
    }
}

void Generator::setSpecialization(specializationType expansionType) {
    //Don't change specialization type if instances have already been generated
    if (this->activeInstances > 0) {
        std::cout  << " \nDEBUG::OOPS! Unable to set expansion type once instances have been generated!";
        return;
    }
    //Don't change specialization type if a specialization type has already been specified
    if (this->specialization != NOSPECIALIZATION) {
        std::cout << "\nDEBUG::OOPS! SpecializationExpansion type has already been set for this generator, unable to change once set!\n";
        return;
    }
    if (expansionType == specializationType::PLAYER) {
        this->specialization = expansionType;
    }
    else if (expansionType == specializationType::WEAPON) {
        this->specialization  = expansionType;
    }
}

void Generator::generateSingle() {
    if (!this->readyToGenerate()) {
        std::cout << "\nDEBUG::Oops! It looks like you are not yet ready to generate objects of this type!\n";
        return;
    }
    ++activeInstances;
    if (this->instances == nullptr) { //If this is the first instance
        this->instances = new Instance[activeInstances];
    }
    else { //Else Need to make a new array of instances that's 1 larger than before
        Instance * temp = new Instance[activeInstances];
        for (int i = 0; i < activeInstances - 1; ++i) {
            temp[i] = this->instances[i];
        }
        delete [] this->instances;
        this->instances = temp; //Set instances to temp
    }
    
    int newInstanceIndex = activeInstances - 1;
    //Give the new instance a new ID number
    instances[newInstanceIndex].identifierNumber = nextObjID++;
    instancesCreatedByThisGenerator.push_back(instances[newInstanceIndex].identifierNumber);
    
    //Set up the rest of the new instance
    this->instances[newInstanceIndex].position = aiVector3D(0.0f, 0.0f, 0.0f);
    this->instances[newInstanceIndex].zoom = 3.5f; //Higher zoom means farther from camera
    this->instances[newInstanceIndex].timeAlive = 0.0f;
    this->instances[newInstanceIndex].thetaX = 0.0f;
    this->instances[newInstanceIndex].thetaY = 0.0f;
    this->instances[newInstanceIndex].thetaZ = 0.0f;
    
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
    //Turn on shader effect
    this->shaderArray[0]->use();
    if (this->hasTexture()){
        this->textureArray[0]->activate();
    }
    
   // glGetActiveAttrib;
    //this->textureArray[0]->tex[0] = 0; //This might fix it? No, I had 2 different tex's
   // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureArray[0]->tex[0], 0); //Might need to do this?
    
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
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
glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices + 100, this->vertices, GL_STATIC_DRAW);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements + 100, elements, GL_STATIC_DRAW);
    
   // glUinform1f
    
    for (int i = 0; i < activeInstances; ++i) {
//        //DEBUG:
//        for (int i = 0; i < 28; ++i) {
//            //printf("%3.2f ", vertices[i]);
//            //std::cout << vertices[i] << " ";
//            //if (i%7 == 6) {std::cout << std::endl;}
//        }
        
        //Set Uniforms for this instance
        glUniform1f(ulocTime, instances[i].timeAlive);
        glUniform1f(ulocZoom, instances[i].zoom);
        glUniform1f(ulocXTrans, instances[i].position.x);
        glUniform1f(ulocYTrans, instances[i].position.y);
        glUniform1f(ulocZTrans, instances[i].position.z);
        glUniform1f(ulocThetaX, instances[i].thetaX);
        glUniform1f(ulocThetaY, instances[i].thetaY);
        glUniform1f(ulocThetaZ, instances[i].thetaZ);
        
        if (specialization == specializationType::PLAYER) { //Do extra stuff required for players
            PlayerInstance * p1 = dynamic_cast<PlayerInstance*>(&instances[0]);
            std::cout << "\nDEBUG::CHECK TO SEE IF P1 is nullptr: " << p1 << std::endl;
            
            if (MAX_PLAYERS >= 2) {
                PlayerInstance* p2 = dynamic_cast<PlayerInstance*>(&instances[1]);
            }
        }
        else if (specialization == specializationType::WEAPON) {
            
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
            std::cout << "\nDEBUG::ERROR(sorta)! Generator's drawInstances command was called with an unrecognized (or just\ncurrently unimplemented) type! Please implement this type!\n\n";
        }
    }
}

//Note that this function will not be efficient if called in a loop to delete a bunch of instances
void Generator::removeInstance(const int & instantID) {
    //Check to make sure instanceID matches an instance generated by this generator
    std::vector<int>::iterator iter;
    iter = find (instancesCreatedByThisGenerator.begin(), instancesCreatedByThisGenerator.end(), instantID);
    if (iter == instancesCreatedByThisGenerator.end()) { //If iterator reached the end
        if ((*iter) != instantID) { //if the end value is not equal to the instanceID
            std::cout << "\n    DEBUG::OOPS! removeInstance called for instance with id # ";
            std::cout << instantID << " but that instance was not found\nwithin this generator!";
            std::cout << std::endl;
            return;
        }
        //else instance is the final value in the vector, so just remove the final value from the array
        --activeInstances; //Decrement the number of active instances by 1
        Instance * temp = new Instance[activeInstances];
        for (int i = 0; i < activeInstances - 1; ++i) {
            temp[i] = this->instances[i];
        }
        delete [] this->instances;
        this->instances = temp; //Set instances to temp
    }
    //else the instance is located in the array somewhere besides the end
    bool found = false; //Need 'found' to keep array indexes lined up
    Instance * temp = new Instance[activeInstances];
    for (int i = 0; i < activeInstances; ++i) {
        if (instances[i].getID() == instantID) {
            found = true;
            //DEBUG:
            std::cout << "\nDEBUG::Deleted intance #" << instantID << std::endl;
        }
        else if (found) {
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
    GLuint temp[this->numberOfElements * 9];
    for (int i = 0; i < numberOfElements*3*2; i+=6) {
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
    this->elements = temp;
    drawLines = true;
    drawTriangles = false;
}

void Generator::convertLinesIntoTriangles() {
    int elemtsPos = 0;
    GLuint temp[this->numberOfElements * 9];
    for (int i = 0; i < numberOfElements; i+=3) {
        temp[i] = elements[elemtsPos];
        temp[i+1] = elements[elemtsPos + 1];
        temp[i+2] = elements[elemtsPos + 3];
        elemtsPos += 6;
    }
    this->elements = temp;
    drawTriangles = true;
    drawLines = false;
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
