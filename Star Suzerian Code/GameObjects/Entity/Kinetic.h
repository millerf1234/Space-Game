//
//  Kinetic.h
//
//  Created by Forrest Miller on 3/14/18.
//

#ifndef Kinetic_h
#define Kinetic_h

#include "GameParameters.h"
#include "Instance.h"
#include "Generator.h"


static constexpr GLfloat KINETIC_PROJECTILE_VERTS[] = {
    
    //Full
    //x        y        z
     1.0f,    0.0f,    1.0f, //Base
     1.0f,    0.0f,   -1.0f,
    -1.0f,    0.0f,   -1.0f,
     1.0f,    0.0f,    1.0f,
    -1.0f,    0.0f,   -1.0f,
    -1.0f,    0.0f,    1.0f, //end base
     1.0f,    0.0f,    1.0f, //vertical triangle 1
     0.0f,    2.5f,    0.0f,
     1.0f,    0.0f,   -1.0f,
     1.0f,    0.0f,   -1.0f, //vertical triangle 2
     0.0f,    2.5f,    0.0f,
    -1.0f,    0.0f,   -1.0f,
    -1.0f,    0.0f,   -1.0f, //Vertical triangle 3
     0.0f,    2.5f,    0.0f,
    -1.0f,    0.0f,    1.0f,
    -1.0f,    0.0f,    1.0f, //vertical triangle 4
     0.0f,    2.5f,    0.0f,
     1.0f,    0.0f,    1.0f,
    
    ////Half
    ////x      y     z
    //1.0,    0.0,   1.0, //Base
    //1.0,    0.0,  -1.0,
    //-1.0,   0.0,  -1.0,
    //1.0,    0.0,   1.0,
    //-1.0,   0.0,   -1.0,
    //-1.0,   0.0,   1.0, //end base
    //1.0,    0.0,   1.0, //vertical triangle 1
    //0.0,    2.5,   0.0,
    //1.0,    0.0,   -1.0,
    //1.0,    0.0,  -1.0, //vertical triangle 2
    //0.0,    2.5,   0.0,
    //-1.0,    0.0,   -1.0,
    //-1.0,   0.0,   -1.0, //Vertical triangle 3
    //0.0,    2.5,   0.0,
    //-1.0,   0.0,   1.0,
    //-1.0,    0.0,   1.0, //vertical triangle face 4
    //0.0,    2.5,   0.0,
    //1.0,    0.0,   1.0,
    
};


class Kinetic : public WeaponInstance {
    
};

#endif /* Kinetic_h */
