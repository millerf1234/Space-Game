//
//  PlayerParticleExplosion.h
//
//  This class is very similar to PlayerParticlePoint, the difference being this
//  class does not have collision (I plan to implement collision on player particle points)
//  and this class will be drawn the color of an explosion.
//
//  Created by Forrest Miller on 5/30/18.
//

#ifndef PLAYER_PARTICLE_EXPLOSION
#define PLAYER_PARTICLE_EXPLOSION

//static constexpr int POINT_VERTS = 1;  //Take these from PlayerParticlePoint
//static constexpr int POINT_VERTEX_SIZE = 3;


class PlayerParticleExplosion : public PlayerParticle {
    
public:
    //Constructors
    PlayerParticleExplosion() = delete;
    
    PlayerParticleExplosion(float xPos, float yPos, float zPos, float xVel, float yVel, float zVel, float zoom = 1.0f) :
    PlayerParticle(xPos, yPos, zPos, xVel, yVel, zVel, zoom)
    {
        this->particlePrimitive = ParticlePrimitive::EXPLOSION;
    }
    
    PlayerParticleExplosion(aiVector3D position, float xVel, float yVel, float zVel,
                        float zoom = 1.0f) : PlayerParticle(position, xVel, yVel, zVel) {
        this->particlePrimitive = ParticlePrimitive::EXPLOSION;
    }
    
    PlayerParticleExplosion(float xPos, float yPos, float zPos, aiVector2D velocity,
                        float zVel, float zoom = 1.0f) : PlayerParticle(xPos, yPos,
                                                                        zPos, velocity,
                                                                        zVel) {
        this->particlePrimitive = ParticlePrimitive::EXPLOSION;
    }
    
    PlayerParticleExplosion(aiVector3D position, aiVector2D velocity, float zVel,
                        float zoom = 1.0f) : PlayerParticle(position, velocity, zVel) {
        this->particlePrimitive = ParticlePrimitive::EXPLOSION;
    }
    
    PlayerParticleExplosion(aiVector3D position, aiVector3D velocity,
                        float zoom = 1.0f) : PlayerParticle(position, velocity.x,
                                                            velocity.y, velocity.z) {
        this->particlePrimitive = ParticlePrimitive::EXPLOSION;
    }
    
    virtual ~PlayerParticleExplosion() override {;}
    
    
    virtual int getNumberOfVerts() override {
        return POINT_VERTS;
    }
    
    virtual int getVertSize() override {
        return POINT_VERTEX_SIZE;
    }
    
};


#endif // PLAYER_PARTICLE_EXPLOSION
