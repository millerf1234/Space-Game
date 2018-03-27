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


constexpr int KINETIC_PROJECTILE_MODEL_VARIATIONS = 7;
static constexpr int KINETIC_PROJECTILE_VERSION0_VERTS_COUNT = 18*3;
static constexpr int KINETIC_PROJECTILE_VERSION1_VERTS_COUNT = 18*3;
static constexpr int KINETIC_PROJECTILE_VERSION2_VERTS_COUNT = 18*3;
static constexpr int KINETIC_PROJECTILE_VERSION3_VERTS_COUNT = 18*3;
static constexpr int KINETIC_PROJECTILE_VERSION4_VERTS_COUNT = 18*3;
static constexpr int KINETIC_PROJECTILE_VERSION5_VERTS_COUNT = 18*3;
static constexpr int KINETIC_PROJECTILE_VERSION6_VERTS_COUNT = 18*3;


//Square Pyrimid Top Half
static constexpr GLfloat KINETIC_PROJECTILE_VERSION0_VERTS[] = {
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

static constexpr GLfloat KINETIC_PROJECTILE_VERSION1_VERTS[] = {
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

static constexpr GLfloat KINETIC_PROJECTILE_VERSION2_VERTS[] = {
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

static constexpr GLfloat KINETIC_PROJECTILE_VERSION3_VERTS[] = {
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

static constexpr GLfloat KINETIC_PROJECTILE_VERSION4_VERTS[] = {
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

static constexpr GLfloat KINETIC_PROJECTILE_VERSION5_VERTS[] = {
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

static constexpr GLfloat KINETIC_PROJECTILE_VERSION6_VERTS[] = {
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
    
    int modelVersion;
   
    
    float earlyThetaZ;
    Kinetic() = delete;
    Kinetic(int id) : WeaponInstance(id) {
        //this->velocityModelRelative = aiVector3D(0.0f, 0.0f, 0.0f);
        this->offsetForLaunchpoint = aiVector3D(0.0f, 0.0f, 0.0f);
        this->damage = KINETIC_WEP_DAMAGE;
        this->wepType = KINETIC;
        this->homing = false;
        this->modelVersion = 0; //Default to model version 0
        //this->modelOffsetLaunchPoint = aiVector3D(0.0f, 0.0f, 0.0f);
    }
    
    Kinetic(int id, int modelVersion) : Kinetic(id) {
        
    }
    
    ~Kinetic() {
        if (this->forward != nullptr) {
            delete this->forward;
            this->forward = nullptr;
        }
    }
    
    int getNumberOfModelVariations() const { return KINETIC_PROJECTILE_MODEL_VARIATIONS;}
    int getModelVersion() const {return this->modelVersion;}
};

#endif /* Kinetic_h */
