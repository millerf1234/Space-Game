//
//  PlayerParticleManager.cpp
//
//  Created by Forrest Miller on 5/29/18.
//

#include "PlayerParticleManager.h"

static Quaternion rotation(0.0f, 0.0f, 1.0f, 0.0f);

PlayerParticleManager::PlayerParticleManager() {
    playerParticleShader = new ShaderWrapper;
    
    
    if (!initialize()) {
        hasValidShaders = false;
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::WARNING! An error was encountered while setting up\n";
            std::cout << "       shaders for Player Particles!\n";
        }
        return;
    }
    hasValidShaders = true;
}

PlayerParticleManager::~PlayerParticleManager() {
    if (playerParticleShader != nullptr) {
        delete playerParticleShader;
        playerParticleShader = nullptr;
    }
}


void PlayerParticleManager::doUpkeep() {
    std::vector<PlayerParticle*>::iterator iter = playerParticles.begin();
    for ( ; iter != playerParticles.end(); iter++) {
        (*iter)->position.x += (*iter)->velocity.x;
        (*iter)->position.y += (*iter)->velocity.y;
        (*iter)->position.z += (*iter)->zVel;
        (*iter)->timeAlive += TIME_TICK_RATE;
    }
}

void PlayerParticleManager::doUpkeep(float timeStep) {
    int steps = (int) (floor(timeStep / TIME_TICK_RATE));
    if (steps < 1) {steps = 1;}
    for (int i = 0; i < steps; i++) {
        std::vector<PlayerParticle*>::iterator iter = playerParticles.begin();
        for ( ; iter != playerParticles.end(); iter++) {
            (*iter)->position.x += (*iter)->velocity.x;
            (*iter)->position.y += (*iter)->velocity.y;
            (*iter)->position.z += (*iter)->zVel;
            (*iter)->timeAlive += TIME_TICK_RATE;
        }
    }
}

void PlayerParticleManager::drawInstances() {
    if (playerParticles.size() == 0) {
        return;
    }
    
    int size = this->computeVertBufferSize();
    if (size > 0) {
        //Tell OpenGL that the vertex shaders will manage setting point size
        glEnable(GL_PROGRAM_POINT_SIZE); //See OpenGL super bible page 248
        
        //Allocate memory to hold the vertex data for the particles
        GLfloat * particles = new GLfloat [size];
        
        //Put the particles into the array
        int arrayIndex = 0;
        std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
        for ( ; iter != playerParticles.end(); iter++) {
            if ( (*iter)->particlePrimitive == UNSPECIFIED) {
                continue;
            }
            else if ( (*iter)->particlePrimitive == POINT) {
                //place point into array
                particles[arrayIndex++] = (*iter)->position.x;
                particles[arrayIndex++] = (*iter)->position.y;
                particles[arrayIndex++] = (*iter)->position.z;
            }
            else if ( (*iter)->particlePrimitive == TRIANGLE) {
                //place triangle vertices into the array
                //worry about winding?
            }
        }
        
        //Reset iter to the first element in the vector
        iter = playerParticles.begin();
        
        //Draw the particles
        playerParticleShader->use();
        glBindBuffer(GL_ARRAY_BUFFER, shaderData.vbo);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, particles, GL_STREAM_DRAW);
        
        //Set the uniforms
        glUniform1f(shaderData.ulocTime, (*iter)->timeAlive);
        glUniform1f(shaderData.ulocRed, (*iter)->red);
        glUniform1f(shaderData.ulocGreen, (*iter)->green);
        glUniform1f(shaderData.ulocBlue, (*iter)->blue);
        glUniform1f(shaderData.ulocZoom, (*iter)->zoom);
        
        
        //glDrawArrays(GL_POINT, etc...);
        glDrawArrays(GL_POINTS, 0, size);
        
        
        delete [] particles;
    }
}


void PlayerParticleManager::particalizePlayer(PlayerEntity * player, SimpleObjLoader * modelData, bool parameterizeLines, unsigned int lineDivisionPoints, bool particalizeTriangles) {
    
    aiVector3D midpoint = player->position;
    
    std::cout << "\n\nPlayer's position is: " << player->position.x << " " << player->position.y << std::endl;
    
    aiVector3D velocity(0.0f, 0.01f * (0.01f / TIME_TICK_RATE), 0.0f);
    
    
    int subdivisions = 600;
    rotation.changeTheta(2.0f * PI / ((float)subdivisions));
    
    for (int i = 0; i < subdivisions; i++) {
        velocity = rotation.computeRotation(velocity);
        PlayerParticlePoint * ppp = new PlayerParticlePoint(midpoint.x, midpoint.y, midpoint.z, velocity.x, velocity.y, velocity.z, 75.0f);
        //PlayerParticlePoint * ppp = new PlayerParticlePoint(midpoint, velocity, 75.0f);
        playerParticles.push_back(ppp);
        
    }
    
}


bool PlayerParticleManager::initialize() {
    
    //Initialize the shader wrapper and OpenGL to allow for the drawing of player particles
    glGenBuffers(1, &shaderData.vbo);
    
    
    if (!playerParticleShader->attachVert((char *)PLAYER_PARTICLE_VERT.c_str()) ) {
        std::cout << "\nError attaching PlayerParticle vert shader!\n";
        return false;
    }
    if (!playerParticleShader->attachFrag((char *)PLAYER_PARTICLE_FRAG.c_str()) ) {
        std::cout << "\nError attaching PlayerParticle frag shader!\n";
        return false;
    }
    
    if (!playerParticleShader->link() ) {
        std::cout << "\nError Linking the PlayerParticle shader program!\n\n";
        return false;
    }
    
    playerParticleShader->setVertexAttribName("position");
    if (!playerParticleShader->specifyVertexLayout(ShaderWrapper::VERT3, shaderData.vbo)) {
        std::cout << "\nAn error occured while specifying the vertex layout for PlayerParticle!\n";
         return false;
    }
    
    shaderData.setUniformLocations(playerParticleShader->getID());
    return true;
}


//Code I started writing from when PlayerParticleManager inherited from EntityManager
/*
PlayerParticleManager::PlayerParticleManager() {
    this->initTemplate = new InitializationTemplate;
    this->generator = new Generator;
    this->hasGenerator = true;
    this->hasCollision = true;
    this->requiresAIInput = this->requiresUserInput = false;
    
    this->specType = PARTICLE;
    
    this->generateInitializationTemplate();
    
    this->initializeFromTemplate();
}


PlayerParticleManager::~PlayerParticleManager() {
    if (PRINT_DESTRUCTOR_CALLS) {
        std::cout << "\nDEBUG:PlayerParticleManager destructor called!";
    }
    if (this->initTemplate != nullptr) {
        if (this->initTemplate->hasVertsAlreadyLoaded) {
            //Then delete heap data that was used
            if (this->initTemplate->vertices != nullptr) {
                delete [] initTemplate->vertices;
                initTemplate->vertices = nullptr;
            }
            if (initTemplate->elements != nullptr) {
                delete [] initTemplate->elements;
                initTemplate->elements = nullptr;
            }
        }
        delete this->initTemplate;
        this->initTemplate = nullptr;
    }
    if (this->generator != nullptr) {
        delete this->generator;
        this->generator = nullptr;
    }
}
*/
