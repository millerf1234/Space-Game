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
        //Player 1 control input first
        if (glfwGetKey(mWindow, 'W') == GLFW_PRESS) {
            p1->accelerate = true;
        }
        if (glfwGetKey(mWindow, 'S') == GLFW_PRESS) {
            p1->decelerate = true;
        }
        if (glfwGetKey(mWindow, 'A') == GLFW_PRESS) {
            p1->turnLeft = true;
        }
        if (glfwGetKey(mWindow, 'D') == GLFW_PRESS) {
            p1->turnRight = true;
        }
        if (glfwGetKey(mWindow, 'Q') == GLFW_PRESS) {
            p1->rollLeft = true;
        }
        if (glfwGetKey(mWindow, 'E') == GLFW_PRESS) {
            p1->rollRight = true;
        }
        if (glfwGetKey(mWindow, GLFW_KEY_TAB) == GLFW_PRESS) {
            p1->shoot = true;
        }
        
        //Player 2 input
        if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS) {
            p1->accelerate = true;
        }
        if (glfwGetKey(mWindow, 'S') == GLFW_PRESS) {
            p1->decelerate = true;
        }
        if (glfwGetKey(mWindow, 'A') == GLFW_PRESS) {
            p1->turnLeft = true;
        }
        if (glfwGetKey(mWindow, 'D') == GLFW_PRESS) {
            p1->turnRight = true;
        }
        if (glfwGetKey(mWindow, 'Q') == GLFW_PRESS) {
            p1->rollLeft = true;
        }
        if (glfwGetKey(mWindow, 'E') == GLFW_PRESS) {
            p1->rollRight = true;
        }
        if (glfwGetKey(mWindow, GLFW_KEY_TAB) == GLFW_PRESS) {
            p1->shoot = true;
        }
        
        
        //Add a pause key
        if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            auto begin = std::chrono::high_resolution_clock::now();  //Time Measurement
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "\nGame Paused!\n";
            //see: http://convert-units.info/time/nanosecond/300000000
            while (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() < 300000000){  //this is saying wait 0.3 seconds before moving on
                std::this_thread::sleep_for(std::chrono::nanoseconds(30000000));
                end = std::chrono::high_resolution_clock::now();
            }
            //Okay, so now that a short pause has occured
            while (1) {
                glfwPollEvents();
                if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
                    //std::cout << "Game Unpaused!" << std::endl;
                    begin = std::chrono::high_resolution_clock::now();
                    auto end = std::chrono::high_resolution_clock::now();
                    while (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() < 30000000){  //this is saying wait 0.3 seconds before moving on
                        std::this_thread::sleep_for(std::chrono::nanoseconds(300000));
                        end = std::chrono::high_resolution_clock::now();
                    }
                    //break;
                    goto unpause;
                }
                else {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(333333333)); //Sleep for a third of a second before checking again if unpaused.
                }
            }
        }
    }
    else {
        std::cout << "\n\nAARG! This game engine is not that robust yet.\nPlease leave MAX_PLAYERS set to 2!\n";
    }
    return;
    unpause: std::cout << "Game Unpaused!" << std::endl;
}

void PlayerManager::drawInstances() {
    
    generator->drawInstances();
}

void PlayerManager::initializeFromTemplate() {
    generator->initializeFromTemplate(*initTemplate); //Sets generator up based off the initialization template
    //this->generator->setSpecialization(PLAYER);
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        generator->generateSingle();
    }
    //I'm going to be super risky and do static_cast, because I know that these Instance *'s are to PlayerInstances. for more information, see: https://stackoverflow.com/questions/28002/regular-cast-vs-static-cast-vs-dynamic-cast
    Instance ** players = generator->getArrayOfInstances();
    if (MAX_PLAYERS < 2) {
        std::cout << "\nDEBUG::GAME WILL BE PLAYED WITH A SINGLE PLAYER\n";
        
        players[0]->zoom = 40.5f;
        players[0]->thetaX = 3.1415927f / 2.0f;
        PlayerInstance * p1 = static_cast<PlayerInstance*>(players[0]);
        p1->red = PLAYER_ONE_RED;
        p1->green = PLAYER_ONE_GREEN;
        p1->blue = PLAYER_ONE_BLUE;
        p1->playerNumber = 1;
        
    }
    else if (MAX_PLAYERS < 3) {
        std::cout << "\nDEBUG::GAME WILL BE PLAYED WITH 2 PLAYERS. LOADED 2 PLAYERS!\n";
        std::cout << "\nDEBUG::PLAYERS[0] globalID is: " << players[0]->identifierNumber;
        std::cout << "\nDEBUG::PLAYERS[1] globalID is: " << players[1]->identifierNumber;
        std::cout << std::endl;
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
        PlayerInstance * p2 = static_cast<PlayerInstance*>(players[1]);
        p2->red = PLAYER_TWO_RED;
        p2->green = PLAYER_TWO_GREEN;
        p2->blue = PLAYER_TWO_BLUE;
        //FOR FUTURE DEBUG: These next 2 lines shouldn't be needed
        p2->playerNumber = 2;
        p2->type = PLAYERINSTANCE; //Set the type specifier on this instance
        //Set input-detection booleans to false
        p2->accelerate = p2->decelerate = p2->turnLeft = p2->turnRight = p2->rollLeft = p2->rollRight = p2->shoot = false;
    }
    else {
        std::cout << "\n\n!!!! OOOPS!!! THIS NUMBER OF PLAYERS IS NOT YET SUPPORTED!\nTO ADD CODE TO SUPPORT MORE PLAYERS, SEE WHERE THIS MESSAGE IS PRINTED\nIN \"PLAYERMANAGER::INITIALIZE_FROM_TEMPLATE\"";
    }
    //std::cout << "\n\nFirst 30 vertices are: \n";
    //for (int i = 0; i < 30; ++i) {
    //    std::cout << "vert[" << i << "] = " << generator->vertices[i] << std::endl;
    //}
    //std::cout << "\n\nFirst 30 elements are: \n";
    //for (int i = 0; i < 30; ++i) {
    //    std::cout << "elem[" << i << "] = " << generator->elements[i] << std::endl;
    //}
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
