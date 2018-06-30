//
//  PlayerParticlePoint.h
//
//  Created by Forrest Miller on 5/29/18.
//

#ifndef PLAYER_PARTICLE_POINT
#define PLAYER_PARTICLE_POINT

#include <stdio.h>
#include "PlayerParticle.h"

static constexpr int POINT_VERTS = 1;
static constexpr int POINT_VERTEX_SIZE = 3;

class PlayerParticlePoint : public PlayerParticle {
public:
    //Constructors
    PlayerParticlePoint() = delete;
    
    PlayerParticlePoint(float xPos, float yPos, float zPos, float xVel, float yVel, float zVel, float zoom = 1.0f) :
    PlayerParticle(xPos, yPos, zPos, xVel, yVel, zVel, zoom)
    {
        this->particlePrimitive = ParticlePrimitive::POINT;
    }
    
    PlayerParticlePoint(aiVector3D position, float xVel, float yVel, float zVel,
                        float zoom = 1.0f) : PlayerParticle(position, xVel, yVel, zVel) {
        this->particlePrimitive = ParticlePrimitive::POINT;
    }
    
    PlayerParticlePoint(float xPos, float yPos, float zPos, aiVector2D velocity,
                        float zVel, float zoom = 1.0f) : PlayerParticle(xPos, yPos,
                                                                        zPos, velocity,
                                                                        zVel) {
        this->particlePrimitive = ParticlePrimitive::POINT;
    }
    
    PlayerParticlePoint(aiVector3D position, aiVector2D velocity, float zVel,
                        float zoom = 1.0f) : PlayerParticle(position, velocity, zVel) {
        this->particlePrimitive = ParticlePrimitive::POINT;
    }
    
    PlayerParticlePoint(aiVector3D position, aiVector3D velocity,
                        float zoom = 1.0f) : PlayerParticle(position, velocity.x,
                                                            velocity.y, velocity.z) {
        this->particlePrimitive = ParticlePrimitive::POINT;
    }
    
    virtual ~PlayerParticlePoint() override {;}
    
    
    virtual int getNumberOfVerts() override {
        return POINT_VERTS;
    }
    
    virtual int getVertSize() override {
        return POINT_VERTEX_SIZE;
    }
    
    
    
    
    
    
    
    
};

#endif // PLAYER_PARTICLE_POINT
