//
//  Player.cpp
//
//  Created by Forrest Miller on 2/13/18.


#include "PlayerManager.h"

PlayerManager::PlayerManager() : GameEntityManager() {
    this->initTemplate = new InitializationTemplate;
    this->generator = new Generator;
    this->hasGenerator = true;
    this->hasCollision = true;
    this->requiresUserInput = true; //Player controlled characters require user input
    this->requiresAIInput = false; //AI's currently will not be controlling players
    //Set specialization type
    this->specType = specializationType::PLAYER;
    //Set-up the initialization template
    this->generateInitializationTemplate();
    //    this->generator->setSpecialization(PLAYER);
    //    this->generator->specialization = specializationType::PLAYER;
    //Initialize the generator from the initialization template
    this->initializeFromTemplate(); //generator->setSpecialization now called inside this function
}

PlayerManager::~PlayerManager() {
    if (PRINT_DESTRUCTOR_CALLS) {
        std::cout << "\nDEBUG:PlayerManager destructor called!";
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

void PlayerManager::doUpkeep() {
    processInput();
    generator->doUpkeep();
}

void PlayerManager::handleInput(GLFWwindow* mWindow) {
    //UPDATE: I moved these booleans to be stored within each player's instance struct
    //I'm going to just use a bunch of boolean variables here to represent keys that were pressed
    //    bool p1_movedUp, p1_movedDown, p1_turnLeft, p1_turnRight, p1_rollLeft, p1_rollRight;
    //    p1_movedUp = p1_movedDown = p1_turnLeft = p1_turnRight = p1_rollLeft = p1_rollRight = false;
    //    bool p2_movedUp, p2_movedDown, p2_turnLeft, p2_turnRight, p2_rollLeft, p2_rollRight;
    //    p2_movedUp = p2_movedDown = p2_turnLeft = p2_turnRight = p2_rollLeft = p2_rollRight = false;
    //    bool p1_shot = false;
    //    bool p2_shot = false;
    
    if (MAX_PLAYERS <= 2 || true) {  //Controls for 2 people  //REALLY I SHOULD EVENTUALLY JUST REMOVE THIS CHECK ENTIRELY!!!
        //For each instance in generator, handle input
        //Start by getting all playerInstances from generator:
        Instance ** players = generator->getArrayOfInstances();
        //Cast player1 from the instances:
        PlayerEntity * p1 = static_cast<PlayerEntity*>(players[0]);
        //Then get player2 from the instances
        PlayerEntity * p2 = static_cast<PlayerEntity*>(players[1]);
        
        //First need to reset all the input's from the last turn
        p1->accelerate = p1->decelerate = p1->turnLeft = p1->turnRight = p1->rollLeft = p1->rollRight = p1->shoot = false;
        p2->accelerate = p2->decelerate = p2->turnLeft = p2->turnRight = p2->rollLeft = p2->rollRight = p2->shoot = false;
        
        //---------------------------------
        // CHECK FOR KEYS BEING PRESSED AND SET BOOLEANS ON THE PLAYER STRUCTS
        //--------------------------------
        bool printMovementDebugMessages = false;
        
        //Player 1 control input first
        if (glfwGetKey(mWindow, 'W') == GLFW_PRESS) {
            p1->accelerate = true;
            if (printMovementDebugMessages && PRINT_DEBUG_MESSAGES) {
                std::cout << "Player1 Acceleterating" << std::endl;
            }
        }
        if (glfwGetKey(mWindow, 'S') == GLFW_PRESS) {
            p1->decelerate = true;
             if (printMovementDebugMessages && PRINT_DEBUG_MESSAGES) {
                std::cout << "Player1 decelerating" << std::endl;
            }
        }
        if (glfwGetKey(mWindow, 'A') == GLFW_PRESS) {
            p1->turnLeft = true;
             if (printMovementDebugMessages && PRINT_DEBUG_MESSAGES) {
                std::cout << "Player1 Turning Left" << std::endl;
            }
        }
        if (glfwGetKey(mWindow, 'D') == GLFW_PRESS) {
            p1->turnRight = true;
             if (printMovementDebugMessages && PRINT_DEBUG_MESSAGES) {
                std::cout << "Player1 Turning Right" << std::endl;
            }
        }
        if (glfwGetKey(mWindow, '1') == GLFW_PRESS) { //was 'Q'
            p1->rollLeft = true;
            if (printMovementDebugMessages && PRINT_DEBUG_MESSAGES) {
                std::cout << "Player1 Rolling Left" << std::endl;
            }
        }
        if (glfwGetKey(mWindow, '3') == GLFW_PRESS) { //Was 'E'
            p1->rollRight = true;
             if (printMovementDebugMessages && PRINT_DEBUG_MESSAGES) {
                std::cout << "Player1 Rolling Right" << std::endl;
            }
        }
        if (glfwGetKey(mWindow, '2') == GLFW_PRESS) { //was GLFW_KEY_TAB
            p1->shoot = true;
             if (printMovementDebugMessages && PRINT_DEBUG_MESSAGES) {
                 std::cout << "Player1 shooting" << std::endl;
             }
            //aiVector2D forward2D(p1->forward->x, p1->forward->y);
            //p1->velocity = p1->velocity - (forward2D * KINETIC_VELOCITY_LAUNCH_IMPULSE);
        }
        //Player 2 input
        if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS) {
            p2->accelerate = true;
        }
        if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
            p2->decelerate = true;
        }
        if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
            p2->turnLeft = true;
        }
        if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            p2->turnRight = true;
        }
        if (glfwGetKey(mWindow, ',') == GLFW_PRESS) {
            p2->rollLeft = true;
        }
        if (glfwGetKey(mWindow, '/') == GLFW_PRESS) {
            p2->rollRight = true;
        }
        if (glfwGetKey(mWindow, '.') == GLFW_PRESS) {
            p2->shoot = true;
            //aiVector2D forward2D(p2->forward->x, p2->forward->y);
            //p2->velocity = p2->velocity - (forward2D * KINETIC_VELOCITY_LAUNCH_IMPULSE);
        }
        //Pushing both ',' and KEY_RIGHT while holding KEY_UP cancel eachother out...
//        glfwPollEvents(); //Hmm seems to be an issue with GLFW!
//        if (p2->turnRight) {
//            if (glfwGetKey(mWindow, ',') == GLFW_PRESS) {
//                p2->rollLeft = true;
//            }
//        }
//        if (p2->rollLeft) {
//            if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//                p2->turnRight = true;
//            }
//        }
        
    }
    else {
        std::cout << "\n\nAARG! This game (engine) is not that robust yet.\nPlease leave MAX_PLAYERS set to 2!\n";
    }
    //return; //only way to get to the line after this is through unpausing
    //unpause: std::cout << "Game Unpaused!" << std::endl;
}

void PlayerManager::drawInstances() {
    generator->drawInstances();
}

void PlayerManager::initializePlayer(PlayerEntity * player) {
    player->zoom = GAME_SCALE;
    player->thetaX = PI / 2.0f; //Set starting x rotation
    player->resetMovementInformation();
    player->rollAmount = 0.0f;
    player->shields = STARTING_PLAYER_SHIELDS;
    
    for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES; ++i) {
        player->translationHistory[i]->x = 0.0f;
        player->translationHistory[i]->y = 0.0f;
        player->translationHistory[i]->z = -0.5f;
    }
    
    //Have player 1 spawn at player 1's spawn position
    if (player->playerNumber == 1) {
        if (MOON_DRIFTER_MODE) {
            //Set moonDrifter spawn orientation for player 1
            player->thetaZ = mdPLAYER1SPAWNROTATION + 0.001f;
            //Then set the moodDrifter spawn points
            player->position.x = mdPLAYER1SPAWNXOFFSET;
            player->position.y = mdPLAYER1SPAWNYOFFSET;
        }
        else { //battle mode
            player->thetaZ = PI / 2.0f + 0.001f; //Set this to pi/2 to get player1 oriented the correct way at start for battle mode
            player->position.x = PLAYER1_STARTOFFSET_X;
            player->position.y = PLAYER1_STARTOFFSET_Y;
        }
    }
    else if ( player->playerNumber == 2 ) {
        if (MOON_DRIFTER_MODE) { //MOON DRIFTER MODE!
            //Set moonDrifter spawn orientation for player 2
            player->thetaZ = mdPLAYER2SPAWNROTATION + 0.001f;;
            //then set moonDrifter spawns
            player->position.x = mdPLAYER2SPAWNXOFFSET;
            player->position.y = mdPLAYER2SPAWNYOFFSET;
        }
        else { //battle mode
            player->thetaZ = -PI / 2.0f + 0.001f;; //Set this to -pi/2 to get player2 oriented the correct way at start
            player->position.x = PLAYER2_STARTOFFSET_X;
            player->position.y = PLAYER2_STARTOFFSET_Y;
        }
    }
    else {
        player->thetaZ = -PI / 4.0f + 0.001f;
        player->position.y = -0.9f * YLIMIT; //Gotta have players spawn somewhere... (NOTE I USE THE SCREEN LIMITS HERE JUST TO SPAWN THE EXTRA PLAYERS NEAR THE EDGE OF THE SCREEN!)
        player->position.x = -0.9f * XLIMIT + 5.0f * ((float)( (player->playerNumber) - 3)); //Make it so players don't spawn on top of eachother
    }
}

void PlayerManager::initializeFromTemplate() {
    generator->initializeFromTemplate(*initTemplate); //Sets generator up based off the initialization template
    generator->shouldGenerateEntities = true; //Set this to true for player manager
    if (MAX_PLAYERS < 1) {return;} //A game with 0 players doesn't need any player instances
    
    
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        generator->generateSingle();
    }
    
    //Remove this 'if MAX_PLAYERS' statement once I have completely rewritten this part
    if (MAX_PLAYERS) {
        Instance ** players = generator->getArrayOfInstances();
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::GAME WILL BE PLAYED WITH " << MAX_PLAYERS << " PLAYERS." << std::endl;
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                std::cout << "DEBUG::PLAYER " << i+1 << " globalID is: " << players[i]->identifierNumber << std::endl;
            }
        }
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            //For more info on static_cast, see: https://stackoverflow.com/questions/28002/regular-cast-vs-static-cast-vs-dynamic-cast
            PlayerEntity * p = static_cast<PlayerEntity*>(players[i]);
            p->playerNumber = i+1; //So player 1 get assigned playerNumber 1, player 2 gets playerNumber 2, etc...
            p->type = PLAYERENTITY;
            
            //Set up the array of engineTranslationHistory that make engine flames grow/shrink dynamically with movement
            for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES; ++i) {
                p->translationHistory[i] = new aiVector3D(0.0f, 0.0f, -0.5f);
            }
            
            //Initialize player needs to have translationHistory vectors allocated to work properly
            initializePlayer(p); //Initialize player is a function that sets up a player. It is called on player respawn as well
            
            //Set Player_Specific parameters based off position in array
            if (i == 0) { //i.e. if p is player 1
                p->red = PLAYER_ONE_RED;
                p->green = PLAYER_ONE_GREEN;
                p->blue = PLAYER_ONE_BLUE;
            }
            else if (i == 1) { //if p is player 2
                p->red = PLAYER_TWO_RED;
                p->green = PLAYER_TWO_GREEN;
                p->blue = PLAYER_TWO_BLUE;
            }
            else if (i == 2) { //Battle mode only below here (and not fully implemented either)
                p->red = PLAYER_THREE_RED;
                p->green = PLAYER_THREE_GREEN;
                p->blue = PLAYER_THREE_BLUE;
            }
            else { //Make every player beyond first 3 be white until I actually implement controls for this many players
                p->red = 1.0f;
                p->green = 1.0f;
                p->blue = 1.0f;
                std::cout << "\nWarning! This is more players than have been implemented in the PlayerManager class.\nThere might be some collision issues going forward if more than 4 players\n";
            }
            //Set collisionBox data
            aiVector2D position = aiVector2D(p->position.x, p->position.y);
            //p->colBox->setMidpointTo(position); //Set midboxes position to match the player's position
            
            p->colBox->presetRotationOrderSize(3); //tell colBox to preallocate some memory for placing rotations into (3 for 3 rotations)
            
            //Make the rotationQuaternions to initialize the player model with as well.
            Quaternion earlyZcolBx(0.0f, 0.0f, 1.0f, p->rollAmount);
            Quaternion xcolBx(1.0f, 0.0f, 0.0f, p->thetaX);
            Quaternion zcolBx(0.0f, 0.0f, 1.0f, p->thetaZ);
            //Quaternion ycolBx(0.0f, 1.0f, 0.0f, p->thetaY);

            //Put the quaternions into the collisionBoxes' rotation order
            p->colBox->addToRotationOrder(earlyZcolBx); //Do the roll rotation first
            p->colBox->addToRotationOrder(xcolBx); //Align the spaceship to fly within the xy-plane
            p->colBox->addToRotationOrder(zcolBx); //This z-axis rotation is for turning the spacecraft
            //p->colBox->addToRotationOrder(ycolBx); //This rotation is actually never used I think
            
            //Quaternion xColBx(1.0f, 0.0f, 0.0f, p->thetaX);
            //Quaternion yColBx(0.0f, 0.0f, -1.0f, p->thetaZ);

            //Add the rotations to the collisionBox's rotation order
           // p->colBox->addToRotationOrder(xColBx);
           // p->colBox->addToRotationOrder(xColBx);
            
            
            p->colBox->setMidpointTo(position); //Set midboxes position to match the player's position
            
            
        }
        return;
    }
    /*
    //Olde Singleton Initialization Code (Don't use):
    //I'm going to be super risky and do static_cast, because I know that these Instance *'s are to PlayerInstances. for more information, see: https://stackoverflow.com/questions/28002/regular-cast-vs-static-cast-vs-dynamic-cast
    Instance ** players = generator->getArrayOfInstances();
    if (MAX_PLAYERS < 2) {
        std::cout << "\nDEBUG::GAME WILL BE PLAYED WITH A SINGLE PLAYER\n";
        
        players[0]->zoom = PLAYER_SIZE;
        players[0]->thetaX = 3.1415927f / 2.0f;
        PlayerInstance * p1 = static_cast<PlayerInstance*>(players[0]);
        p1->red = PLAYER_ONE_RED;
        p1->green = PLAYER_ONE_GREEN;
        p1->blue = PLAYER_ONE_BLUE;
        p1->playerNumber = 1;
        std::cout << "\nSingle Player PlayerInstance Generation code is incomplete.";
        std::cout << "\nPlease complete the code inside PlayerManager.initializeFromTemplate() before continuing\n";
        
    }
    else if (MAX_PLAYERS < 3) {
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::GAME WILL BE PLAYED WITH 2 PLAYERS. LOADED 2 PLAYERS!\n";
            std::cout << "\nDEBUG::PLAYERS[0] globalID is: " << players[0]->identifierNumber;
            std::cout << "\nDEBUG::PLAYERS[1] globalID is: " << players[1]->identifierNumber;
            std::cout << std::endl;
        }
        //Set Spawn Parameters for each player:
        //Player 1 spawn parameters
        players[0]->zoom = PLAYER_SIZE; //Set starting size
        players[0]->thetaX = 3.1415927f / 2.0f; //pi/2   which is same as 90 degrees
        players[0]->thetaZ = 3.1415927f / 2.0f;
        players[0]->position.x += PLAYER1_STARTOFFSET_X;
        players[0]->position.y += PLAYER1_STARTOFFSET_Y;
        
        //Player 2 spawn parameters
        players[1]->zoom = PLAYER_SIZE; //Both will get the same zoom
        players[1]->thetaX = 3.1415927f / 2.0f;
        players[1]->thetaZ = -3.1415927f / 2.0f;
        players[1]->position.x += PLAYER2_STARTOFFSET_X;
        players[1]->position.y += PLAYER2_STARTOFFSET_Y;
        
        //Set the PLAYER-specific variables for player1
        PlayerInstance * p1 = static_cast<PlayerInstance*>(players[0]); //see: https://stackoverflow.com/questions/8469900/cant-downcast-because-class-is-not-polymorphic
        p1->red = PLAYER_ONE_RED;
        p1->green = PLAYER_ONE_GREEN;
        p1->blue = PLAYER_ONE_BLUE;
        //FOR FUTURE DEBUG: THese next 2 lines shouldn't be needed
        p1->playerNumber = 1; //Note that playerNumber and identifierNumber are different
        p1->type = PLAYERINSTANCE;
        //Set input-detection booleans to false
        p1->accelerate = p1->decelerate = p1->turnLeft = p1->turnRight = p1->rollLeft = p1->rollRight = p1->shoot = false;
        
        //Loop to create an array of previous engine positions
        for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES; ++i) {
            //Code for Doing engine flame with transformations on CPU:
           // p1->translationHistory[i] = new aiVector3D(p1->position); //Call from copy constructor of p1's initial position
           // p1->translationHistory[i]->z -= 0.5f;
            
            //Code for Doing engine flame with transformations on GPU:
            p1->translationHistory[i] = new aiVector3D(0.0f, 0.0f, -0.5f);
        }
        PlayerInstance * p2 = static_cast<PlayerInstance*>(players[1]);
        p2->red = PLAYER_TWO_RED;
        p2->green = PLAYER_TWO_GREEN;
        p2->blue = PLAYER_TWO_BLUE;
        //FOR FUTURE DEBUG: These next 2 lines shouldn't be needed
        p2->playerNumber = 2;
        p2->type = PLAYERINSTANCE; //Set the type specifier on this instance
        //Set input-detection booleans to false
        p2->accelerate = p2->decelerate = p2->turnLeft = p2->turnRight = p2->rollLeft = p2->rollRight = p2->shoot = false;
        for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES; ++i) {
             //Doing engine flame with transformations on CPU
            //p2->translationHistory[i] = new aiVector3D(p2->position); //Call from copy constructor of p2's initial position
            //p2->translationHistory[i]->z -= 0.5f;
            //Doing engine flame with transformation on GPU
            p2->translationHistory[i] = new aiVector3D(0.0f, 0.0f, -0.5f);
        }
    }
    else {
        std::cout << "\n\n!!!! OOOPS!!! THIS NUMBER OF PLAYERS IS NOT YET SUPPORTED!\nTO ADD CODE TO SUPPORT MORE PLAYERS, SEE WHERE THIS MESSAGE IS PRINTED\nIN \"PLAYERMANAGER::INITIALIZE_FROM_TEMPLATE()\"";
    }
    //Message for debug:
    //std::cout << "\n\nFirst 30 vertices are: \n";
    //for (int i = 0; i < 30; ++i) {
    //    std::cout << "vert[" << i << "] = " << generator->vertices[i] << std::endl;
    //}
    //std::cout << "\n\nFirst 30 elements are: \n";
    //for (int i = 0; i < 30; ++i) {
    //    std::cout << "elem[" << i << "] = " << generator->elements[i] << std::endl;
    //}
     */
}

void PlayerManager::processInput() {
    Instance ** players = generator->getArrayOfInstances();
    for (int i = 0; i < generator->getInstanceCount(); ++i) {
        PlayerEntity * player = static_cast<PlayerEntity *>(players[i]);
        
        if (player->isDead) { continue;} //No need to process input from dead players
        
        //NOTE! rollThreshold is just for drawing engine flames, it is not an actual limit on rolling
        float rollThreshold = 0.5f * (PI / 2.0f); //Used in turnLeft and turnRight to check if rolled past threshhold
        
        //DEBUG:
        //if (PRINT_DEBUG_MESSAGES) {
           // std::cout << "\nPlayer " << player->playerNumber << "'s current roll amount: " << player->rollAmount << std::endl;
        //}
        
        //Do player rotation changes based off read input
        if (player->turnLeft) {
            player->thetaZ -= PLAYER_ROTATION_SPEED_TURNING;
            //Change engine thruster patterns depending on roll
            if (player->rollAmount > rollThreshold) { //If rotated to the right
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = player->rollAmount - rollThreshold;
            }
            else if (player->rollAmount < -(rollThreshold)) {
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = player->rollAmount + rollThreshold;
            }
            else {
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = 0.0f;
            }
        }
        if (player->turnRight) {
            player->thetaZ += PLAYER_ROTATION_SPEED_TURNING;
            //Change engine thruster patterns depending on roll
            if (player->rollAmount > rollThreshold) { //If rotated to the right
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = player->rollAmount - rollThreshold;
            }
            else if (player->rollAmount < -rollThreshold) {
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = player->rollAmount + rollThreshold;
            }
            else {
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = 0.0f;
            }
        }
        if (!player->turnLeft && !player->turnRight) { //If not turning, set x to 0
             player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = 0.0f;
            //player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = 0.0f;
        }
        
        if (player->rollLeft) {
            player->rollAmount -= PLAYER_ROTATION_SPEED_ROLLING * TIME_TICK_RATE / 0.01f;
            //if (player->rollAmount < -PI / 2.0f - + (PLAYER_ROTATION_SPEED_TURNING / 11.0f)) {//For before colBox had fudgefactor
            if (player->rollAmount < -PI / 2.0f) {
                player->rollAmount = -PI / 2.0f;
            }
            /* //If not putting a limit on roll, uncomment this
            if (player->rollAmount > (2.0f * PI)) {
                player->rollAmount -= 2.0f * PI;
            }
            */
        }
        if (player->rollRight) {
            player->rollAmount += PLAYER_ROTATION_SPEED_ROLLING * TIME_TICK_RATE / 0.01f;
            //if (player->rollAmount > PI / 2.0f + (PLAYER_ROTATION_SPEED_TURNING / 11.0f)) {//This small amount is so that roll angle is never actually exactly 0.0f
            
            if (player->rollAmount > PI / 2.0f) {
                player->rollAmount = PI / 2.0f;
            }
            /* //If not putting a limit on roll, uncomment this
            if (player->rollAmount < (2.0f * PI)) {
                player->rollAmount += 2.0f * PI;
            }
             */
        }
        
        //Update the rotation quaternions stored within 'player' to represent how the ship is turning
        player->xRot->changeTheta(player->thetaX);
        player->yRot->changeTheta(player->thetaY);
        player->zRot->changeTheta(player->thetaZ);
        
        //Reset forward to be facing the same direction as forward in model coordinates (positive z axis is facing forward there)
        player->forward->x = player->forward->y = 0.0f; //This is the default 'forward' vector when the model is loaded
        player->forward->z = 1.0f;
        //Need to translate this vector the same way the model is being translated on the GPU in the shader code
        *(player->forward) = player->xRot->computeRotation(*(player->forward)); //Currently rotations happen in the order -> x, z, y
        *(player->forward) = player->zRot->computeRotation(*(player->forward));
        *(player->forward) = player->yRot->computeRotation(*(player->forward));
        
        if (PRINT_DEBUG_MESSAGES) {
            //std::cout << "DEBUG::Forward for player " << player->playerNumber << " is: " << forward.x << " " << forward.y <<" ";
            //std::cout << forward.z << std::endl;
        }
        
        //So now forward should represent the actual direction the ship is facing.
        if (player->accelerate) {//The last term (the one with TIME_TICK_RATE) is to make speed change if time tick rate changes
            player->velocity.x += PLAYER_MOVEMEMT_ACCELERATION_LINEAR * player->forward->x * (TIME_TICK_RATE / 0.01f);
            player->velocity.y -= PLAYER_MOVEMEMT_ACCELERATION_LINEAR * player->forward->y * (TIME_TICK_RATE / 0.01f);
            //Update translation history to make the engine flame grow as player accelerates
            player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z = PLAYER_ENGINE_FLAME_SIZE_INCREASE_FROM_VELOCITY * (player->velocity.Length() / PLAYER_MOVEMENT_MAX_SPEED);
        }
        if (player->decelerate) {
            float velocityMagnitude = player->velocity.Length();
            if (velocityMagnitude < 0.025f) {
                velocityMagnitude = 0.0f;
            }
            else {
                player->velocity.Normalize();
            }
            player->velocity *= velocityMagnitude * 0.95f;
            player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z = max(0.08f, player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z);
            
            
            
            //This code is buggy, doesn't work evenly in all directions...
            //If applying the deceleration actually causes the ship's velocity to shrink
            //Method 1  (Note that method 1 is better in my opinion, though both have their shortcomings)
//            if (abs(player->velocity.x - PLAYER_MOVEMEMT_ACCELERATION_LINEAR /* * player->forward->x */ * (TIME_TICK_RATE / 0.01f)) < abs(player->velocity.x * TIME_TICK_RATE / 0.01f)) {
//                player->velocity.x -= 1.0f * PLAYER_MOVEMEMT_ACCELERATION_LINEAR * player->forward->x * (TIME_TICK_RATE / 0.01f);
//                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z *= 0.8f;
//            }
//            else {
//                player->velocity.x = 0.0f;
//                //Don't let engine flame shrink smaller than 0.08f once engines are started
//                 player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z = max(0.08f, player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z);
//            }
//            if (abs(player->velocity.y - PLAYER_MOVEMEMT_ACCELERATION_LINEAR /* * player->forward->y*/ * (TIME_TICK_RATE / 0.01f)) < abs(player->velocity.y * TIME_TICK_RATE / 0.01f)) {
//                player->velocity.y += 2.0f * PLAYER_MOVEMEMT_ACCELERATION_LINEAR * player->forward->y * (TIME_TICK_RATE / 0.01f);
//                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z *= 0.8f; //Shrink by a small factor
//            }
//            else {
//                player->velocity.y = 0.0f;
//                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z = max(0.08f, player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z);
            //}
        }
        
        //Check for shooting at the bottom of this function now, with the rest of the weapontracker stuff
//        //check for shooting
//        if (player->shoot) {
//            player->wepTracker->setKineticWasFired();
//        }
        
        
        //Update the translation history array for player (shift all the positions over 1)
        for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1; ++i) {
            *(player->translationHistory[i]) = *(player->translationHistory[i+1]);
        }
        
        //Update the players collisionBox
        aiVector3D tempMidpoint3D(player->position.x, player->position.y, player->position.z);
        aiVector2D tempMidpoint = aiVector2D(tempMidpoint3D.x, tempMidpoint3D.y);
        
        //I was getting problems here with having tempMidpoint getting set to NAN. If that happens, move position to the center
        if (isnan(tempMidpoint3D.x) || isnan(tempMidpoint3D.y)) {
            players[0]->position = aiVector3D(PLAYER1_STARTOFFSET_X, PLAYER1_STARTOFFSET_Y, 0.0f);
            players[1]->position = aiVector3D(PLAYER2_STARTOFFSET_X, PLAYER2_STARTOFFSET_Y, 0.0f);
            
//
//            //See if translation history has any good positions that can be used
//            for (int i = PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 3; i >= 0; i--) {
//                if (!isnan((player->translationHistory[i])->x) && !isnan((player->translationHistory[i]->y))) {
//                    player->position = *(player->translationHistory[i]);
//
//                }
//            }
        }
        
        player->colBox->setMidpointTo(tempMidpoint);
        
        player->colBox->setCollisionBoxShrinkageFactor(PLAYER_COLLISION_BOX_TO_MODEL_SCALE_FACTOR); 
        //player->colBox->changeRotationAt(0, player->thetaX);
//        player->colBox->changeRotationAt(1, PI / 2.0f /*player->thetaZ*/);
        
        
        player->colBox->changeRotationAt(0, player->rollAmount );
        player->colBox->changeRotationAt(1, player->thetaX);
        player->colBox->changeRotationAt(2, -1.0f * player->thetaZ + PI); //Box was backwards, this is a quick fix
        //player->colBox->changeRotationAt(3, player->thetaY); //This rotation actually never happens?
        
        
        //Lastly, set the flags within player's wepTracker for weapons being switched or fired
        if (player->hasWeaponTracker && player->wepTracker->getHasWeponSpawnPointsSet()) {
            aiVector3D pos3D = player->position;
            player->wepTracker->setPosition(aiVector2D(pos3D.x, pos3D.y));
            
            //Set the forward direction for the weapons tracker
            aiVector3D tempForward3D = *(player->forward);
            aiVector2D tempForward2D(tempForward3D.x, tempForward3D.y);
            player->wepTracker->setForwardDirection(tempForward2D);
            
            //check for shooting
            if (player->shoot) {
                bool activeWeaponTypeFound = false;
                //Get the active weapon type
                if (player->wepTracker->getKineticActive()) {
                    player->updateWeaponTracker(); //Give the weapon tracker the latest player position information
                    activeWeaponTypeFound = true;
                    //Check to see if the player has enough ammo to shoot
                    AmmoCount playerAmmo = player->wepTracker->getAmmoCount();
                    if (playerAmmo.kinetic <= 0) {
                        std::cout << "\nPlayer " << player->playerNumber << " out of Kinetic ammo!\n";
                    }
                    else {
                        playerAmmo.kinetic--;
                        player->wepTracker->setKineticWasFired(); //set the kinetic-fired flag
                    }
                    
                }
                //else if (player->wepTracker->getOtherWepTypeActive() {
                    //Check for ammo in other weapon type
                    //if there is ammo, decrease ammo and set the spawn flag
                
                //}
                //else if  (other weapon types)
                
                //after checking all weapon types
                if (!activeWeaponTypeFound) {
                    if (PRINT_DEBUG_WARNING_MESSAGES) {
                    std::cout << "\nDEBUG::OOS! No Active Weapon Type found in PlayerManager function processInput()...\n";
                    }
                }
            }
           
        }
        else {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! THIS PLAYER DOES NOT HAVE A WEAPONS TRACKER ATTACHED!\n";
            }
        }
    }
    //Check to see if any player's playerboxes are overlapping, and if so, move them apart
}

void PlayerManager::processCollisions() {
    //Just gonna hardcode in something simple knowing I have only 2 players, can make this more robust later
    Instance ** players = generator->getArrayOfInstances();
    
    Impact2D playerImpact(0.5f);
//    playerImpact.setMass1(100.0f); //Using a large mass difference for testing
//    playerImpact.setMass2(0.25f);
    playerImpact.setMass1(1.0f);
    playerImpact.setMass2(1.0f);
    
    PlayerEntity * p1 = static_cast<PlayerEntity *>(players[0]);
    PlayerEntity * p2 = static_cast<PlayerEntity *>(players[1]);
    if (p1->isDead || p2->isDead) { //Don't do collisions if a player is dead
        return;
    }
    //See if player1 hit player2 (or if player2 hit player1)
    if (p1->colBox->isOverlapping(*(p2->colBox)) || p2->colBox->isOverlapping(*(p1->colBox))) {
        //p1->colBox->moveApartAlongAxisBetweenMidpoints(*(p2->colBox));
        //aiVector2D p1BoxMidpoint = p1->colBox->getMidpoint();
        //aiVector2D p2BoxMidpoint = p2->colBox->getMidpoint();
        
        //p1->position.x = p1BoxMidpoint.x;
        //p1->position.y = p1BoxMidpoint.y;
        
        //p2->position.x = p2BoxMidpoint.x;
        //p2->position.y = p2BoxMidpoint.y;
        
        //playerImpact.computeCollisionSimple(p1->velocity, p2->velocity);
        float massP1 = 1.0f;
        float massP2 = 1.0f;
        float coefOfRestitution = 0.5f;
        playerImpact.computeCollisionExperimentalWithEndingVeloctiyOutput(p1->velocity, p2->velocity,
                                                                          massP1, massP2,
                                                                          *(p1->colBox), *(p2->colBox),
                                                                          coefOfRestitution);
        
        p1->colBox->moveApartAlongAxisBetweenMidpoints(*(p2->colBox));
        aiVector2D p1BoxMidpoint = p1->colBox->getMidpoint();
        aiVector2D p2BoxMidpoint = p2->colBox->getMidpoint();
        
        p1->position.x = p1BoxMidpoint.x;
        p1->position.y = p1BoxMidpoint.y;
        
        p2->position.x = p2BoxMidpoint.x;
        p2->position.y = p2BoxMidpoint.y;
        
        //Have the players move a step based off their new velocity vectors
        //Update every instances position with it's velocity
        p1->position.x += p1->velocity.x;
        p1->position.y += p1->velocity.y;
        p2->position.x += p2->velocity.x;
        p2->position.y += p2->velocity.y;
        
    }
    
}

void PlayerManager::generateInitializationTemplate() {
    initTemplate->vertShaderPath = PLAYERSHIP_BODY_VERT;
    initTemplate->hasVert = true;
    initTemplate->fragShaderPath = PLAYERSHIP_BODY_FRAG;
    initTemplate->hasFrag = true;
    
    initTemplate->vertAttribName = "position";
    initTemplate->typeDataRequired = PLAYER;
    
    //If doing format vert3normal3 (NOTE: DON"T DO THIS FORMAT) then uncomment these lines:
    //initTemplate->normalAttribName = "norml";
    //initTemplate->vert3norml3 = true;
    
    //If doing format vert3, then do this:
    initTemplate->vert3 = true;
    
    //Player models will be loaded from an obj file, so set alreadyLoaded to false
    initTemplate->hasVertsAlreadyLoaded = false;
    initTemplate->modelFilePath = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/space_ship2.obj";
    
}

void PlayerManager::processPlayerDeaths() {
    Instance ** players = generator->getArrayOfInstances();
    for (int i = 0; i < generator->getInstanceCount(); i++) {
        PlayerEntity * player = static_cast<PlayerEntity *>( players[i] );
        if ((player->shouldDieFlag)) {
            player->shouldDieFlag = false;
            player->isDead = true;
            player->deaths += 1;
            player->health = 100.0f; //(float)FRAMES_BETWEEN_PLAYER_RESPAWN; //Don't rely on FRAMES_BETWEEN_PLAYER_RESPAWN being positive. 100.0f was chosen randomly
            player->framesUntilRespawn = max(FRAMES_BETWEEN_PLAYER_RESPAWN, 2);
        }
        if (player->isDead) { //Decrement the respawn counter on all dead players
            //set player's velocity to 0
            player->velocity.x = 0.0f;
            player->velocity.y = 0.0f;
            player->framesUntilRespawn -= 1;
            if (player->framesUntilRespawn <= 0) {
                processPlayerRespawn(player, i);
            }
        }
    }
}

void PlayerManager::processPlayerRespawn(PlayerEntity * player, int playerInstanceNumber) {
    //Reset player's attibutes and flags
    player->isDead = false;
    player->framesUntilRespawn = 0;
    player->health = STARTING_PLAYER_HEALTH;
    player->velocity.x = 0.0f;
    player->velocity.y = 0.0f;
    AmmoCount deadPlayersAmmo = player->wepTracker->getAmmoCount();  //reset player ammo
    deadPlayersAmmo.kinetic = PLAYER_KINETIC_AMMO;
    
    initializePlayer(player); //Resets the player
    
    //Gather information on the location of all players to prevent spawn overlapping
    aiVector3D playerPositions[generator->getInstanceCount()];
    Instance ** playerInstances = generator->getArrayOfInstances();
    
    static int loopTimeoutCounter;
    bool validSpawnPoint = true;
    loopTimeoutCounter = 0; //reset before entering loop
    do {
        loopTimeoutCounter++;
        player->colBox->setMidpointTo(aiVector2D(player->position.x, player->position.y));
        player->rollAmount = 0;
        player->colBox->changeRotationAt(0, player->rollAmount);
        player->colBox->changeRotationAt(1, player->thetaX);
        player->colBox->changeRotationAt(2, player->thetaZ);
        
    //Check this players' position against the positions of the other players
        for (int i = 0; i < generator->getInstanceCount(); i++) {
            if ( i == playerInstanceNumber ) { break; }
            PlayerEntity * otherPlayer = static_cast<PlayerEntity *>(playerInstances[i]);
            if (otherPlayer->isDead) { break; }
            if (otherPlayer->colBox->isOverlapping(*(player->colBox))) {
                validSpawnPoint = false;
            }
        }
        if (!validSpawnPoint) { //Move the spawn point if it isn't valid
            player->position.x = player->position.x / 2.0f;
            player->position.y = player->position.y / 2.0f;
            if (player->position.x < 5.0f) {
                player->position.x = XLIMIT - 2.0f;
            }
            if (player->position.y < 3.0f) {
                player->position.y = -YLIMIT + 2.0f;
            }
        }
    } while (!validSpawnPoint && (loopTimeoutCounter < 100));
    
    //player->colBox->setMidpointTo(aiVector2D(player->position.x, player->position.y));
    
    //Reset to rotations as well

    //player->colBox->changeRotationAt(3, player->thetaY);
}

                                                                                                        
//Returns the larger of the two floats  (i really should replace this with a templated function)
float PlayerManager::max(const float& x1, const float& x2) const {
    //return the larger
    if (x1 >= x2) {return x1;}
    else {return x2;}
}
int PlayerManager::max(const int x1, const int x2) const {
    //return the larger
    if (x1 >= x2) {return x1;}
    else {return x2;}
}
