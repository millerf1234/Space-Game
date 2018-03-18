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


static constexpr int KINETIC_PROJECTILE_VERTS_COUNT = 18*3;

static constexpr GLfloat KINETIC_PROJECTILE_VERTS[] = {
    //Square Pyrimid
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
    
};


class Kinetic : public WeaponInstance {
    Kinetic() : WeaponInstance() {}
    Kinetic(aiVector3D wepSpawnOffset, aiVector2D midpoint, float instanceZoom, aiVector2D velocity) : WeaponInstance() {
        
    }
};

#endif /* Kinetic_h */
