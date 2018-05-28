//
//  Please don't look at any of this code! I wrote this after I realized how flawed my
//  overall design is so I do some weird things to make it work. I myself cringe looking
//  at this code. Spare yourself the pain...
//
//  DeathAnimation.cpp
//
//  Created by Forrest Miller on 5/26/18.  (Note this is almost 2 months after I did most
//                                          of the rest of the project)
//

#include "DeathAnimation.h"

namespace DeathAnimation {
    static constexpr int DEATH_ANIMATION_TOTAL_FRAMES = 420;
    static constexpr int DEATH_SCENE_PART_ONE_FRAMES = 60;
    static constexpr int DEATH_SCENE_PART_TWO_FRAMES = 180;
    static const aiVector2D PLAYER_COLBOX_SHIFT = aiVector2D(1000.0f, 1000.0f);
    
    
    //Structs
    typedef struct GameStateOnEnteringFunction {
        aiVector3D alivePlayerPosition;
        float alivePlayerThetaX, alivePlayerThetaY, alivePlayerThetaZ;
        bool bothPlayersDead;
        int indexOfAlivePlayer;
        std::vector<aiVector2D> projectilePositions;
    } GameState;
    
    
    typedef struct GameStateOnEnteringFunction2 {
        
    } GameState2;
    
    typedef struct PlayerStateToRecord {
        
    } RecordedPlayerState;
    
    //Helper Function prototypes
    void recordGameState(GameStateInfoForDeathAnimation * gState, GameState & stateToRecord);
    void resetGameState(GameStateInfoForDeathAnimation * gState, GameState & recordedState);
    void resetStage(Stage *);
    void recordPlayerInstances(PlayerManager * playerManager, GameState & stateToRecord);
    void resetPlayerInstances(PlayerManager * playerManager, GameState & stateToRecord);
    float calculateRotationStep(float min, float max);
    
    
    
    
    //Actual Function:
    void playDeathAnimation(GameStateInfoForDeathAnimation * gState, PlayerEntity * player) {
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::Playing Player Death Animation...\n";
        }
        GameState recordedState;
        recordedState.bothPlayersDead = true;
        recordedState.indexOfAlivePlayer = -1;
        recordGameState(gState, recordedState);
        
        
        float xStep = calculateRotationStep(3.14159f, 3.14159f*2.0f);
        float yStep = calculateRotationStep(0.0f, 0.5f * 3.14159f);
        float zStep = calculateRotationStep(3.14159f * 2.0f, 3.14159f * 5.5f);
        
        std::cout << "\nxStep is " << xStep << "\nyStep is " << yStep << "\nzStep is " << zStep;
        
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
                        instances[j]->position.x += deadPlayerScreenCoord.x / 60.0f;
                        instances[j]->position.y += deadPlayerScreenCoord.y / 60.0f;
                    }
                }
                player->position.x -= deadPlayerScreenCoord.x / 30.0f;
                player->position.y -= deadPlayerScreenCoord.y / 30.0f;
                
                player->zoom = (deadPlayersStartingZoom -
                                ( (deadPlayersStartingZoom - 15.0f) * ((float)i) / DEATH_SCENE_PART_ONE_FRAMES));
            }
            
            //------------------------------------------------------------------
            //
            //        DEATH ANIMATION PART 2
            //
            //------------------------------------------------------------------
            else if (i < DEATH_SCENE_PART_TWO_FRAMES) {
                rotationSpeedIncreaser += ((1.0f / DEATH_SCENE_PART_TWO_FRAMES) *
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
                drawStage = false; //Don't draw the stage beyond part 1
                
                
            }
            
            //Draw the 'scene'
            std::vector<GameEntityManager*>::iterator entityDrawIterator = gState->gEntities.begin();
            for (; entityDrawIterator < gState->gEntities.end(); ++entityDrawIterator) {
                bool iteratorIsForStage = ( !(*entityDrawIterator)->requiresUserInput ||
                                           (!(*entityDrawIterator)->requiresAIInput) ); //I am recognizing Stage by knowing Stage doesn't require input while all other entities do
                
                
                //if (iteratorIsForStage && !drawStage) {break;} //This line currently doesn't work as intended!
                
                (*entityDrawIterator)->drawInstances();
            }
            //Draw weapon instances as well
            //gState->weaponOverseer->drawInstances();
            
            glfwSwapBuffers(gState->mWindow);
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        
        
        resetGameState(gState, recordedState);
        if (PRINT_DEBUG_MESSAGES || true) {
            std::cout << "\nDEBUG::Finished Playing Player Death Animation...\n";
        }
    }
    
    
    void recordGameState(GameStateInfoForDeathAnimation * gState, GameState & stateToRecord) {
        
        //Don't need to record stage because I know the fixed points to always move it back to
        
        recordPlayerInstances(gState->playerManager, stateToRecord);
        
        
        
    }
    
    void resetGameState(GameStateInfoForDeathAnimation * gState, GameState & recordedState) {
        
        resetStage(gState->stage);
        resetPlayerInstances(gState->playerManager, recordedState);
        
        
    }
    
    void resetStage(Stage * stage) {
        aiVector3D * stagePosition = &(stage->getInstances()[0]->position);
        if (CENTER_AND_FULLSCREEN_IMAGE) {
            stagePosition->x = stagePosition->y = stagePosition->z = 0.0f;
        }
        else {
            stagePosition->x = 0.5f;
            stagePosition->y = stagePosition->z = 0.0f;
        }
    }
    
    void recordPlayerInstances(PlayerManager * playerManager, GameState & stateToRecord) {
        Instance ** playerInstances = playerManager->getInstances();
        for (int i = 0; i < playerManager->getNumberOfPlayerInstances(); i++) {
            PlayerEntity * player = static_cast<PlayerEntity *>(playerInstances[i]);
            if (player->isDead) {
                continue; //or could be break;
            }
            stateToRecord.indexOfAlivePlayer = i;
            stateToRecord.bothPlayersDead = false;
            stateToRecord.alivePlayerPosition = player->position;
            stateToRecord.alivePlayerThetaX = player->thetaX;
            stateToRecord.alivePlayerThetaY = player->thetaY;
            stateToRecord.alivePlayerThetaZ = player->thetaZ;
        }
        
        //Shift the player's collision boxes as well so they are not on the screen
        for (int i = 0; i < playerManager->getNumberOfPlayerInstances(); i++) {
            playerInstances[i]->colBox->translate(PLAYER_COLBOX_SHIFT);
        }
        
        
    }
    
    void resetPlayerInstances(PlayerManager * playerManager, GameState & stateToRecord) {
        
        Instance ** playerInstances = playerManager->getInstances();
        
        //Reset the collision boxes for each player to be where they were when the death animation started
        aiVector2D translation = PLAYER_COLBOX_SHIFT;
        translation.x *= -1.0f;
        translation.y *= -1.0f;
        for (int i = 0; i < playerManager->getNumberOfPlayerInstances(); i++) {
            playerInstances[i]->colBox->translate(translation);
        }
        
        
        if (stateToRecord.bothPlayersDead) {
            return;
        }
        
        
        playerInstances[stateToRecord.indexOfAlivePlayer]->position = stateToRecord.alivePlayerPosition;
        
        playerInstances[stateToRecord.indexOfAlivePlayer]->thetaX = stateToRecord.alivePlayerThetaX;
        playerInstances[stateToRecord.indexOfAlivePlayer]->thetaY = stateToRecord.alivePlayerThetaY;
        playerInstances[stateToRecord.indexOfAlivePlayer]->thetaZ = stateToRecord.alivePlayerThetaZ;
        
    }
    
    float calculateRotationStep(float min, float max) {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        static std::mt19937 mt_rand(seed);
        //static std::mt19937 mt_rand((unsigned)time(0u));  //Use a static random num generator
        auto real_rand = std::bind(std::uniform_real_distribution<double>(min, max),
                                   mt_rand);
        float angle = real_rand();
        return (angle / (DEATH_SCENE_PART_TWO_FRAMES));
    }
    
} //namespace DeathAnimation
