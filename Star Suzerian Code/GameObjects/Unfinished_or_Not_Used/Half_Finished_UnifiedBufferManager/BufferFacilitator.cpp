//Oh wait, I just remembered that different objects have different vertex layouts, so putting them in the same buffer doesn't make a whole lot of sense (unless I rewrote my whole application to change vao's on the fly)



//
//  BufferFacilitator.cpp
//
//  Created by Forrest Miller on 5/21/18.
//

//This class is for abstracting away management of the vertex and element (and
//possibly other) buffers. Instead of dealing directly with it's own buffer, a
//drawable object registers itself with a BufferFacilitator object, which
//then handles drawing.
//This class will probably not be thread safe (at least initally)


#include "BufferFacilitator.h"

//Constants
constexpr int NOT_BOUND = -1;

//These 3 static variables are used to track which buffer is bound to prevent
//conflict if there are multiple instances of this class
int BufferFacilitator::NextBufferFacilitatorID = 0;
int BufferFacilitator::currentlyBoundVertBuffer = NOT_BOUND;
int BufferFacilitator::currentlyBoundElemBuffer = NOT_BOUND;


//Constructor
BufferFacilitator::BufferFacilitator() {
    this->bufferID = NextBufferFacilitatorID++;
    
}


//Destructor
BufferFacilitator::~BufferFacilitator() {
    //Unbind this buffer if it is bound
    if (this->isBound()) {
        currentlyBoundVertBuffer = NOT_BOUND;
        currentlyBoundElemBuffer = NOT_BOUND;
    }
}

//Member Functions

bool BufferFacilitator::isBound() {
    return ( (currentlyBoundVertBuffer == this->bufferID) &&
             (currentlyBoundElemBuffer == this->bufferID) );
}
    
//bool BufferFacilitator::bind() {
//
//}


/*
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
            
            //Note that sizeof() behaves differently between Stack and Heap memory
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numberOfVertices, vertices, GL_STREAM_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberOfElements, elements, GL_STREAM_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(stackVertices), stackVertices, GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stackElements), stackElements, GL_STATIC_DRAW);
            
*/
