//
//  Please don't look at any of this code! I wrote this after I realized how flawed my
//  overall design is so I do some weird things to make it work. I myself cringe looking
//  at this code. Spare yourself the pain...
//
//  UPDATE: Actually I refactored it bit, so it's not quite as terrible as it was initially.
//          That being said, it still is not great...
//
//  DeathAnimation.cpp
//
//  Created by Forrest Miller on 5/26/18.  (Note this is almost 2 months after I did most
//                                          of the rest of the project)
//

#include "DeathAnimation.h"

using namespace MathFunc;

namespace DeathAnimation {
    static constexpr int DEATH_ANIMATION_TOTAL_FRAMES = 420;
    static constexpr int DEATH_SCENE_PART_ONE_FRAMES = 60;
    static constexpr int DEATH_SCENE_PART_TWO_FRAMES = 120;
    static constexpr int DEATH_SCENE_PART_TWO_CUTOFF =
                                DEATH_SCENE_PART_ONE_FRAMES + DEATH_SCENE_PART_TWO_FRAMES;
    static const aiVector2D PLAYER_COLBOX_OFFSCREEN_SHIFT = aiVector2D(1000.0f, 1000.0f);//1000 was chosen arbitrarily
    
    typedef struct PlayerStateToRecord {
        //Fields
        aiVector3D position;
        float zoom;
        float thetaX, thetaY, thetaZ;
        //Constructor
        PlayerStateToRecord() {
            position.x = position.y = position.z = 0.0f;
            thetaX = thetaY = thetaZ = 0.0f;
            zoom = 1.0f;
        }
        //2 member functions
        void recordPlayer(PlayerEntity * player) {
            this->position = player->position;
            this->thetaX   = player->thetaX;
            this->thetaY   = player->thetaY;
            this->thetaZ   = player->thetaZ;
            this->zoom     = player->zoom;
            //Shift the player's collision boxes as well so it is not on the screen
            player->colBox->translate(PLAYER_COLBOX_OFFSCREEN_SHIFT);
        }
        void resetPlayer(PlayerEntity * player) {
            player->position = this->position;
            player->thetaX = this->thetaX;
            player->thetaY = this->thetaY;
            player->thetaZ = this->thetaZ;
            player->zoom =   this->zoom;
            //Reset player's collision box
            aiVector2D translation = PLAYER_COLBOX_OFFSCREEN_SHIFT;
            translation.x *= -1.0f;
            translation.y *= -1.0f;
            player->colBox->translate(translation);
        }
    } RecordedPlayerState;
    
    typedef struct GameStateOnEnteringFunction {
        //Field
        RecordedPlayerState * playerStates; //arrayOfRecordedStates
        //Constructor
        GameStateOnEnteringFunction() { playerStates = nullptr; }
        //2 member functions
        void recordPlayerStates(PlayerManager * pMan) {
            playerStates = new RecordedPlayerState [pMan->getNumberOfPlayerInstances()];
            Instance ** pInstances = pMan->getInstances();
            for (int i = 0; i < pMan->getNumberOfPlayerInstances(); i++) {
                playerStates[i].recordPlayer(static_cast<PlayerEntity *> (pInstances[i]));
            }
        }
        void resetPlayerStates(PlayerManager * pMan) {
            if (playerStates == nullptr) { return; } //recordPlayerStates was never called
            Instance ** pInstances = pMan->getInstances();
            for (int i = 0; i < pMan->getNumberOfPlayerInstances(); i++) {
                playerStates[i].resetPlayer(static_cast<PlayerEntity *> (pInstances[i]));
            }
            //Release memory
            delete [] playerStates;
            playerStates = nullptr;
        }
    } GameState;
    
    //Helper Function prototypes
    void recordGameState(GameStateInfoForDeathAnimation * gState, GameState & stateToRecord);
    void resetGameState(GameStateInfoForDeathAnimation * gState, GameState & recordedState);
    void resetStage(Stage *);
    void translatePlayerParticles(PlayerParticleManager * particleManag, PlayerEntity * deadPlayer);
    //float getRandomInRange(float min, float max); //I moved this to be in a math-funcion file
    
    
    //Actual Function:
    void playDeathAnimation(GameStateInfoForDeathAnimation * gState, PlayerEntity * player, PlayerParticleManager * playerParticles) {
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::Playing Player Death Animation...\n";
        }
        
        GameState recordedGameState;
        recordGameState(gState, recordedGameState);
        
        static float pi = 3.14159f;
        float xStep = getRandomInRange(1.0f * pi, 2.00f * pi) / DEATH_SCENE_PART_TWO_FRAMES;
        float yStep = getRandomInRange(   0.0f  , 0.75f * pi) / DEATH_SCENE_PART_TWO_FRAMES;
        float zStep = getRandomInRange(2.0f * pi, 5.50f * pi) / DEATH_SCENE_PART_TWO_FRAMES;
        
        //Get the player who died's position
        aiVector2D deadPlayerScreenCoord(player->position.x, player->position.y);
        
        float deadPlayersStartingZoom = player->zoom;
        bool drawStage = true;
        float rotationSpeedIncreaser = 0.0f; //This acts more as a loop counter than a parameter
        for (int i = 0; i < DEATH_ANIMATION_TOTAL_FRAMES; i++) {
            
            //------------------------------------------------------------------
            //
            //        DEATH ANIMATION PART 1
            //
            //------------------------------------------------------------------
            if (i < DEATH_SCENE_PART_ONE_FRAMES) {
                std::vector<GameEntityManager*>::iterator entityManagerIterator = gState->gEntities.begin();
                for (; entityManagerIterator < gState->gEntities.end(); ++entityManagerIterator) {
                    Instance ** instances = (*entityManagerIterator)->getInstances();
                    for (int j = 0; j < (*entityManagerIterator)->getNumberOfInstances(); j++) {
                        instances[j]->position.x -= deadPlayerScreenCoord.x / 60.0f;
                        instances[j]->position.y -= deadPlayerScreenCoord.y / 60.0f;
                    }
                }
                //player->position.x -= deadPlayerScreenCoord.x / 30.0f;
                //player->position.y -= deadPlayerScreenCoord.y / 30.0f;
                
                player->zoom = (deadPlayersStartingZoom -
                                ( (deadPlayersStartingZoom - 14.0f) * ((float)i) / DEATH_SCENE_PART_ONE_FRAMES));
                player->thetaZ += 0.095f;
            }
            else if (i == DEATH_SCENE_PART_ONE_FRAMES) { //End of part 1 animation
                std::vector<GameEntityManager*>::iterator entityManagerIterator = gState->gEntities.begin();
                for (; entityManagerIterator < gState->gEntities.end(); ++entityManagerIterator) {
                    Instance ** instances = (*entityManagerIterator)->getInstances();
                    for (int j = 0; j < (*entityManagerIterator)->getNumberOfInstances(); j++) {
                        instances[j]->position.x += 1000.0f;
                        instances[j]->position.y += 1000.0f;
                    }
                }
                player->position.x = player->position.y = 0.0f; //Reset position to help with cancellation error
            }
            //------------------------------------------------------------------
            //
            //        DEATH ANIMATION PART 2
            //
            //------------------------------------------------------------------
            
            else if (i < DEATH_SCENE_PART_TWO_CUTOFF) {
                rotationSpeedIncreaser += ((1.0f / DEATH_SCENE_PART_TWO_CUTOFF) *
                                           (1.0f + pow(rotationSpeedIncreaser, 2.0f)));
                //player->xRot->changeTheta(player->xRot->getTheta() + xStep);
                //player->yRot->changeTheta(player->yRot->getTheta() + yStep);
                //player->zRot->changeTheta(player->zRot->getTheta() + zStep);
                player->thetaX += (xStep + xStep * rotationSpeedIncreaser);
                player->thetaY += (yStep + yStep * rotationSpeedIncreaser);
                player->thetaZ += (zStep + zStep * rotationSpeedIncreaser);
            }
            
            //------------------------------------------------------------------
            //
            //        DEATH ANIMATION PART 3
            //
            //------------------------------------------------------------------
            else {
                drawStage = false; //Don't draw the stage beyond this point
                
                if (i % FRAMES_BETWEEN_PLAYER_EXPLOSION_WAVES == 0 &&
                    (i < DEATH_SCENE_PART_TWO_CUTOFF + EXPLOSION_PARTICLE_FRAMES_CUTOFF)) {
                    playerParticles->particalizePlayer(player, gState->playerManager->getModelData(), true, 25u, false);
                    Quaternion * particleRotationQuaternion = playerParticles-> getTheExplosionRotationQuaternionSoThatItCanBeMessedWith();
                    //particleRotationQuaternion->changeTheta(particleRotationQuaternion->getTheta() + EXPLOSION_WAVE_ANGLE_CHANGE);
                    //particleRotationQuaternion->changeTheta(particleRotationQuaternion->getTheta() + sin(2.913f) + sin(5.0f));
                    particleRotationQuaternion->changeTheta(particleRotationQuaternion->getTheta() + PI / (7.5f/11.0f) + 0.0067f); //1.342375f); //PI / 7.842375
                }
                
                player->zoom = pow(player->zoom, 1.06f); //1.05f);
                //playerParticles->doUpkeep(); //Move this to happen every loop iteration
            }
            
            //Draw the 'scene'
            playerParticles->doUpkeep();
            std::vector<GameEntityManager*>::iterator entityDrawIterator = gState->gEntities.begin();
            for (; entityDrawIterator < gState->gEntities.end(); ++entityDrawIterator) {
                //bool iteratorIsForStage = ( !(*entityDrawIterator)->requiresUserInput ||
                //                           (!(*entityDrawIterator)->requiresAIInput) ); //I am recognizing Stage by knowing Stage doesn't require input while all other entities do
                
                
                //if (iteratorIsForStage && !drawStage) {break;} //This line currently doesn't work as intended!
                
                (*entityDrawIterator)->drawInstances();
            }
            playerParticles->drawInstances(); //Draw the player particles
            
            //Draw weapon instances as well
            //gState->weaponOverseer->drawInstances();
            
            glfwSwapBuffers(gState->mWindow);
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        
        
        playerParticles->resetTheExplosionRotationQuaternionAfterItHasBeenMessedWith();
        resetGameState(gState, recordedGameState);
        translatePlayerParticles(playerParticles, player); 
        
        //Apply the dead player's velocity on entering the animation to each particle  (maybe attach an identifier to each collection of particles so velocity is only applied to the most recently create ones)
        //Have a function in PlayerParticleManager that handles adding a velocity to each particle? 
        
        
        if (PRINT_DEBUG_MESSAGES || true) {
            std::cout << "\nDEBUG::Finished Playing Player Death Animation...\n";
        }
    }
    
    
    void recordGameState(GameStateInfoForDeathAnimation * gState, GameState & stateToRecord) {
        //No recording is needed for stage since it will always return to a fixed location
        stateToRecord.recordPlayerStates(gState->playerManager);
        //Weapon instances are not drawn during death animation, so no need to record them
    }
    
    void resetGameState(GameStateInfoForDeathAnimation * gState, GameState & recordedState) {
        resetStage(gState->stage);
        recordedState.resetPlayerStates(gState->playerManager);
    }
    
    void resetStage(Stage * stage) {
        
        aiVector3D * stagePosition = &(stage->getInstances()[0]->position);
        stagePosition->x = stagePosition->y = 0.0f;

        //I'm going to save this code below because this is how it should be done,
        //but actually the way I am offseting stage is a calculation performed
        // within the drawing logic...
        /*
        aiVector3D * stagePosition = &(stage->getInstances()[0]->position);
        if (STAGE_POSITION_CENTER_AND_FULLSCREEN_IMAGE) {
            stagePosition->x = stagePosition->y = stagePosition->z = 0.0f;
        }
        else {
            stagePosition->x = 0.5f;
            stagePosition->y = stagePosition->z = 0.0f;
        } */
    }
    
    //Translate the player particles so that the explosion gets recentered to be at the position where the player died instead of the center of the screen
    void translatePlayerParticles(PlayerParticleManager * particleManag, PlayerEntity * deadPlayer) {
        aiVector2D translation;
        translation.x = deadPlayer->position.x;
        translation.y = deadPlayer->position.y;
        particleManag->translateParticles(translation);
        
    }
    
//    float getRandomInRange(float min, float max) {
//        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
//        static std::mt19937 mt_rand((unsigned int)seed);
//        auto real_rand = std::bind(std::uniform_real_distribution<double>(min, max), mt_rand);
//        return real_rand();
//    }
    
} //namespace DeathAnimation
