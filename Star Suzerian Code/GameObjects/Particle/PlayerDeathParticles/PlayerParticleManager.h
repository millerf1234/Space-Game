//
//  PlayerParticleManager.h
//
//  This class has the same roll as the other managers, except it is just managing
//  particles (i.e. Renderables) instead of managing Entities, so it doesn't share the
//  same inheritance structure as the other managers (great design, I know...)
//
//  Created by Forrest Miller on 5/29/18.
//

#ifndef PLAYER_PARTICLE_MANAGER
#define PLAYER_PARTICLE_MANAGER

#include <stdio.h>
#include "Quaternion.h"
#include "GameParameters.h"
#include "PlayerManager.h"
#include "PlayerParticlePoint.h"
#include "PlayerParticleExplosion.h"
#include "PlayerParticleTriangle.h"
#include "MathFunctions.h"
//#include "DeathAnimation.h"


typedef struct ShaderData {
    //GLuint vbo;
    GLint ulocTime, ulocRed, ulocGreen, ulocBlue, ulocZoom, ulocPointSize;
    //GLuint vboPoints, vboTriangles;
    
    ShaderData() {
        //vbo = 0u;
        ulocTime = -1;
        ulocRed = ulocGreen = ulocBlue = -1;
        ulocZoom = -1;
        ulocPointSize = -1;
    }
    
    void setUniformLocationsForPoint(GLuint shaderID);
    void setUniformLocationsForExplosion(GLuint shaderID);
    
} ShaderData;

class PlayerParticleManager {
private: //fields
    std::vector<PlayerParticle *> playerParticles;
    
    std::vector<PlayerParticlePoint *> playerPointParticles;
    std::vector<PlayerParticleExplosion *> playerExplosionParticles;
    std::vector<PlayerParticleTriangle *> playerTriangleParticles;
    
    ShaderWrapper * playerParticlePointShader;
    ShaderWrapper * playerParticleExplosionShader;
    
    bool hasValidPointParticleShader;
    bool hasValidExplosionParticleShader;
    bool hasValidTriangleParticleShader;
    
    ShaderData shaderDataPoint;
    ShaderData shaderDataExplosion;
    
public:
    PlayerParticleManager();
    
    ~PlayerParticleManager();
    
    void doUpkeep();
    void drawInstances();
    
    int getNumberOfParticles() const {return (int)playerParticles.size();}
    
    PlayerParticle ** getPlayerParticles() {
        return playerParticles.data();
    }
    
    void particalizePlayer(PlayerEntity * player, SimpleObjLoader * modelData, bool parameterizeLines, unsigned int lineDivisionPoints, bool particalizeTriangles);
    
    void deleteAllPlayerParticles();
    
    void markPlayerParticlesThatAreBeyondScreenLimitsForDeletion();
    
    Quaternion * getTheExplosionRotationQuaternionSoThatItCanBeMessedWith() const;
    void resetTheExplosionRotationQuaternionAfterItHasBeenMessedWith();
    
    void translateParticles(aiVector2D translation);
    
private: //Private Member Functions
    bool initializeForDrawingPoint();
    bool initializeForDrawingExplosion();
    bool initializeForDrawingTriangle();
    
    void destroyParticlesFlaggedForDestruction();
    
    int computeVertBufferSize();
    
    void fillParticleVectors();
    
    //The constructVertexBuffer... function is to allow for only 1 call to glBufferData (instead of having to call it multiple times per frame)
    void constructVertexBufferFromParticleVectors(GLfloat *, int&, int&);
    
    void parameterizeModelIntoOrderedVertexListOfTriangles(SimpleObjLoader * modelData,
                                                           std::vector<float> & vertices);
    
    void placePlayerPointParticlesAlongTriangleEdges(PlayerEntity * player,
                                                     int lineDivisionPoints,
                                                     std::vector<PlayerParticle *> & playerParticles,
                                                    float * orderedModelTrianglePositionData,
                                                     int trianglePositionDataArraySize);
    
    
    //void generateExplosionParticles(const aiVector3D&);
    //void spawnExplosionParticle(const aiVector3D&, const aiVector3D&, float magnitude);
    
    
    void drawPointParticles(int);
    void drawExplosionParticles(int, int);
    void drawTriangleParticles(int, int);
    
    
    
    
};



//Class I started writing from when PlayerParticleManager inherited from EntityManager
/*
class PlayerParticleManager : public GameEntityManager {
private:
    std::vector<PlayerParticle *> playerParticles;
    
public:
    PlayerParticleManager();
    virtual ~PlayerParticleManager() override;
    
    virtual void doUpkeep() override;
    virtual void drawInstances() override;
    
    PlayerParticle ** getPlayerParticles() {
        return playerParticles.data();
    }
    
    void particalizePlayer(PlayerEntity * player, SimpleObjLoader * modelData);
    
protected:
    virtual void initializeFromTemplate() override;
private:
    void generateInitializationTemplate();
    
    
};

*/

#endif // PLAYER_PARTICLE_MANAGER
