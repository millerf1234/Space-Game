//Oh wait, I just remembered that different objects have different vertex layouts, so putting them in the same buffer doesn't make a whole lot of sense (unless I rewrote my whole application to change vao's on the fly)


//
//  BufferFacilitator.h
//
//  Created by Forrest Miller on 5/21/18.
//

#pragma once

#ifndef BUFFER_FACILITATOR
#define BUFFER_FACILITATOR

#include <vector>
#include "GameParameters.h"
#include "SimpleObjLoader.h"
#include "glad.h"

enum bufferType {vertex, element, uniform, other};

class BufferFacilitator {
    //There are 3 static variables that are used to handle multiple
    static int NextBufferFacilitatorID; //The ID to assign the next constructed buffer
    static int currentlyBoundVertBuffer; //The ID of the buffer currently bound
    static int currentlyBoundElemBuffer; //The ID of the buffer currently bound 
    
private:
    int bufferID; //This objects BufferFacilitatorID Number
    bufferType buffType;
    std::vector<GLfloat> vertexData;
    std::vector<GLuint> elementData;
    
    std::vector<int> elementDataOffsets;
    std::vector<int> elementDataSizes;
    
    
    
public:
    //Constructors
    BufferFacilitator();
    
    //Destructor
    ~BufferFacilitator();
    
    bool isBound();
    
    bool bind();
    
    bool addData(SimpleObjLoader obj, int & bufPositionID);
    bool addData(GLfloat * vertData, int vertDataSize, GLuint * elemData,
                 int elemDataSize, int & bufPositionID);
    
    bool bufferData();
    
    //Call these two functions when drawing
    int getBufferOffset(int bufPositionID);
    int getBufferedDataSize(int bufPositionID);
    
    
    
};


#endif // BUFFER_FACILITATOR
