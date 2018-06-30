//
//  PlayerParticleTriangle.h
//
//  Created by Forrest Miller on 5/29/18.
//

#ifndef PLAYER_PARTICLE_TRIANGLE
#define PLAYER_PARTICLE_TRIANGLE

#include <stdio.h>
#include "PlayerParticle.h"

enum class vertexWinding{CLOCKWISE, COUNTER_CLOCKWISE, UNSPECIFIED_WINDING};

static constexpr int TRIANGLE_VERTS = 3;
static constexpr int TRIANGLE_VERTEX_SIZE = 3;

class PlayerParticleTriangle : public PlayerParticle {
public:
    PlayerParticleTriangle(float xPos, float yPos, float zPos, float xVel, float yVel, float zVel, float zoom = 1.0f) :
    PlayerParticle(xPos, yPos, zPos, xVel, yVel, zVel, zoom)
    {
        this->particlePrimitive = ParticlePrimitive::TRIANGLE;
    }
    virtual ~PlayerParticleTriangle() override {;}
    
    virtual int getNumberOfVerts() override {
        return TRIANGLE_VERTS;
    }
    
    virtual int getVertSize() override {
        return TRIANGLE_VERTEX_SIZE;
    }
    
//    
//    Quaternion * rollAxis; //To make triangle roll
//    float angularVelocity; //The step to change rollAxis by 
//    
//private:
//    aiVector3D offsets[3];
//    
//    
};


#endif // PLAYER_PARTICLE_TRIANGLE
