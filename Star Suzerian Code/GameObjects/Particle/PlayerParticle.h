//
//  PlayerParticle.h
//
//  Created by Forrest Miller on 5/28/18.
//


//NOTE: The default primitive for a player particle will be a point, so triangles
//      will need to be marked as triangles (which they should handle on their own)
//      This means a triangle can be made to act as a point primitive set at it's midpoint

#ifndef PLAYER_PARTICLE
#define PLAYER_PARTICLE

#include "Instance.h"

#include "MeshTriangle.h"

enum ParticlePrimitive {POINT, TRIANGLE};

class PlayerParticle : public Renderable {
public:
    ParticlePrimitive particlePrimitive;
    float zVel;
    float red;
    float green;
    float blue;
    
    PlayerParticle() = delete;
    PlayerParticle(float xPos, float yPos, float zPos, float xVel, float yVel, float zVel, float zoom = 1.0f) {
        this->position.x = xPos;
        this->position.y = yPos;
        this->position.z = zPos;
        
        this->velocity.x = xVel;
        this->velocity.y = yVel;
        this->zVel = zVel;
        
        initialize(zoom);
    }
    
    PlayerParticle(aiVector3D position, float xVel, float yVel, float zVel, float zoom = 1.0f) {
        this->position = position;
        
        this->velocity.x = xVel;
        this->velocity.y = yVel;
        this->zVel = zVel;
        
        initialize(zoom);
    }
    
    PlayerParticle(float xPos, float yPos, float zPos, aiVector2D velocity, float zVel, float zoom = 1.0f) {
        this->position.x = xPos;
        this->position.y = yPos;
        this->position.z = zPos;
        
        this->velocity = velocity;
        this->zVel = zVel;
        
        initialize(zoom);
    }
    
    PlayerParticle(aiVector3D position, aiVector2D velocity, float zVel, float zoom = 1.0f) {
        this->position = position;
        this->velocity = velocity;
        this->zVel = zVel;
        initialize(zoom);
    }
    
    virtual ~PlayerParticle() override;

    void setColor(float red, float green, float blue) {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }
    
    virtual int getNumberOfVerts() = 0; //To be overwritten by subclasses
    
private:
    void initialize(float zoom) {
        this->timeAlive = 0.0f;
        this->zoom = zoom;
        this->particlePrimitive = POINT;
        
        red = green = blue = 0.25f;
    }
    
};

#endif // PLAYER_PARTICLE
