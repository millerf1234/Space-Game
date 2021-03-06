//  This class manages all the particle types associated with player particle
//  effects
//
//  PlayerParticleManager.cpp
//
//  Created by Forrest Miller on 5/29/18.
//

#include "PlayerParticleManager.h"


static Quaternion rotation(0.0f, 0.0f, 1.0f, (8.0f * PI / PLAYER_EXPLOSION_PARTICLES_PER_WAVE));
static Quaternion rotationForOvalPattern(0.0f, 0.0f, 1.0f, PI/4.0f); //Hmm not sure if I really want this...

static GLuint particleVBO = 0u;

//-----------------------------------------------------------------------------
//    struct ShaderData function implementation
//-----------------------------------------------------------------------------

static std::string MSG_ABOUT_MISSING_UNIFORM_LOCATION =
"(NOTE THAT THIS IS EXPECTED BEHAVIOR IF THE UNIFORM IS NOT\nUSED IN THE SHADER!)\n";

void ShaderData::setUniformLocationsForPoint(GLuint shaderID) {
    ulocTime = glGetUniformLocation(shaderID, "time");
    if (ulocTime == -1) {
        std::cout << "\nError Setting Uniform Location for 'time'";
        std::cout << " in PlayerParticle shader (Point)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
    ulocZoom = glGetUniformLocation(shaderID, "zoom");
    if (ulocZoom == -1) {
        std::cout << "\nError Setting Uniform Location for 'zoom'";
        std::cout << " in PlayerParticle shader (Point)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
    ulocRed = glGetUniformLocation(shaderID, "red");
    if (ulocRed == -1) {
        std::cout << "\nError Setting Uniform Location for 'red'";
        std::cout << " in PlayerParticle shader (Point)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
    ulocGreen = glGetUniformLocation(shaderID, "green");
    if (ulocGreen == -1) {
        std::cout << "\nError Setting Uniform Location for 'green'";
        std::cout << " in PlayerParticle shader (Point)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
    ulocBlue = glGetUniformLocation(shaderID, "blue");
    if (ulocBlue == -1) {
        std::cout << "\nError Setting Uniform Location for 'blue'";
        std::cout << " in PlayerParticle shader (Point)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
    
    ulocPointSize = glGetUniformLocation(shaderID, "pointSize");
    if (ulocPointSize == -1) {
        std::cout << "\nError Setting Uniform Location for 'pointSize'";
        std::cout << " in PlayerParticle shader (Point)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
}

void ShaderData::setUniformLocationsForExplosion(GLuint shaderID) {
    ulocTime = glGetUniformLocation(shaderID, "time");
    if (ulocTime == -1) {
        std::cout << "\nError Setting Uniform Location for 'time'";
        std::cout << " in PlayerParticle shader (Explosion)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
    ulocZoom = glGetUniformLocation(shaderID, "zoom");
    if (ulocZoom == -1) {
        std::cout << "\nError Setting Uniform Location for 'zoom'";
        std::cout << " in PlayerParticle shader (Explosion)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
    ulocPointSize = glGetUniformLocation(shaderID, "pointSize");
    if (ulocPointSize == -1) {
        std::cout << "\nError Setting Uniform Location for 'pointSize'";
        std::cout << " in PlayerParticle shader (Explosion)!\n";
        std::cout << MSG_ABOUT_MISSING_UNIFORM_LOCATION;
    }
}

//-----------------------------------------------------------------------------
//    PlayerParticleManager Function Implementations:
//-----------------------------------------------------------------------------

PlayerParticleManager::PlayerParticleManager() {
    playerParticlePointShader = new ShaderWrapper;
    playerParticleExplosionShader = new ShaderWrapper;
    
    glGenBuffers(1, &particleVBO);
    
    if (!initializeForDrawingPoint()) {
        hasValidPointParticleShader = false;
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::WARNING! An error was encountered while setting up\n";
            std::cout << "       shaders for Player Point Particles!\n";
        }
        //return;
    }
    else {
        hasValidPointParticleShader = true;
    }
    
    if (!initializeForDrawingExplosion()) {
        hasValidExplosionParticleShader = false;
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::WARNING! An error was encountered while setting up\n";
            std::cout << "       shaders for Player Explosion Particles!\n";
        }
    }
    else {
        hasValidExplosionParticleShader = true;
    }
    
    //if (!initializeForDrawingTriangles()) {
    //  Same as above....
    //}
    hasValidTriangleParticleShader = false;
    
}

PlayerParticleManager::~PlayerParticleManager() {
    if (playerParticlePointShader != nullptr) {
        delete playerParticlePointShader;
        playerParticlePointShader = nullptr;
    }
    
    if (playerParticleExplosionShader != nullptr) {
        delete playerParticleExplosionShader;
        playerParticleExplosionShader = nullptr;
    }
}

void PlayerParticleManager::doUpkeep() {
    if (playerParticles.size() == 0) {
        return;
    }
    std::vector<PlayerParticle*>::iterator iter = playerParticles.begin();
    for ( ; iter != playerParticles.end(); iter++) {
        //(*iter)->velocity.x += MathFunc::getRandomInRange(-0.11f, 0.11f); //Adding this slows the loop way down. Probably changes how the compiler is able to optimize
        (*iter)->position.x += (*iter)->velocity.x;
        (*iter)->position.y += (*iter)->velocity.y;
        (*iter)->position.z += (*iter)->zVel;
        (*iter)->timeAlive += TIME_TICK_RATE;
        
        //Mark the explosion objects for desctruction
        if ( (*iter)->particlePrimitive == ParticlePrimitive::EXPLOSION ) {
            if ( (((*iter)->timeAlive) / TIME_TICK_RATE) >= FRAMES_TO_KEEP_PLAYER_EXPLOSION_AROUND) {
                (*iter)->flagForDestruction();
            }
        }
    }
    markPlayerParticlesThatAreBeyondScreenLimitsForDeletion();
    destroyParticlesFlaggedForDestruction();
}

void PlayerParticleManager::drawInstances() {
    if (playerParticles.size() == 0) {
        return;
    }
    
    //clear the vectors of the specific particle types
    playerPointParticles.clear();
    playerExplosionParticles.clear();
    playerTriangleParticles.clear();
    //Since I don't track the number of each particle type currently, each of these 3 vectors will be oversized...
    //playerPointParticles.reserve(playerParticles.size());//Reserve won't affect what vector.size() returns
    //playerExplosionParticles.reserve(playerParticles.size());
    //playerTriangleParticles.reserve(playerParticles.size());
    fillParticleVectors();
    
    int size = this->computeVertBufferSize();
    if (size > 0) {
        //Tell OpenGL that the vertex shaders will manage setting point size
        glEnable(GL_PROGRAM_POINT_SIZE); //See OpenGL super bible page 248
        
        //Allocate memory to hold the vertex data for all the particles
        GLfloat * particles = new GLfloat [size];
        
        int explosionBufferOffset = NOT_SET;
        int triangleBufferOffset = NOT_SET;
        constructVertexBufferFromParticleVectors(particles, explosionBufferOffset, triangleBufferOffset);
        
        
        if (explosionBufferOffset == NOT_SET) { //I am not sure if this is the best thing to do here... (returning without drawing...)
            std::cout << "\nDEBUG::Error drawing particles\n";
            return;
        }
        
        if (particleVBO == 0u) {
            std::cout << "\nOOPS! An error occured setting the particle VBO!\n";
            return;
        }
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, particles, GL_STREAM_DRAW);
        
        //Not sure if i need the "-1" in calculation...
        if (hasValidPointParticleShader) {
            //drawPointParticles( (explosionBufferOffset - 1) / POINT_VERTEX_SIZE);
            drawPointParticles( (explosionBufferOffset) / POINT_VERTEX_SIZE);
            if (PRINT_PLAYER_PARTICLES_DRAW_COUNTS) {
                std::cout << "\nDrawing " << ((explosionBufferOffset - 1) / POINT_VERTEX_SIZE);
                std::cout << " Player Point Particles!\n";
            }
        }
        if (hasValidExplosionParticleShader) {
            //drawExplosionParticles(explosionBufferOffset, (triangleBufferOffset - 1) / 3);
            drawExplosionParticles(explosionBufferOffset, (triangleBufferOffset) / 3);
            if (PRINT_PLAYER_PARTICLES_DRAW_COUNTS) {
                std::cout << "\nDrawing " << (triangleBufferOffset - 1) / POINT_VERTEX_SIZE;
                std::cout << " Player Explosion Particles!\n";
            }
        }
        
        if (hasValidTriangleParticleShader) {
            drawTriangleParticles(triangleBufferOffset, (triangleBufferOffset - 1) / 3);
            if (PRINT_PLAYER_PARTICLES_DRAW_COUNTS) {
                std::cout << "\nDrawing " << (triangleBufferOffset - 1) / 3;
                std::cout << " Player Triangle Particles!\n";
            }
        }
        
        //        //Put the particles into the array
        //        int arrayIndex = 0;
        //        std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
        //        for ( ; iter != playerParticles.end(); iter++) {
        //            if ( (*iter)->particlePrimitive == ParticlePrimitive::UNSPECIFIED) {
        //                continue;
        //            }
        //            else if ( (*iter)->particlePrimitive == ParticlePrimitive::POINT) {
        //                //place point into array
        //                particles[arrayIndex++] = (*iter)->position.x;
        //                particles[arrayIndex++] = (*iter)->position.y;
        //                particles[arrayIndex++] = (*iter)->position.z;
        //            }
        //            else if ( (*iter)->particlePrimitive == ParticlePrimitive::EXPLOSION) {
        //                //place point into array
        //                particles[arrayIndex++] = (*iter)->position.x;
        //                particles[arrayIndex++] = (*iter)->position.y;
        //                particles[arrayIndex++] = (*iter)->position.z;
        //            }
        //            else if ( (*iter)->particlePrimitive == ParticlePrimitive::TRIANGLE) {
        //                //place triangle vertices into the array
        //                //worry about winding?
        //            }
        //        }
        //
        //        //Reset iter to the first element in the vector
        //        iter = playerParticles.begin();
        //
        //        //Draw the particles
        //        playerParticlePointShader->use();
        //        glBindBuffer(GL_ARRAY_BUFFER, shaderData.vbo);
        //
        //        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, particles, GL_STREAM_DRAW);
        //
        //        static float pointSize = 1.0f;
        //        static float counter = 0.0f;
        //        counter = counter + 3.14159f / 45.0f;
        //        pointSize += 0.2f*(float)sin(counter);
        //
        //        //Set the uniforms
        //        glUniform1f(shaderData.ulocTime, (*iter)->timeAlive);
        //        glUniform1f(shaderData.ulocRed, (*iter)->red);
        //        glUniform1f(shaderData.ulocGreen, (*iter)->green);
        //        glUniform1f(shaderData.ulocBlue, (*iter)->blue);
        //        glUniform1f(shaderData.ulocZoom, (*iter)->zoom);
        //        glUniform1f(shaderData.ulocPointSize, pointSize);
        //
        //
        //        //glDrawArrays(GL_POINT, etc...);
        //        glDrawArrays(GL_POINTS, 0, size);
        
        
        delete [] particles;
    }
}

void PlayerParticleManager::createExplosion(PlayerEntity * player) {
    aiVector3D midpoint = player->position;
    aiVector3D velocity(0.0f, 0.6f * (0.01f / TIME_TICK_RATE), 0.0f);
    generateExplosionParticles(midpoint, velocity);
}

void PlayerParticleManager::particalizePlayer(PlayerEntity * player, SimpleObjLoader * modelData, bool parameterizeLines, unsigned int lineDivisionPoints, bool particalizeTriangles) {
    
    if (parameterizeLines && false) { ///This never worked properly, so it is deactivated by doing the "&& false"
        std::vector<float> orderedPositions;
        //std::vector<float> * orderedPositionsPtr = &orderedPositions;
        parameterizeModelIntoOrderedVertexListOfTriangles(modelData, orderedPositions);
       // std::cout << "\nOrdered Positions size is: " << orderedPositions.size() << "\n";
        //orderedPositionsPtr = &orderedPositions;
       // placePlayerPointParticlesAlongTriangleEdges(player, lineDivisionPoints, playerParticles, orderedPositionsPtr);
        placePlayerPointParticlesAlongTriangleEdges(player, lineDivisionPoints, playerParticles, orderedPositions.data(), (int)orderedPositions.size());
    }
    
    aiVector3D midpoint = player->position;
    
    //    std::cout << "\n\nPlayer's position is: " << player->position.x << " " << player->position.y << std::endl;
    
    
    
    aiVector3D velocity(0.0f, 0.6f * (0.01f / TIME_TICK_RATE), 0.0f);
    //aiVector3D randomlyScaledVelocity;
    
    int subdivisions = PLAYER_EXPLOSION_PARTICLES_PER_WAVE; //60;
    
    //rotation.changeTheta(2.0f * PI / ((float)subdivisions));
    
//    static bool modify = false;
//
//    float velocityMultiple1 = 0.856f;  //0.856f;
//    float velocityMultiple2 = 0.651f;  //0.651f;
//    float velocityMultiple3 = 0.52f;   // 0.52f;
//    float velocityMultiple4 = 0.40f;   //0.40f;
//    float velocityMultiple5 = 0.299f;  //0.299f;
    
   // Quaternion randomExtraRotation(0.0f, 0.0f, 1.0f, MathFunc::getRandomInRange(0.0f, PI / 2.0f));
    
    
    ////////////////////////////////////////////////////////////////////////////
    ///     EXPLOSION VARIATION PARAMETERS     (THIS FACILITATES A RANDOM EXPLOSION EFFECT)
    ////////////////////////////////////////////////////////////////////////////
    //Variables
    static constexpr int NUMBER_OF_EFFECTS = 7;
    int effectToDo = MathFunc::getRandomInRange(1, (NUMBER_OF_EFFECTS + 1)); //The interval here will include 1 and will exclude (NUMBER_OF_EFFECTS + 1)
    float effectMin, effectMax;
    //effectToDo = 5; //Do this to hardcode in a single effect (Effect 5 by itself is pretty cool)
    //Logic to set up effect
    if (effectToDo == 1) {
        effectMin = 0.05f;
        effectMax = 0.285f;
    }
    else if (effectToDo == 2) {
        effectMin = 0.65f;
        effectMax = 11.1f;
    }
    else if (effectToDo == 3) {
        effectMin = 0.85f;
        effectMax = 1.275f;
    }
    else if (effectToDo == 4) {
        effectMin = 1.65f;
        effectMax = 1.85f;
    }
//    else if (effectToDo == 5) {
//        effectMin = 3.0f;
//        effectMax = 5.0f;
//    }
    else if (effectToDo == 5) {
        effectMax = rotation.getTheta();
        effectMin = cos(effectMax) + 0.35f * sin(effectMax) + 0.25f;
        effectMax = 1.55f * effectMin;
    }
    else if (effectToDo == 6) {
        float temp = rotation.getTheta();
        effectMin = sqrt((abs(sin(temp)) + abs(cos(temp)) * 2.0f));
        effectMax = sqrt(floor(sqrt((abs(pow(sin(temp + cos(effectMin)), 6.0f)))))) + 1.30f * effectMin * sin(effectMin);
        
        effectMin = MathFunc::getRandomInRange(0.5f, 0.6f) * effectMin;
        effectMax = MathFunc::getRandomInRange(0.70f, 0.75) * effectMax;
    }
    else {
        effectMin = effectMax = 2.8f;
    }
    ////////////////////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////////////////////
    ///   ACTUAL CODE FOR THE EXPLOSION
    ////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < subdivisions; i++) {
        
        float randomScalar = MathFunc::getRandomInRange(effectMin, effectMax); //(0.01f, 0.95f); //(0.65f, 11.1f);     //(0.85f, 10.55f);
        //std::cout << "\nRandom is: " << randomScalar;
        
        
        
        
        //rotation.changeTheta(MathFunc::getRandomInRange(2.0f*PI / ((float)subdivisions), 3.14159f));
        velocity = rotation.computeRotation(velocity);
        
        //        randomlyScaledVelocity.x = velocity.x * MathFunc::getRandomInRange(0.01f, 0.95f);
        //        randomlyScaledVelocity.y = velocity.y * MathFunc::getRandomInRange(0.01, 0.95f);
        
        //velocity = velocity * randVel;
        //PlayerParticleExplosion * ppp = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, randomlyScaledVelocity.x, randomlyScaledVelocity.y, velocity.z, 75.0f);
        
        //////////////////////////////////////////////////////////////////////////////////////////
        ///   Explosion Particle Creation
        //////////////////////////////////////////////////////////////////////////////////////////
        ///Right
        //PlayerParticleExplosion * pppRight = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, velocity.x + randomScalar, velocity.y * randomScalar + MathFunc::getRandomInRange(0.2f, 0.4f) * randomScalar, velocity.z, 75.0f);
        PlayerParticleExplosion * pppRight = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, velocity.x + randomScalar, velocity.y * randomScalar, velocity.z, 75.0f);
        
        playerParticles.push_back(pppRight);
        
        ///Left
        PlayerParticleExplosion * pppLeft = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, velocity.x - randomScalar, velocity.y * randomScalar, velocity.z, 75.0f);
        //PlayerParticlePoint * ppp = new PlayerParticlePoint(midpoint, velocity, 75.0f);
        playerParticles.push_back(pppLeft);
        
        ///Up
        PlayerParticleExplosion * pppUp = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, velocity.x * randomScalar, velocity.y + randomScalar, velocity.z, 75.0f);
        playerParticles.push_back(pppUp);
        
        ///Down
        PlayerParticleExplosion * pppDown = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, velocity.x * randomScalar, velocity.y - randomScalar, velocity.z, 75.0f);
        playerParticles.push_back(pppDown);
        
        ///UpRight
        PlayerParticleExplosion * pppUpRight = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, (velocity.x * randomScalar) + randomScalar, (velocity.y * randomScalar) + randomScalar, velocity.z, 75.0f);
        playerParticles.push_back(pppUpRight);
        
        ///DownRight
        PlayerParticleExplosion * pppDownRight = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, (velocity.x * randomScalar) + randomScalar, (velocity.y * randomScalar) - randomScalar, velocity.z, 75.0f);
        playerParticles.push_back(pppDownRight);
        
        ///UpLeft
        PlayerParticleExplosion * pppUpLeft = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, (velocity.x * randomScalar) - randomScalar, (velocity.y * randomScalar) + randomScalar, velocity.z, 75.0f);
        playerParticles.push_back(pppUpLeft);
        
        ///DownLeft
        PlayerParticleExplosion * pppDownLeft = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, (velocity.x * randomScalar) - randomScalar, (velocity.y * randomScalar) - randomScalar, velocity.z, 75.0f);
        playerParticles.push_back(pppDownLeft);
        
        ///Everywhere  (cuz why not?)
        PlayerParticleExplosion * pppAll = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, velocity.x * randomScalar, velocity.y * randomScalar, velocity.z, 75.0f);
        playerParticles.push_back(pppAll);
        //////////////////////////////////////////////////////////////////////////////////////////
        
//        //Make explosion rotate sorta (turn down number of particles if doing this)
//        std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
//        for ( ; iter != playerParticles.end(); iter++) {
//            aiVector3D particleVelocity((*iter)->velocity.x, (*iter)->velocity.y, 0.0f);
//            particleVelocity = randomExtraRotation.computeRotation(particleVelocity);
//            (*iter)->velocity = aiVector2D(particleVelocity.x, particleVelocity.y);
//        }
        
        /*  //Need to uncomment a '}' down below if going to undo this comment
        if (true) {
            //            float velocityMultiple1 = 0.856f;  //0.856f;
            //            float velocityMultiple2 = 0.651f;  //0.651f;
            //            float velocityMultiple3 = 0.52f;   // 0.52f;
            //            float velocityMultiple4 = 0.40f;   //0.40f;
            //            float velocityMultiple5 = 0.299f;  //0.299f;
            
            if (modify) {
                float velocityIncrease = 1.19f;
                velocityMultiple1 *= velocityIncrease; // *= 1.05f;
                velocityMultiple2 *= velocityIncrease; // *= 1.05f;
                velocityMultiple3 *= velocityIncrease; // *= 1.05f;
                velocityMultiple4 *= velocityIncrease; // *= 1.05f;
                velocityMultiple5 *= velocityIncrease; // *= 1.05f;
                modify = false;
            }
            else {
                modify = true;
            }
            
            
            //Multiple 1
            PlayerParticleExplosion * ppp = new PlayerParticleExplosion(midpoint, velocityMultiple1 * velocity.x * randomScaler, velocityMultiple1 * velocity.y * randomScaler, velocity.z);
            playerParticles.push_back(ppp);
            
            //Multiple 2
            ppp = new PlayerParticleExplosion(midpoint, velocityMultiple2 * velocity.x * randomScaler, velocityMultiple2 * velocity.y * randomScaler, velocity.z, 75.0f);
            playerParticles.push_back(ppp);
            
            //Multiple 3
            ppp = new PlayerParticleExplosion(midpoint, velocityMultiple3 * velocity.x * randomScaler, velocityMultiple3 * velocity.y * randomScaler, velocity.z);
            playerParticles.push_back(ppp);
            
            //Multiple 4
            ppp = new PlayerParticleExplosion(midpoint, velocityMultiple4 * velocity.x * randomScaler, velocityMultiple4 * velocity.y * randomScaler, velocity.z);
            playerParticles.push_back(ppp);
             
            //Multiple 5
            ppp = new PlayerParticleExplosion(midpoint, velocityMultiple5 * velocity.x * randomScaler, velocityMultiple5 * velocity.y * randomScaler, velocity.z);
            playerParticles.push_back(ppp);
             */
            /*
             //Ovals 1
             ppp = new PlayerParticleExplosion(midpoint, velocityMultiple3 * velocity.y, velocityMultiple1 * velocity.x, velocity.z);
             playerParticles.push_back(ppp);
             ppp = new PlayerParticleExplosion(midpoint, velocityMultiple1 * velocity.x, velocityMultiple3 * velocity.y, velocity.z);
             playerParticles.push_back(ppp);
             
             aiVector3D rotatedVelocity = rotationForOvalPattern.computeRotation(velocity);
             ppp = new PlayerParticleExplosion(midpoint, velocityMultiple3 * rotatedVelocity.y, velocityMultiple1 * rotatedVelocity.x, 0.0f);
             playerParticles.push_back(ppp);
             ppp = new PlayerParticleExplosion(midpoint, velocityMultiple1 * rotatedVelocity.x, velocityMultiple3 * rotatedVelocity.y, 0.0f);
             playerParticles.push_back(ppp);
             
             */
            
            
            //            ppp = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, 0.5f * velocity.y, 0.5 * velocity.x, velocity.z);
            //            playerParticles.push_back(ppp);
            //
            //            ppp = new PlayerParticleExplosion(midpoint, 0.35f * velocity.y, 0.35f * velocity.x, velocity.z);
            //            playerParticles.push_back(ppp);
            //
            //            ppp = new PlayerParticleExplosion(midpoint, 0.25f * velocity.y, 0.25f * velocity.x, velocity.z);
            //            playerParticles.push_back(ppp);
            
            
        //}
        
    }
    
}

Quaternion * PlayerParticleManager::getTheExplosionRotationQuaternionSoThatItCanBeMessedWith() const {
    return &rotation;
}

void PlayerParticleManager::resetTheExplosionRotationQuaternionAfterItHasBeenMessedWith() {
    rotation.changeTheta( (2.0f * PI) / PLAYER_EXPLOSION_PARTICLES_PER_WAVE);
}


void PlayerParticleManager::deleteAllPlayerParticles() {
    if (this->getNumberOfParticles() < 1) {
        return;
    }
    std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
    for ( ; iter != playerParticles.end(); iter++) {
        (*iter)->flagForDestruction();
    }
    
    destroyParticlesFlaggedForDestruction();
}

void PlayerParticleManager::markPlayerParticlesThatAreBeyondScreenLimitsForDeletion() {
    std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
    for ( ; iter != playerParticles.end(); iter++) {
        if ( abs((*iter)->position.x) > (2.0f * XLIMIT)) {
            (*iter)->flagForDestruction();
        }
        else if (abs((*iter)->position.y) > 2.0f * YLIMIT) {
            (*iter)->flagForDestruction();
        }
    }
}

void PlayerParticleManager::translateParticles(aiVector2D translation) {
    std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
    for ( ; iter != playerParticles.end(); iter++) {
        (*iter)->position.x += translation.x;
        (*iter)->position.y += translation.y;
    }
}

bool PlayerParticleManager::initializeForDrawingPoint() {
    
    //Initialize the shader wrapper and OpenGL to allow for the drawing of player particles
    //glGenBuffers(1, &shaderDataPoint.vbo);
    
    
    if (!playerParticlePointShader->attachVert((char *)PLAYER_PARTICLE_VERT.c_str()) ) {
        std::cout << "\nError attaching PlayerParticle vert shader!\n";
        return false;
    }
    if (!playerParticlePointShader->attachFrag((char *)PLAYER_PARTICLE_POINT_FRAG.c_str()) ) {
        std::cout << "\nError attaching PlayerParticle Point frag shader!\n";
        return false;
    }
    
    if (!playerParticlePointShader->link() ) {
        std::cout << "\nError Linking the PlayerParticle Point shader program!\n\n";
        return false;
    }
    
    playerParticlePointShader->setVertexAttribName("position");
    //    if (!playerParticlePointShader->specifyVertexLayout(ShaderWrapper::VERT3, shaderDataPoint.vbo)) {
    if (!playerParticlePointShader->specifyVertexLayout(ShaderWrapper::VERT3, particleVBO)) {
        std::cout << "\nAn error occured while specifying the vertex layout for PlayerParticle! (POINT)\n";
        return false;
    }
    
    shaderDataPoint.setUniformLocationsForPoint(playerParticlePointShader->getID());
    return true;
}

bool PlayerParticleManager::initializeForDrawingExplosion() {
    //Initialize the shader wrapper and OpenGL to allow for the drawing of player particles
    //glGenBuffers(1, &shaderDataExplosion.vbo);
    
    if (!playerParticleExplosionShader->attachVert((char *)PLAYER_PARTICLE_VERT.c_str()) ) {
        std::cout << "\nError attaching PlayerParticle vert shader to Explosion program!\n";
        return false;
    }
    if (!playerParticleExplosionShader->attachFrag((char *)PLAYER_PARTICLE_EXPLOSION_FRAG.c_str())) {
        std::cout << "\nError attaching PlayerParticle Explosion frag shader!\n";
        return false;
    }
    
    if (!playerParticleExplosionShader->link() ) {
        std::cout << "\nError Linking the PlayerParticle Explosion shader program!\n\n";
        return false;
    }
    
    playerParticleExplosionShader->setVertexAttribName("position");
    //if (!playerParticleExplosionShader->specifyVertexLayout(ShaderWrapper::VERT3,
    //                                                         shaderDataPoint.vbo)) {
    if (!playerParticleExplosionShader->specifyVertexLayout(ShaderWrapper::VERT3,
                                                            particleVBO)) {
        std::cout << "\nAn error occured while specifying the vertex layout for ";
        std::cout << "PlayerParticle!(EXPLOSION)\n";
        return false;
    }
    
    shaderDataExplosion.setUniformLocationsForExplosion(playerParticleExplosionShader->getID());
    return true;
}

bool initializeForDrawingTriangle() {
    //Todo...
    return true;
}

void PlayerParticleManager::destroyParticlesFlaggedForDestruction() {
    if (this->getNumberOfParticles() > 0) {
        std::vector<PlayerParticle *> particlesToPreserve;
        std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
        for ( ; iter != playerParticles.end(); iter++) {
            if ( (*iter)->checkIfFlaggedForDestruction()) {
                delete (*iter);
                (*iter) = nullptr;
            }
            else {
                particlesToPreserve.push_back((*iter));
            }
        }
        
        this->playerParticles.clear(); //Clears everything from vector and sets size to 0
        
        if (particlesToPreserve.size() > 0) { //If any particles were preserved
            this->playerParticles.reserve(particlesToPreserve.size());
            particlesToPreserve.swap(this->playerParticles);
        }
    }
}

int PlayerParticleManager::computeVertBufferSize() {
    int size = 0;
    std::vector<PlayerParticle*>::iterator iter = playerParticles.begin();
    for ( ; iter != playerParticles.end(); iter++) {
        if ((*iter)->particlePrimitive == ParticlePrimitive::UNSPECIFIED) {
            continue;
        }
        else  {
            size += ( (*iter)->getVertSize() * (*iter)->getNumberOfVerts());
        }
    }
    return size;
}

void PlayerParticleManager::fillParticleVectors() {
    std::vector<PlayerParticle *>::iterator iter = playerParticles.begin();
    for ( ; iter != playerParticles.end(); iter++) {
        if ( (*iter)->particlePrimitive == ParticlePrimitive::UNSPECIFIED) {
            continue;
        }
        else if ( (*iter)->particlePrimitive == ParticlePrimitive::POINT) {
            playerPointParticles.push_back(static_cast<PlayerParticlePoint *>(*iter));
        }
        else if ( (*iter)->particlePrimitive == ParticlePrimitive::EXPLOSION) {
            playerExplosionParticles.push_back(static_cast<PlayerParticleExplosion *>(*iter));
        }
        else if ( (*iter)->particlePrimitive == ParticlePrimitive::TRIANGLE) {
            playerTriangleParticles.push_back(static_cast<PlayerParticleTriangle *> (*iter));
        }
    }
}

void PlayerParticleManager::constructVertexBufferFromParticleVectors(GLfloat * vertData,
                                                                     int& explosionOffset,
                                                                     int& triangleOffset) {
    int index = 0;
    //Put the point particle vertices into the array
    std::vector<PlayerParticlePoint *>::iterator pointIter = playerPointParticles.begin();
    for (; pointIter != playerPointParticles.end(); pointIter++) {
        vertData[index++] = (*pointIter)->position.x;
        vertData[index++] = (*pointIter)->position.y;
        vertData[index++] = (*pointIter)->position.z;
    }
    
    //Mark where the explosion vertices begin in the array
    explosionOffset = index;
    //Put the explosion particle vertices into the array
    std::vector<PlayerParticleExplosion *>::iterator explosionIter = playerExplosionParticles.begin();
    for (; explosionIter != playerExplosionParticles.end(); explosionIter++) {
        vertData[index++] = (*explosionIter)->position.x;
        vertData[index++] = (*explosionIter)->position.y;
        vertData[index++] = (*explosionIter)->position.z;
    }
    
    
    //Mark where the triangle vertices begin in the array
    triangleOffset = index;
    std::vector<PlayerParticleTriangle *>::iterator triangleIter = playerTriangleParticles.begin();
    for (; triangleIter != playerTriangleParticles.end(); triangleIter++) {
        vertData[index++] = (*triangleIter)->position.x;
        vertData[index++] = (*triangleIter)->position.y;
        vertData[index++] = (*triangleIter)->position.z;
    }
    
}


///Eventually I should make it so that the model only needs to be parameterized once instead of
///repeatedly
//This function takes a loaded model and a vector of floats and fills the vector with the xyz coordinates
//of the model's triangles in the proper ordering.
void PlayerParticleManager::parameterizeModelIntoOrderedVertexListOfTriangles(SimpleObjLoader *     modelData,
                                                                              std::vector<float> &  vertices) {
    if (!(modelData->isLoaded)) { return; }
    
    vertices.clear();
    
    vertices.reserve(modelData->model.faces * 3 * 3); //Three verts per face and three coordinates per vert
    for (int i = 0; i < modelData->model.faces; i++) {
        int * face = modelData->faces[i]; //Faces are triangles
        int  positionIndex = face[0] - 1; ///THE '-1' is VERY VERY important!
        //std::cout << "\nFace " << i << "Position 1 index is " << positionIndex;
        vertices.push_back(modelData->positions[positionIndex][0]);
        vertices.push_back(modelData->positions[positionIndex][1]);
        vertices.push_back(modelData->positions[positionIndex][2]);
        positionIndex = face[3] - 1;
        //std::cout << "\nFace " << i << "Position 2 index is " << positionIndex;
        vertices.push_back(modelData->positions[positionIndex][0]);
        vertices.push_back(modelData->positions[positionIndex][1]);
        vertices.push_back(modelData->positions[positionIndex][2]);
        positionIndex = face[6] - 1;
        //std::cout << "\nFace " << i << "Position 3 index is " << positionIndex;
        vertices.push_back(modelData->positions[positionIndex][0]);
        vertices.push_back(modelData->positions[positionIndex][1]);
        vertices.push_back(modelData->positions[positionIndex][2]);
    }
}

//void PlayerParticleManager::placePlayerPointParticlesAlongTriangleEdges(PlayerEntity * player,
//                                                                        int lineDivisionPoints,
//                                                                        std::vector<PlayerParticle *> &
//                                                                          playerParticles,
//                                                                        std::vector<float> *
//                                                                          orderedModelTrianglePositions) {
void PlayerParticleManager::placePlayerPointParticlesAlongTriangleEdges(PlayerEntity * player,
                                                                        int lineDivisionPoints,
                                                                        std::vector<PlayerParticle *> & playerParticles,
                                                                        float * orderedModelTrianglePositionData,
                                                                        int trianglePositionDataArraySize) {

    if (trianglePositionDataArraySize < 9) {
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::WARNING! placePlayerPointParticlesAlongTrianglesEdges function\n";
            std::cout << "called to place point particles along the edges of the triangles of this ";
            std::cout << "model but\nwhen recording this model only " << trianglePositionDataArraySize;
            std::cout << " xyz coordinates\nwere recorded from this model!\n";
            std::cout << "Function is returning without spawning any particles!\n";
        }
        return;
    }
    
    //So we know we have at least 1 triangle
    int outerLoopIterations = (trianglePositionDataArraySize / 9);
    for (int i = 0; i < outerLoopIterations; i++) { //"for each triangle"
        int vertArrayIndex;
        for (int vertex = 0; vertex < 2; vertex++) {
            vertArrayIndex = (9*i)+(3*vertex);
            aiVector3D startingVert(orderedModelTrianglePositionData[vertArrayIndex],
                                    orderedModelTrianglePositionData[vertArrayIndex + 1],
                                    orderedModelTrianglePositionData[vertArrayIndex + 2]);
            aiVector3D endingVert(orderedModelTrianglePositionData[vertArrayIndex + 3],
                                  orderedModelTrianglePositionData[vertArrayIndex + 4],
                                  orderedModelTrianglePositionData[vertArrayIndex + 5]);
            std::cout << "\nStarting vert is: " << startingVert.x << " " << startingVert.y << " " << startingVert.z << "\n";
            std::cout << "Ending vert is: " << endingVert.x << " " << endingVert.y << " " << endingVert.z << "\n";
            aiVector3D vectorBetweenStartAndEnd = endingVert - startingVert; //i.e. head minus tail
            
            spawnParticlesAlongLine(startingVert, vectorBetweenStartAndEnd, lineDivisionPoints, player,
                                    playerParticles);
            
//            //Now that we have equation for our line, parameterize it and then
//            //step along it while spawning particles at each step
//
//            for (int stepSize = 0; stepSize < lineDivisionPoints; stepSize++) {
//                aiVector3D pointParticlePosition = startingVert +
//                vectorBetweenStartAndEnd * ((float)stepSize / (float)lineDivisionPoints);
//
//                PlayerParticlePoint * particle = new PlayerParticlePoint(pointParticlePosition.x, pointParticlePosition.y, pointParticlePosition.z, player->velocity.x, player->velocity.y, 0.0f, 75.0f);
//                playerParticles.push_back(particle);
//            }
            
        }
        //Need to connect the last leg of the triangle as well (from vertex 3 to vertex 1)
        vertArrayIndex = 9*i;
        aiVector3D startingVert(orderedModelTrianglePositionData[vertArrayIndex] + 6,
                                orderedModelTrianglePositionData[vertArrayIndex + 7],
                                orderedModelTrianglePositionData[vertArrayIndex + 8]);
        aiVector3D endingVert(orderedModelTrianglePositionData[vertArrayIndex],
                              orderedModelTrianglePositionData[vertArrayIndex + 1],
                              orderedModelTrianglePositionData[vertArrayIndex + 2]);
        aiVector3D vectorBetweenStartAndEnd = endingVert - startingVert; //i.e. head minus tail
        
        spawnParticlesAlongLine(startingVert, vectorBetweenStartAndEnd, lineDivisionPoints, player,
                                playerParticles);
//        //Now that we have equation for our line, parameterize it and then
//        //step along it while spawning particles at each step
//        for (int stepSize = 0; stepSize < lineDivisionPoints; stepSize++) {
//            aiVector3D pointParticlePosition = startingVert +
//            vectorBetweenStartAndEnd * ((float)stepSize / (float)lineDivisionPoints);
//
//            PlayerParticlePoint * particle = new PlayerParticlePoint(pointParticlePosition.x, pointParticlePosition.y, pointParticlePosition.z, player->velocity.x, player->velocity.y, 0.0f, 75.0f);
//            playerParticles.push_back(particle);
//        }
        
    }
    
}


void PlayerParticleManager::spawnParticlesAlongLine(aiVector3D                      startingPoint,
                                                    aiVector3D                      line,
                                                    int                             numberOfSubdivisions,
                                                    PlayerEntity *                  player,
                                                    std::vector<PlayerParticle *> & playerParticles) {
    static Quaternion earlyZRot(0.0f, 0.0f, 1.0f, 0.0f);
    static Quaternion xRot(1.0f, 0.0f, 0.0f, 0.0f);
    static Quaternion zRot(0.0f, 0.0f, 1.0f, 0.0f);
    static Quaternion yRot(0.0f, 1.0f, 0.0f, 0.0f);
    
    //Set rotations based off the player instance
    earlyZRot.changeTheta(player->rollAmount);
    xRot.changeTheta(player->thetaX);
    zRot.changeTheta(player->thetaZ);
    yRot.changeTheta(player->thetaY);
    //Now that we have equation for our line, parameterize it and then
    //step along it while spawning particles at each step
    for (int stepNumber = 1; stepNumber < (numberOfSubdivisions + 1); stepNumber++) {
        aiVector3D pointParticlePosition = startingPoint +
        line * ((float)stepNumber / (float(numberOfSubdivisions + 1)));
        
        //Rotate the spawn point to match model's orientation on screen
        pointParticlePosition = earlyZRot.computeRotation(pointParticlePosition);
        pointParticlePosition = xRot.computeRotation(pointParticlePosition);
        pointParticlePosition = zRot.computeRotation(pointParticlePosition);
        pointParticlePosition = yRot.computeRotation(pointParticlePosition);
        
//        PlayerParticlePoint * particle = new PlayerParticlePoint(pointParticlePosition.x, pointParticlePosition.y, pointParticlePosition.z, player->velocity.x, player->velocity.y, 0.0f, 75.0f);
        
//        PlayerParticlePoint * particle = new PlayerParticlePoint(pointParticlePosition.x, pointParticlePosition.y, pointParticlePosition.z, MathFunc::getRandomInRange(-0.1f, 0.1f), 0.0f, 0.0f, 75.0f);
        PlayerParticlePoint * particle = new PlayerParticlePoint(pointParticlePosition, pointParticlePosition.x / 10.0f , pointParticlePosition.y / 10.0f, pointParticlePosition.z / 15.0f, 75.0f);

        playerParticles.push_back(particle);
    }
    
    
}


void PlayerParticleManager::generateExplosionParticles(aiVector3D spawnPoint, aiVector3D velocity) {
    
    int subdivisions = PLAYER_EXPLOSION_PARTICLES_PER_WAVE;
    
    ////////////////////////////////////////////////////////////////////////////
    ///     EXPLOSION VARIATION PARAMETERS     (THIS FACILITATES A RANDOM EXPLOSION EFFECT)
    ////////////////////////////////////////////////////////////////////////////
//    //Variables
//    static constexpr int NUMBER_OF_EFFECTS = 6;
//    int effectToDo = MathFunc::getRandomInRange(1, (NUMBER_OF_EFFECTS + 1)); //The interval here will include 1 and will exclude (NUMBER_OF_EFFECTS + 1)
//    float effectMin, effectMax;
//
//    //Logic to set up effect
//    if (effectToDo == 1) {
//        effectMin = 0.05f;
//        effectMax = 0.485f;
//    }
//    else if (effectToDo == 2) {
//        effectMin = 0.45f;
//        effectMax = 11.1f;
//    }
//    else if (effectToDo == 3) {
//        effectMin = 0.85f;
//        effectMax = 1.275f;
//    }
//    else if (effectToDo == 4) {
//        effectMin = 1.69f;
//        effectMax = 1.81f;
//    }
//    else if (effectToDo == 5) {
//        effectMax = rotation.getTheta();
//        effectMin = cos(effectMax);
//        effectMax = 1.35f * effectMin;
//    }
//    //    else if (effectToDo == 5) {
//    //        effectMin = 3.0f;
//    //        effectMax = 5.0f;
//    //    }
//    else {
//        effectMin = effectMax = 2.8f;
//    }
    ////////////////////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////////////////////
    ///   ACTUAL CODE FOR THE EXPLOSION
    ////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < subdivisions; i++) {
        
       // float randomScalar = MathFunc::getRandomInRange(effectMin, effectMax); //(0.01f, 0.95f); //(0.65f, 11.1f);     //(0.85f, 10.55f);
        //std::cout << "\nRandom is: " << randomScalar;
        
        
        
        
        //rotation.changeTheta(MathFunc::getRandomInRange(2.0f*PI / ((float)subdivisions), 3.14159f));
      //  velocity = rotation.computeRotation(velocity);
        
        //        randomlyScaledVelocity.x = velocity.x * MathFunc::getRandomInRange(0.01f, 0.95f);
        //        randomlyScaledVelocity.y = velocity.y * MathFunc::getRandomInRange(0.01, 0.95f);
        
        //velocity = velocity * randVel;
        //PlayerParticleExplosion * ppp = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, randomlyScaledVelocity.x, randomlyScaledVelocity.y, velocity.z, 75.0f);
//
//        //////////////////////////////////////////////////////////////////////////////////////////
//        ///   Explosion Particle Creation
//        //////////////////////////////////////////////////////////////////////////////////////////
//        ///Right
//        //PlayerParticleExplosion * pppRight = new PlayerParticleExplosion(midpoint.x, midpoint.y, midpoint.z, velocity.x + randomScalar, velocity.y * randomScalar + MathFunc::getRandomInRange(0.2f, 0.4f) * randomScalar, velocity.z, 75.0f);
//        PlayerParticleExplosion * pppRight = new PlayerParticleExplosion(spawnPoint.x, spawnPoint.y, spawnPoint.z, velocity.x + randomScalar, velocity.y * randomScalar, velocity.z, 75.0f);
//
//        playerParticles.push_back(pppRight);
//
//        ///Left
//        PlayerParticleExplosion * pppLeft = new PlayerParticleExplosion(spawnPoint.x, spawnPoint.y, spawnPoint.z, velocity.x - randomScalar, velocity.y * randomScalar, velocity.z, 75.0f);
//        //PlayerParticlePoint * ppp = new PlayerParticlePoint(midpoint, velocity, 75.0f);
//        playerParticles.push_back(pppLeft);
//
//        ///Up
//        PlayerParticleExplosion * pppUp = new PlayerParticleExplosion(spawnPoint, velocity.x * randomScalar, velocity.y + randomScalar, velocity.z, 75.0f);
//        playerParticles.push_back(pppUp);
//
//        ///Down
//        PlayerParticleExplosion * pppDown = new PlayerParticleExplosion(spawnPoint, velocity.x * randomScalar, velocity.y - randomScalar, velocity.z, 75.0f);
//        playerParticles.push_back(pppDown);
//
//        ///UpRight
//        PlayerParticleExplosion * pppUpRight = new PlayerParticleExplosion(spawnPoint, (velocity.x * randomScalar) + randomScalar, (velocity.y * randomScalar) + randomScalar, velocity.z, 75.0f);
//        playerParticles.push_back(pppUpRight);
//
//        ///DownRight
//        PlayerParticleExplosion * pppDownRight = new PlayerParticleExplosion(spawnPoint, (velocity.x * randomScalar) + randomScalar, (velocity.y * randomScalar) - randomScalar, velocity.z, 75.0f);
//        playerParticles.push_back(pppDownRight);
//
//        ///UpLeft
//        PlayerParticleExplosion * pppUpLeft = new PlayerParticleExplosion(spawnPoint, (velocity.x * randomScalar) - randomScalar, (velocity.y * randomScalar) + randomScalar, velocity.z, 75.0f);
//        playerParticles.push_back(pppUpLeft);
//
//        ///DownLeft
//        PlayerParticleExplosion * pppDownLeft = new PlayerParticleExplosion(spawnPoint, (velocity.x * randomScalar) - randomScalar, (velocity.y * randomScalar) - randomScalar, velocity.z, 75.0f);
//        playerParticles.push_back(pppDownLeft);
//
//        ///Everywhere  (cuz why not?)
//        PlayerParticleExplosion * pppAll = new PlayerParticleExplosion(spawnPoint, velocity.x * randomScalar, velocity.y * randomScalar, velocity.z, 75.0f);
//        playerParticles.push_back(pppAll);
//        //////////////////////////////////////////////////////////////////////////////////////////
//
   }
}

void PlayerParticleManager::drawPointParticles(int numToDraw) {
    //Eventually I should implement uniform buffer objects so each particle has its own uniforms, but for now just set the uniforms based off the first particle in the vector
    if (playerPointParticles.size() < 1) { return;}
    playerParticlePointShader->use();
    PlayerParticlePoint * particleToGetUniformsFrom = playerPointParticles[0];
    
    static float pointSize = PLAYER_PARTICLE_POINT_SIZE;
    //    static float counter = 0.0f;
    //    counter = counter + 3.14159f / 45.0f;
    //    pointSize += 0.2f*(float)sin(counter);
    
    //Set the uniforms
    glUniform1f(shaderDataPoint.ulocTime, particleToGetUniformsFrom->timeAlive);
    glUniform1f(shaderDataPoint.ulocRed, particleToGetUniformsFrom->red);
    glUniform1f(shaderDataPoint.ulocGreen, particleToGetUniformsFrom->green);
    glUniform1f(shaderDataPoint.ulocBlue, particleToGetUniformsFrom->blue);
    glUniform1f(shaderDataPoint.ulocZoom, particleToGetUniformsFrom->zoom);
    glUniform1f(shaderDataPoint.ulocPointSize, pointSize);
    
    glDrawArrays(GL_POINTS, 0, numToDraw);
    
}

void PlayerParticleManager::drawExplosionParticles(int offset, int numToDraw) {
    //Eventually I should implement uniform buffer objects so each particle has its own uniforms, but for now just set the uniforms based off the first particle in the vector
    if (playerExplosionParticles.size() < 1) { return;}
    playerParticleExplosionShader->use();
    PlayerParticleExplosion * particleToGetUniformsFrom = playerExplosionParticles[0];
    
    static float pointSize = EXPLOSION_PARTICLE_POINT_SIZE;
    //static float counter = 0.0f;
    //counter = counter + 3.14159f / 45.0f;
    //pointSize += 0.2f*(float)sin(counter);
    
    //Set the uniforms
    glUniform1f(shaderDataExplosion.ulocTime, particleToGetUniformsFrom->timeAlive);
    glUniform1f(shaderDataExplosion.ulocZoom, particleToGetUniformsFrom->zoom);
    glUniform1f(shaderDataExplosion.ulocPointSize, pointSize);
    
    glDrawArrays(GL_POINTS, offset, numToDraw);
    
}

void PlayerParticleManager::drawTriangleParticles(int offset, int numToDraw) {
    if (playerTriangleParticles.size() < 1) { return;}
    ///Player Triangle Particles were never implemented
    // (This note will matter once I implement triangles):
    //Eventually I should implement uniform buffer objects so each particle has its own uniforms, but for now just set the uniforms based off the first particle in the vector
}
