//
//  Stage.h
//
//  Created by Forrest Miller on 2/17/18.
//

#ifndef Stage_h
#define Stage_h

#include <stdio.h>
#include "Generator.h"
#include "GameParameters.h" //Contains filepath information


static constexpr int STAGE_VERTS_SIZE = 4*(2+3+2);
static constexpr GLfloat STAGE_VERTS[] = {
    // Position                     Color                      Texture (U/V)
    -1.0f,  1.0f,            0.0f,   0.0f,   0.0f,            0.0f, 0.0f, //  Top-left corner   //0
     1.0f,  1.0f,            0.0f,   0.00f,   0.0f,            1.0f, 0.0f, //  Top-right corner  //1
     1.0f, -1.0f,            0.0f,   0.0f,   0.0f,            1.0f, 1.0f, // Lower-right corner //2
    -1.0f, -1.0f,            0.0f,   0.0f,   0.0f,            0.0f, 1.0f  // Lower-left corner  //3
};

static constexpr int STAGE_ELEMENTCOUNT = 2*3;
static constexpr GLuint STAGE_ELEMENTS[] {  //               t1 ___    /|
    0, 1, 2, //Triangle 1 (top-left, top-right, lower-right)   |  /   / |     (Oops these are not right)
    2, 3, 0  //Triangle 2 (lower-right, lower-left, top-left)  | /   /__| t2
}; //                                                          |/

class Stage {
private:
    //A pointer to a struct of information on how to initialize this object
    InitializationTemplate * stageInitFormat; //Contains information on how a stage should be constructed
    
    //void setStageInitFormat(); //Function that generates the stageInitFormat
    void setStageInitFormat(int textureToLoadInParameterArray);
public:
    //Fields
    Generator * generator;
    
    //Constructor
    Stage();
    Stage(int textureToLoadInParameterArray);
    //Destructor
    ~Stage();
    
    
    //void drawInstance(); //Just have the generator worry about drawing its instances
    
    
    
};

#endif /* Stage_h */
