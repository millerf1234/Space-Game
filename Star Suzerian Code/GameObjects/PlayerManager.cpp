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
    //Set specialization type
    this->specType = specializationType::PLAYER;
    //Set-up the initialization template
    this->generateInitializationTemplate();
//    this->generator->setSpecialization(PLAYER);
    //this->generator->specialization = specializationType::PLAYER;
    //Initialize the generator from the initialization template
    this->initializeFromTemplate(); //generator->setSpecialization now called inside this function
}

PlayerManager::~PlayerManager() {
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
    
    if (MAX_PLAYERS == 2) {  //Controls for 2 people
        //For each instance in generator, handle input
        //Start by getting all playerInstances from generator:
        Instance ** players = generator->getArrayOfInstances();
        //Cast player1 from the instances:
        PlayerInstance * p1 = static_cast<PlayerInstance*>(players[0]);
        //Then get player2 from the instances
        PlayerInstance * p2 = static_cast<PlayerInstance*>(players[1]);
        
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
        }
        //UPDATE: I moved pause to be handled inside the Game class where it belongs
////Should add a counter out here so that unpausing doesn't cause pausing again on the next frame!
////Add a pause key
//if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
//    auto begin = std::chrono::high_resolution_clock::now();  //Time Measurement
//    auto end = std::chrono::high_resolution_clock::now();
//    std::cout << "\nGame Paused!\n";
//    //see: http://convert-units.info/time/nanosecond/300000000
//    while (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() < 300000000){  //this is saying wait 0.3 seconds before moving on
//        std::this_thread::sleep_for(std::chrono::nanoseconds(30000000));
//        end = std::chrono::high_resolution_clock::now();
//    }
//    //Okay, so now that a short pause has occured
//    while (1) {
//        glfwPollEvents();
//        if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
//            //std::cout << "Game Unpaused!" << std::endl;
//            begin = std::chrono::high_resolution_clock::now();
//            auto end = std::chrono::high_resolution_clock::now();
//            while (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() < 30000000){  //this is saying wait 0.3 seconds before moving on
//                std::this_thread::sleep_for(std::chrono::nanoseconds(300000));
//                end = std::chrono::high_resolution_clock::now();
//            }
//            //break; //Doesn't work right...
//            goto unpause;
//        }
//        else {
//            std::this_thread::sleep_for(std::chrono::nanoseconds(333333333)); //Sleep for a third of a second before checking again if unpaused.
//        }
//    }
//}
    }
    else {
        std::cout << "\n\nAARG! This game engine is not that robust yet.\nPlease leave MAX_PLAYERS set to 2!\n";
    }
    //return; //only way to get to the line after this is through unpausing
    //unpause: std::cout << "Game Unpaused!" << std::endl;
}

void PlayerManager::drawInstances() {
    generator->drawInstances();
}



void PlayerManager::initializeFromTemplate() {
    generator->initializeFromTemplate(*initTemplate); //Sets generator up based off the initialization template
    if (MAX_PLAYERS < 1) {return;} //A game with 0 players doesn't need any player instances
    
    //this->generator->setSpecialization(PLAYER); //I no longer use this function
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        generator->generateSingle();
    }
    
    //Remove this 'if MAX_PLAYERS' statement once I have comepltely rewritten this part
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
            PlayerInstance * p = static_cast<PlayerInstance*>(players[i]);
            p->zoom = PLAYER_SIZE;
            p->thetaX = PI / 2.0f; //Set starting x rotation
            p->playerNumber = i+1; //So player 1 get assigned playerNumber 1, player 2 gets playerNumber 2, etc...
            p->type = PLAYERINSTANCE;
            p->accelerate = p->decelerate = p->turnLeft = p->turnRight = p->rollLeft = p->rollRight = p->shoot = false;
            //Set up the array of engineTranslationHistory that make engine flames grow/shrink dynamically with movement
            for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES; ++i) {
                p->translationHistory[i] = new aiVector3D(0.0f, 0.0f, -0.5f);
            }
            
            //Set Player_Specific parameters based off position in array
            if (i == 0) {
                p->red = PLAYER_ONE_RED;
                p->green = PLAYER_ONE_GREEN;
                p->blue = PLAYER_ONE_BLUE;
                p->thetaZ = PI / 2.0f; //Set this to pi/2 to get player1 oriented the correct way at start
                p->position.x += PLAYER1_STARTOFFSET_X;
                p->position.y += PLAYER1_STARTOFFSET_Y;
            }
            else if (i == 1) {
                p->red = PLAYER_TWO_RED;
                p->green = PLAYER_TWO_GREEN;
                p->blue = PLAYER_TWO_BLUE;
                p->thetaZ = -PI / 2.0f; //Set this to -pi/2 to get player2 oriented the correct way at start
                p->position.x += PLAYER2_STARTOFFSET_X;
                p->position.y += PLAYER2_STARTOFFSET_Y;
            }
            else if (i == 2) {
                p->red = PLAYER_THREE_RED;
                p->green = PLAYER_THREE_GREEN;
                p->blue = PLAYER_THREE_BLUE;
                p->thetaZ = -PI / 4.0f;
            }
            else { //Make every player beyond first 3 be white until I actually implement controls for this many players
                p->red = 1.0f;
                p->green = 1.0f;
                p->blue = 1.0f;
                std::cout << "\nWarning! This is more players than have been implemented in the PlayerManager class.\nThere might be some collision issues going forward if more than 4 players\n";
                p->position.y = -0.9f * YLIMIT; //Gotta have players spawn somewhere...
                p->position.x = -0.9f * XLIMIT + 5.0f * ((float)(i-3)); //Make it so players don't spawn on top of eachother
            }
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
        PlayerInstance * player = static_cast<PlayerInstance *>(players[i]);
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
            if (player->turnRight) {goto turnRightAlso;}
            
        }
        else if (player->turnRight) {
        turnRightAlso: player->thetaZ += PLAYER_ROTATION_SPEED_TURNING;
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
            player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->y = 0.0f;
        }
        
        if (player->rollLeft) {
            player->rollAmount -= PLAYER_ROTATION_SPEED_ROLLING * TIME_TICK_RATE / 0.01f;
            if (player->rollAmount < -PI / 2.0f) {
                player->rollAmount = -PI / 2.0f;
            }
        }
        if (player->rollRight) {
            player->rollAmount += PLAYER_ROTATION_SPEED_ROLLING * TIME_TICK_RATE / 0.01f;
            if (player->rollAmount > PI / 2.0f) {
                player->rollAmount = PI / 2.0f;
            }
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
            player->velocity.x += PLAYER_MOVEMENT_SPEED_LINEAR * player->forward->x * (TIME_TICK_RATE / 0.01f);
            player->velocity.y -= PLAYER_MOVEMENT_SPEED_LINEAR * player->forward->y * (TIME_TICK_RATE / 0.01f);
            //Update translation history to make the engine flame grow as player accelerates
            player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z = PLAYER_ENGINE_FLAME_SIZE_INCREASE_FROM_VELOCITY * (player->velocity.Length() / PLAYER_MOVEMENT_MAX_SPEED);
        }
        if (player->decelerate) {
            //If applying the deceleration actually causes the ship's velocity to shrink
            //Method 1  (Note that method 1 is better in my opinion, though both have their shortcomings)
            if (abs(player->velocity.x - PLAYER_MOVEMENT_SPEED_LINEAR * player->forward->x * (TIME_TICK_RATE / 0.01f)) < abs(player->velocity.x)) {
                player->velocity.x -= 2.0f * PLAYER_MOVEMENT_SPEED_LINEAR * player->forward->x * (TIME_TICK_RATE / 0.01f);
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z *= 0.8f;
            }
            else {
                player->velocity.x = 0.0f;
                //Don't let engine flame shrink smaller than 0.08f once engines are started
                 player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z = max(0.08f, player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z);
            }
            if (abs(player->velocity.y + PLAYER_MOVEMENT_SPEED_LINEAR * player->forward->y * (TIME_TICK_RATE / 0.01f)) < abs(player->velocity.y)) {
                player->velocity.y += 2.0f * PLAYER_MOVEMENT_SPEED_LINEAR * player->forward->y * (TIME_TICK_RATE / 0.01f);
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z *= 0.8f; //Shrink by a small factor
            }
            else {
                player->velocity.y = 0.0f;
                player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z = max(0.08f, player->translationHistory[PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1]->z);
            }
        }
        
        
        //Update the translation history array for player (shift all the positions over 1)
        for (int i = 0; i < PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES - 1; ++i) {
            *(player->translationHistory[i]) = *(player->translationHistory[i+1]);
        }
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

                                                                                                        
//Returns the larger of the two integers
float PlayerManager::max(const float& i1, const float& i2) {
    //    return ((float)(i1 >= i2) * i1); //compare i1 i2 and returns 0.0f if i2 is bigger
    //return the larger integer
    if (i1 >= i2) {return i1;}
    else {return i2;}
}
