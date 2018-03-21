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
public:
    aiVector3D offsetForLaunchpoint;
    
    float earlyThetaZ;
    Kinetic() = delete;
    Kinetic(int id) : WeaponInstance(id) {
        //this->velocityModelRelative = aiVector3D(0.0f, 0.0f, 0.0f);
        this->offsetForLaunchpoint = aiVector3D(0.0f, 0.0f, 0.0f);
        this->damage = KINETIC_WEP_DAMAGE;
        this->wepType = KINETIC;
        this->homing = false;
        //this->modelOffsetLaunchPoint = aiVector3D(0.0f, 0.0f, 0.0f);
    }
    
    ~Kinetic() {
        if (this->forward != nullptr) {
            delete this->forward;
            this->forward = nullptr;
        }
    }
};

#endif /* Kinetic_h */
