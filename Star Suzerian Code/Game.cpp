//  This contains the implementation for the class that essentially handles running the game.
//  
//  Game.cpp
//
//  Created by Forrest Miller on 2/12/18.

#define MAX_GAME_ENTITIES 100000

#include "Game.h"



//Constructor
Game::Game(MonitorData & mPtr) {
    //Make the rotation quaternions (4th parameter is starting theta)
    std::cout << INDENT << "Generating Global Rotation Quaternions...";
    xRot = new Quaternion(1.0f, 0.0f, 0.0f, 0.0f); //rotation about x axis
    yRot = new Quaternion(0.0f, 1.0f, 0.0f, 0.0f); //rotation about y axis
    zRot = new Quaternion(0.0f, 0.0f, 1.0f, 0.0f); //rotation about z axis
    this->levels = nullptr;
    this->numberOfLevels = 0; //Start with 0 levels
    counter = 0.0f; //Set game clock to 0
    p1Score = p2Score = frameNumber = frameUnpaused = 0ull;
    this->mWindow = mPtr.activeMonitor; //Set window to the active window
    std::cout << "Done" << std::endl;
    
    //Some debug code for testing (Delete this eventually)
//    //ColloisionBox formation test (Pathological case)
//    float modelVerts [] = {
//        //x0 ,    y ,   z
//         1.5f,  1.5f,  1.0f,
//        -1.0f,  1.0f,  1.0f,
//        -1.0f, -1.0f,  1.0f,
//         1.0f, -1.0f,  1.0f,
//         1.5f,  1.5f, -1.0f,
//        -1.0f,  1.0f, -1.0f,
//        -1.0f, -1.0f, -1.0f,
//         1.0f, -1.0f, -1.0f,
//    };
//    CollisionBox temp(modelVerts, 24);
}

//Destructor (basically does a 'delete x' for every 'new x' that was called in the constructor)
Game::~Game() { //std::cout << "\nDEBUG::Game destructor was called...\n";
    //Delete rotation Quaternions
    if (xRot != nullptr) {//Check to make sure they havn't already been deleted for some reason
        delete xRot;
        xRot = nullptr; //Set to nullptr to avoid dangling pointer
    }
    if (yRot != nullptr) {
        delete yRot;
        yRot = nullptr;
    }
    if (zRot != nullptr) {
        delete zRot;
        zRot = nullptr;
    }
 
    //Delete game entities in the gameEntityManager array
    std::vector<GameEntityManager *>::iterator iter;
    for (iter = gEntities.begin(); iter != gEntities.end(); ++iter) {
        if (*iter != nullptr) {
            delete (*iter);
            (*iter) = nullptr;
        }
    }
    
}

void Game::playIntroMovie() {  }

//Note that since dynamic memory to entityManager pointers are allocated into the
//vector, the only way they will be deleted is also through the vector. i.e. the vector will
//always have ownership over its entries
void Game::loadGameObjects() {
    //Load the stages first
    std::cout << std::endl << std::endl << INDENT << "Loading Stage...\n";
    //for (int i = 0; i < NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD; ++i) {
        gEntities.push_back(new Stage);
        std::cout << INDENT << "Level " << /*i*/0+1 << " loaded..." << std::endl;
    //}
    //Since I am no longer doing the loop, do:
    //stage = static_cast<Stage*> (gEntities[0]); //Note: Looking at this code a month after I wrote it and not sure why I do (did) this
    
    
    //Then load weapons next (i.e. initiailize them, but don't actually spawn any yet...) Just get them ready for drawing.
    std::cout << std::endl << INDENT << "Loading Weapons...\n";
    wepOverseer.initializeWeapons();
    
    //Then load all parts required for a PLAYER object
    std::cout << std::endl << INDENT << "Loading Player...\n";
    std::cout << INDENT << "    Loading Player Model...\n";
    gEntities.push_back(new PlayerManager);
    playerManager = static_cast<PlayerManager *>(gEntities[1]);
    
    int numberOfPlayersThatNeedWeaponManagers = playerManager->getNumberOfPlayerInstances();
    Instance ** playerInstances = playerManager->getPlayerInstances();
    
    for (int i = 0; i < numberOfPlayersThatNeedWeaponManagers; i++) {
        //Cast the playerInstance to a playerInstance
        if (playerInstances[i]->type != PLAYERENTITY) {
            if (PRINT_DEBUG_WARNING_MESSAGES) {
                std::cout << "\nDEBUG::WARNING! PlayerManager has a non-player instance at position" << i << " !\n";
            }
            continue;
        }
        Entity * pEnt = static_cast<Entity*>(playerInstances[i]);
        wepOverseer.generateAndAttachWeaponTrackerToInstance(pEnt);
        //pInst->configureWeaponTracker(); //do extra configuration on the weapon tracker
    }
    
    
    
    //gEntities.push_back(new RocketManager);
    //rocketManager = static_cast<RocketManager *>(gEntities[2]);
    
    
    //std::cout << std::endl << INDENT <<  "Loading background...\n";
    //this->background = new BackGroundGenerator;
    
    //std::cout << std::endl << INDENT << "Loading Stages...\n";
    //this->levels = new Stage* [NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD]; //Create an array of pointer to stage
//    for (int i = 0; i < NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD; ++i) {
//        //this->levels[i] = new Stage(i);
//        std::cout << INDENT << "    Level " << i+1 << " loaded..." << std::endl;
//    }
    ////initializeFromTemplate should be called by Stage constructor
    ////this->levels[0]->generator->initializeFromTemplate(<#const InitializationTemplate &#>)
    ////Generate a first level
    //levels[0]->generator->generateSingle();
    
    
    //void Game::loadShaders() {  }
    //void Game::loadTextures() {
    //Turns out I need a to bind an objectGenerators ->tex first before setting parameters on it (which makes sense)
    //    std::cout << std::endl << INDENT << "Configuring Texture Parameters..." << std::endl;
    //    std::cout << INDENT << "    " << "Setting Edge-Wrap: Set to CLAMP_TO_EDGE" << std::endl;
    
}



bool Game::launch() {

    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////    GAME LOOP     ///////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    
    while (glfwWindowShouldClose(mWindow) == false) {
        
        auto frameBegin = std::chrono::high_resolution_clock::now();
        
        //----------------------------------------------------------------------
        //  RESET OpenGL states to prepare for rendering next frame
        //----------------------------------------------------------------------
        
        glDepthFunc(GL_LESS); //Explicity tell the z-culling operation which direction to cull
        
        
        //----------------------------------------------------------------------
        //  CHECK INPUT from users
        //----------------------------------------------------------------------
        
        //Check for game-wide input directly
        //check to see if need to Exit loop by user pressing the escape key
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (PRINT_DEBUG_MESSAGES) {
                std::cout << "\nDEBUG::ESC KEY PRESS DETECTED. GAME WILL EXIT!\n";
            }
            glfwSetWindowShouldClose(mWindow, true); //This tells the render loop that this iteration will be its last
            continue; //Run the next loop iteration (which should close the program)
        }
        //Check to see if game should pause (pausing causes control to remain inside this block until an unpause occurs)
        //Need to put a delay so unpausing doesn't cause pausing on the next few frames
        //Ask "if enough frames have passed since the game was last unpaused and the unpause key is being pressed..."
        if (frameNumber >= (frameUnpaused + 10ull) && glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            auto begin = std::chrono::high_resolution_clock::now();  //Time Measurement
            auto end = std::chrono::high_resolution_clock::now(); //Note that time measurement is in nanoseconds
            std::cout << "Game Paused!\n"; //Announce that the game has been paused
            //To convert nanoseconds to seconds, see: http://convert-units.info/time/nanosecond/300000000
            while (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() < 300000000){  //this is saying wait 0.3 seconds before moving on to check for unpause
                std::this_thread::sleep_for(std::chrono::nanoseconds(3000000));
                end = std::chrono::high_resolution_clock::now();
            }
            //Okay, so now that a short pause has occured
            while (true) { //Infinite loop of checking if unpaused key get's pressed
                glfwPollEvents();
                if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
                    goto unpause;
                }
                else if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    glfwSetWindowShouldClose(mWindow, true);
                    goto unpause;
                }
                else {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(3333333)); //Sleep for a small amount of a second before checking again if unpaused.
                }
            }
        }
        //Need some way for unpausing to work. How about this:
        if (false) {
            unpause: std::cout << "Game Unpaused!" << std::endl;
            frameUnpaused = frameNumber;
        }
        
        //I went with a decentralized design (probably a mistake) where input reading is very decentralized
        //For each object in the game that can recieve user input, have that object check for and respond to that input
        std::vector<GameEntityManager*>::iterator entityInputIterator = gEntities.begin(); //I guess I could have just reused the same iterator for each time I iterate through gameEntities... Oh well
        for (; entityInputIterator < gEntities.end(); ++entityInputIterator) {
            (*entityInputIterator)->handleInput(mWindow);
        }
        
        
        //----------------------------------------------------------------------
        //  Perform Game Logic Calculation
        //----------------------------------------------------------------------
        //In logic, do:
        std::vector<GameEntityManager*>::iterator entityLogicIterator = gEntities.begin();
        //first, make sure that all the objects in game age at the same rate, so do
         for (; entityLogicIterator < gEntities.end(); ++entityLogicIterator) {
             (*entityLogicIterator)->ageObjects();
         }
        //Also tell the WeaponOverseer to age all currently active weapon instances
        wepOverseer.ageWeaponInstances();
        
        entityLogicIterator = gEntities.begin(); //Reset the iterator to the beginning of the vector
        //Loop through all the gameEntities and handle the rest of their required upkeep steps
        for (; entityLogicIterator < gEntities.end(); ++entityLogicIterator) {
            (*entityLogicIterator)->doUpkeep();
        }
        wepOverseer.doUpkeep();
        
        entityLogicIterator = gEntities.begin(); //Reset the iterator to the beginning of the vector
        //Loop through all the gameEntities and process any collisions that occured after all object's movement has been computed
        for (; entityLogicIterator < gEntities.end(); ++entityLogicIterator) {
            (*entityLogicIterator)->processCollisions(); //this is collision between entities
        }
        
        //Have the WeaponOverseer process all of the active WeaponTrackers to see if any flags were flagged
        wepOverseer.processWeaponTrackers();
        
        std::vector<WeaponInstance *> allActiveWeaponInstances;
        wepOverseer.getAllActiveWeaponInstances(allActiveWeaponInstances); //Have weaponOverseer fill the vector will all active weapon instances
        if (allActiveWeaponInstances.size() > 0) {  //Check to make sure there actually exist active weapon instances
            processInterEntityEvents(playerManager, allActiveWeaponInstances); //Check to see if entities of different types are running into eachother
        }
        //Not sure yet if this next line should be inside the 'if' statement right before this line, or outside 'if' statement
        wepOverseer.deleteFlaggedWeaponInstances(); //Delete any flagged weapon instances (they probably were flagged in Game.processInterEntityEvents() )
        
        
        //----------------------------------------------------------------------
        //  Draw
        //----------------------------------------------------------------------
       
        //-------  MSAA NOT WORKING TEST? ---------------------
        //see: https://developer.apple.com/library/content/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_fsaa/opengl_fsaa.html
        //glad_glEnable(GL_MULTISAMPLE);  //See also: https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
    
        //Gonna stick in some MSAA test code from OpenGL cookbook page 192
        if (PRINT_MSAA_INFO_FROM_GPU_DRIVER) {
            GLint bufs, samples;
            glad_glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
            glad_glGetIntegerv(GL_SAMPLES, &samples);
            printf("\nMSAA Information: buffers available = %d, samples = %d\n", bufs, samples);
        }
        //--------End of MSAA TEST CODE ------------------------
        
        std::vector<GameEntityManager*>::iterator entityDrawIterator = gEntities.begin();
        for (; entityDrawIterator < gEntities.end(); ++entityDrawIterator) {
            (*entityDrawIterator)->drawInstances();
        }
        //Draw weapon instances as well
        wepOverseer.drawInstances();
        
        //glad_glDisable(GL_MULTISAMPLE);
        //
        glBindVertexArray(0); //Vertex attribute array
        glUseProgram(0);
        
        //----------------------------------------------------------------------
        // Flip buffers and get ready for the next frame
        //----------------------------------------------------------------------
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Reset depth/color buffer bits
        ++frameNumber;
        if (PRINT_FRAME_PROCESS_TIME_INFORMATION) { //Might want to add more frame-time information then just process time / frame
            auto frameEnd = std::chrono::high_resolution_clock::now(); //Note that time measurement is in nanoseconds
            std::cout << "\nFrame " << frameNumber << " processed in " << std::chrono::duration_cast<std::chrono::nanoseconds>(frameEnd-frameBegin).count()  << " nanoseconds.";
        }
    }
    
    std::cout << "\nGame Exiting! Thanks for playing!" << std::endl; //Player Healths:\n"; //todo.. Implement a message that announces a winner
    
    
    if (PRINT_DEBUG_MESSAGES) {
        std::cout << "\nDEBUG::Exiting Game Loop!" << std::endl;
    }
    return true;
}


void Game::processInterEntityEvents(PlayerManager * pManag, std::vector<WeaponInstance*> activeWepInstances) {
    if (activeWepInstances.size() == 0) {return;}
    int playerInstanceCount = pManag->getNumberOfPlayerInstances();
    if (playerInstanceCount == 0) {
        std::cout << "\n!DEBUG::OOPS! There are no player instances for some reason...!\n";
        return;
    }
    //Get the player instances
    Instance ** pInstances = pManag->getPlayerInstances();
    PlayerEntity * player;
    //For each player in playerManager
    for (int i = 0; i < playerInstanceCount; i++) {
        //Cast the instance to be a player instance
        player = static_cast<PlayerEntity *> (pInstances[i]);
        CollisionBox * playersColBox = player->colBox;
        
        //For each active weapon instance
        std::vector<WeaponInstance*>::iterator wepIter = activeWepInstances.begin();
        for (; wepIter != activeWepInstances.end(); wepIter++) {
            //Check to make sure the weapon has existed for long enough to have left the collision box of the ship that fired it
            
            if ((*wepIter)->timeAlive < 2.0f * TIME_TICK_RATE )  {
                continue;
            }
            WeaponType wt = (*wepIter)->getWeaponType();
            switch (wt) {
                case KINETIC: //If the weapon is a kinetic weapon
                {
                    //cast to kinetic instance
                    Kinetic * kWepInst = static_cast<Kinetic*>((*wepIter));
                    CollisionBox * kineticColBox = kWepInst->colBox;
                    //Check to see if the weapon's collisionBox and the player's collisionBox are overlapping
                    if (kineticColBox->isOverlapping(*playersColBox) ){
                        //Do damage to player
                        player->health -= kWepInst->damage;
                        //Print the player's damage amount!
                        if (PRINT_PLAYER_DAMAGE_MESSAGES) {
                            std::cout << "Player " << player->playerNumber << " was hit! Player ";
                            std::cout << player->playerNumber << " has now taken ";
                            printf("%.4f", STARTING_PLAYER_HEALTH - player->health);
                            std::cout << " of damage!\n";
                        }
                        //Have kinetic impacts affect other player's velocity
                        aiVector2D velocityShift = kWepInst->velocity;
                        velocityShift = velocityShift.Normalize();
                        velocityShift *= KINETIC_VELOCITY_IMPACT;
                        player->velocity += velocityShift;
                        player->position = player->position + aiVector3D((velocityShift * 2.0f).x, (velocityShift * 2.0f).y, 0.0f);
                        
                        //player->wepTracker->  //No health in weapon tracker? What about shield?
                        //Mark the Kinetic instance for destruction
                        kWepInst->shouldBeDestroyed = true;
                    }
                    break;
                }
                case HEXAGON_BOMB:
                case LASER:
                case ROCKET:
                case HOMINGROCKET:
                    break;
                case UNINITIALIZED:
                default:
                    std::cout << "\nDEBUG::WARNING! Detected a weapon instance that was never initialized with a weapon type!\n";
                    break;
            }
        }
    }
}
