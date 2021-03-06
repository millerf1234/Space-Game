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
    
    playerParticleManager = nullptr;
    std::cout << "Done" << std::endl;
    
    std::cout << "Setting Up Audio...\n";
    audioListenerContext = std::make_unique<AudioRenderer>();
    std::cout << "Audio Set-Up Complete!\n";
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
        //if ((*iter)->getSpecType() == STAGE) { continue; }
        if (*iter != nullptr) {
            delete (*iter);
            (*iter) = nullptr;
        }
    }
//    if (this->stage != nullptr) {
//        delete this->stage;
//        this->stage = nullptr;
//    }
    
    
    if (playerParticleManager != nullptr) {
        delete playerParticleManager;
        playerParticleManager = nullptr;
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
    Stage * levelOne = new Stage();
    this->stage = levelOne;
    gEntities.push_back(levelOne);
    
    
        std::cout << INDENT << "Level " << /*i*/0+1 << " loaded..." << std::endl;
    //}
    //Since I am no longer doing the loop, do:
    //stage = static_cast<Stage*> (gEntities[0]); //Note: Looking at this code a month after I wrote it and not sure why I do (did) this
    
    
    //Then load weapons next (i.e. initiailize them, but don't actually spawn any yet...) Just get them ready for drawing.
    std::cout << std::endl << INDENT << "Loading Weapons...\n";
    wepOverseer.initializeWeapons();
    
    std::cout << "\n" << INDENT << "Loading Particle effects...\n";
    playerParticleManager = new PlayerParticleManager;
    std::cout << INDENT << "  Particle Effects loaded!\n";
    
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
    //initializeFromTemplate should be called by Stage constructor
    //this->levels[0]->generator->initializeFromTemplate(<#const InitializationTemplate &#>)
    //Generate a first level
    //levels[0]->generator->generateSingle();
    
    
    //void Game::loadShaders() {  }
    //void Game::loadTextures() {
    //Turns out I need a to bind an objectGenerators ->tex first before setting parameters on it (which makes sense)
    //    std::cout << std::endl << INDENT << "Configuring Texture Parameters..." << std::endl;
    //    std::cout << INDENT << "    " << "Setting Edge-Wrap: Set to CLAMP_TO_EDGE" << std::endl;
    
}



bool Game::launch() {
    
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
    
    

    // //////////////////////////////////////////////////////////////////////// //
    // /////////////////////////    GAME LOOP     ///////////////////////////// //
    // //////////////////////////////////////////////////////////////////////// //
    
    //FPS Counter? see: http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/
    while (glfwWindowShouldClose(mWindow) == false) {
        
        auto frameBegin = std::chrono::high_resolution_clock::now();
        
        //----------------------------------------------------------------------
        //  RESET OpenGL states to prepare for rendering next frame
        //----------------------------------------------------------------------
        
        //I don't need to do this inside the loop because it never changes in the loop
        // glDepthFunc(GL_LESS); //Explicity tell the z-culling operation which direction to cull
        
        
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
        
        // //Check to see if should toggle drawing collision details
        static unsigned long long frameThatInputWasLastDetected = 0ull; 
        if (glfwGetKey(mWindow, GLFW_KEY_0) == GLFW_PRESS) {
            if ( !( (frameNumber - frameThatInputWasLastDetected) < 15ull) ) {
                //Need to get access to a generator class so that I can toggle
                //it's static variable telling all generators whether or not to
                //draw collision details
                //Going to get the generator from Stage
                gEntities[0]->setGeneratorDrawCollision(!(gEntities[0]->getGeneratorDrawCollision()));
                //gEntities[0]->ge
//                if (TOGGLE_DRAW_COLLISION_DETAILS) {
//                    TOGGLE_DRAW_COLLISION_DETAILS = false;
//                }
//                else {
//                    TOGGLE_DRAW_COLLISION_DETAILS = true;
//                }
                //TOGGLE_DRAW_COLLISION_DETAILS = !TOGGLE_DRAW_COLLISION_DETAILS;
                std::cout << "Drawing CollisionDetails now set to: " << TOGGLE_DRAW_COLLISION_DETAILS;
                std::cout << "\n";
                frameThatInputWasLastDetected = frameNumber;
            }
        }
        
        //Check to see if game should pause (pausing causes control to remain inside this block until an unpause occurs)
        //Need to put a delay so unpausing doesn't cause pausing on the next few frames
        //"if enough frames have passed since the game was last unpaused" and "is the unpause key is being pressed..."
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
                    goto unpause;  ///Will Jump to 'unpause' (see below)
                }
                else if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    glfwSetWindowShouldClose(mWindow, true);
                    goto unpause; ///Will jump to 'unpause' (see below) and will set the game loop to exit
                }
                else {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(33333333)); //Sleep for a small amount of a second before checking again if unpaused.
                }
            }
        }
        //Need some way for unpausing to work. How about this:
        if (false) {
            ///THE go to statements above just go to here. It's not that bad!
            unpause: std::cout << "Game Unpaused!" << std::endl; ///Both exits from the pause loop will go to here
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
        
        playerParticleManager->doUpkeep(); //Do upkeep on the particles
        
        entityLogicIterator = gEntities.begin(); //Reset the iterator to the beginning of the vector
        //Loop through all the gameEntities and process any collisions that occured after all object's movement has been computed
        for (; entityLogicIterator < gEntities.end(); ++entityLogicIterator) {
            (*entityLogicIterator)->processCollisions(); //this is collision between entities
        }
        
        //Have the WeaponOverseer process all of the active WeaponTrackers to see if any flags were flagged
        wepOverseer.processWeaponTrackers();
        
        std::vector<WeaponInstance *> allActiveWeaponInstances;
        wepOverseer.getAllActiveWeaponInstances(allActiveWeaponInstances); //Have weaponOverseer fill the vector will all active weapon instances
        
        processInterEntityEvents(playerManager, allActiveWeaponInstances); //Handle inter-entity events (such as player death and player-weapon collision)
        
        wepOverseer.deleteFlaggedWeaponInstances(); //Delete any flagged weapon instances (they probably were flagged in Game.processInterEntityEvents() )
        
        
        //----------------------------------------------------------------------
        //  Draw
        //----------------------------------------------------------------------
       
        
        std::vector<GameEntityManager*>::iterator entityDrawIterator = gEntities.begin();
        for (; entityDrawIterator < gEntities.end(); ++entityDrawIterator) {
            (*entityDrawIterator)->drawInstances();
        }
        //Draw weapon instances as well
        wepOverseer.drawInstances();
        
        //Draw particles
        playerParticleManager->drawInstances();
        
        //glad_glDisable(GL_MULTISAMPLE);
        //
        glBindVertexArray(0); //Vertex attribute array
        glUseProgram(0);
        
        //----------------------------------------------------------------------
        // Flip buffers and get ready for the next frame
        //----------------------------------------------------------------------
        glfwSwapBuffers(mWindow); //this basically means present the frame buffer and aquire the next frame buffer to render the next frame into
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Reset depth/color buffer bits
        ++frameNumber;
        
        //---------------------------------------------------------------------
        // Print out frame process time statistics
        //---------------------------------------------------------------------
#ifdef CODE_IS_NOT_PRODUCTION_VERSION
        if(PRINT_FRAME_PROCESS_TIME_INFORMATION_PER_INTERVAL || PRINT_FRAME_PROCESS_TIME_INFORMATION_PER_FRAME){
            glFinish(); //Need glFinish() when timing rendering... This function call should be removed for production
            auto frameEnd = std::chrono::high_resolution_clock::now(); //Note that time measurement is in nanoseconds
            ///Frame Process Time Per Frame
            if (PRINT_FRAME_PROCESS_TIME_INFORMATION_PER_FRAME) { //Might want to add more frame-time information then just process time / frame
                static std::ostringstream frameTimesPrintoutBuffer;
                frameTimesPrintoutBuffer << "\nFrame " << frameNumber << " processed in " << std::chrono::duration_cast<std::chrono::nanoseconds>(frameEnd-frameBegin).count()  << " nanoseconds.";
                
                if (frameNumber % 60ull == 0) {
                    std::cout << frameTimesPrintoutBuffer.str() << std::endl;
                    //frameTimesPrintoutBuffer.flush(); //This doesn't actually empty the ostringstream
                    //But this does:
                    std::ostringstream().swap(frameTimesPrintoutBuffer); //This resets the frameTimesPrintoutBuffer OSS by constructing a new OSS and then swapping it with our frameTimesPrintoutBuffer OSS.
                }
            }
            ///Frame Process Time Per Interval
            if (PRINT_FRAME_PROCESS_TIME_INFORMATION_PER_INTERVAL) {
                //The following static variables are used to store frame-statistics
                static long double totalProcessingTime = 0.0l; //This only will be set once since it is static
                static long double minProcessingTime = std::numeric_limits<double>::infinity(); //or '= NAN'
                
                static long double maxProcessingTime = 0.0l;
                
                //Record the statistics
                long double frameProcessTime =(long double)//Calculate the time spent on the most recent frame
                (std::chrono::duration_cast<std::chrono::nanoseconds> (frameEnd - frameBegin).count());
                
                totalProcessingTime += frameProcessTime;
                if (frameProcessTime < minProcessingTime)
                    minProcessingTime = frameProcessTime;
                if (frameProcessTime > maxProcessingTime)
                    maxProcessingTime = frameProcessTime;
                
                
                if ((frameNumber % PRINT_FRAME_PROCESS_TIME_INFORMATION_INTERVAL_LENGTH) == 0ull) {
                    ///Change value from nanosecond to milliseconds
                    totalProcessingTime /= (long double)(1e6); //1 millisecond = 1e6 nanoseconds
                    maxProcessingTime /= (long double)(1e6);
                    minProcessingTime /= (long double)(1e6);
                    
                    fprintf(stderr, "\nFrame Statistic Information for the past %llu frames:\n",
                            PRINT_FRAME_PROCESS_TIME_INFORMATION_INTERVAL_LENGTH);
                    fprintf(stderr, "    Total Processing Time:   %08.2LF milliseconds\n",   totalProcessingTime); //'%LF' is 'long float'
                    fprintf(stderr, "    Maximum Processing Time: %08.2LF milliseconds\n", maxProcessingTime);
                    fprintf(stderr, "    Minimum Processing Time: %08.2LF milliseconds\n", minProcessingTime);
                    fprintf(stderr, "    Average Processing Time: %08.2LF milliseconds\n", totalProcessingTime / ( static_cast<long double> ( PRINT_FRAME_PROCESS_TIME_INFORMATION_INTERVAL_LENGTH)));
                    fprintf(stderr, "        Total Frames since program launch: %llu\n", frameNumber);
                    
                    //Reset the statistics to prepare for the next interval
                    totalProcessingTime = 0.0L;
                    minProcessingTime = std::numeric_limits<double>::infinity();
                    maxProcessingTime = 0.0L;
                    
                }
            }
        }
#endif //#ifdef CODE_IS_NOT_PRODUCTION_VERSION  
    }
    
    std::cout << "\nGame Exiting! Thanks for playing!" << std::endl;
    
    Instance ** pInstances = playerManager->getInstances();
    PlayerEntity * player1 = static_cast<PlayerEntity *> (pInstances[0]);
    PlayerEntity * player2 = static_cast<PlayerEntity *> (pInstances[1]);
    std::cout << "Player 1 Deaths: " << player1->deaths << std::endl;
    std::cout << "Player 2 Deaths: " << player2->deaths << std::endl;
    
    if (PRINT_DEBUG_MESSAGES) {
        std::cout << "\nDEBUG::Exiting Game Loop!" << std::endl;
    }
    return true;
}

void Game::processInterEntityEvents(PlayerManager * pManag, std::vector<WeaponInstance*> activeWepInstances) {
    if (activeWepInstances.size() == 0) {
        //If there are no active weapons instances, then just do the logic for dead players and move on
        pManag->processPlayerDeaths();
        return;
    }
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
                //continue; //Continue will cause the outer loop to continue...
                break; //break causes the inner loop to continue
            }
            WeaponType wt = (*wepIter)->getWeaponType();
            switch (wt) {
                case KINETIC: //If the weapon is a kinetic weapon
                {
                    //cast to kinetic instance
                    Kinetic * kWepInst = static_cast<Kinetic*>((*wepIter));
                    CollisionBox * kineticColBox = kWepInst->colBox;
                    //Check to see if the weapon's collisionBox and the player's collisionBox are overlapping
                    if (kineticColBox->isOverlapping(*playersColBox) && !(player->isDead) ){
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
                        
                        //Have kinetic do damage to shields?
                        
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
        
        //Check to see if player died
        if (player->health <= 0.0f) {
            std::cout << "\nPlayer " << player->playerNumber << " died! ";
            std::cout << "Player has died " << player->deaths + 1 << " times!\n";
            //Need the + 1 because the player death hasn't been processed at this point
            player->shouldDieFlag = true;
           
            if (PLAY_DEATH_ANIMATION) {
                //Record the game state in a struct
                DeathAnimation::GameStateInfoForDeathAnimation * gameStateAtPlayersDeath = new DeathAnimation::GameStateInfoForDeathAnimation;
                captureGameState(gameStateAtPlayersDeath);
              
                DeathAnimation::playDeathAnimation(gameStateAtPlayersDeath, player, playerParticleManager);
                
                //Clean up memory used in capturing game state
                delete gameStateAtPlayersDeath;
                gameStateAtPlayersDeath = nullptr;
            }
        }
        pManag->processPlayerDeaths(); //Have player manager process the player's death
    }
}

void Game::captureGameState(DeathAnimation::GameStateInfoForDeathAnimation * state) {
    state->stage = this->stage;
    state->playerManager = this->playerManager;
    state->weaponOverseer = &(this->wepOverseer);
    state->gEntities = this->gEntities;
    state->mWindow = this->mWindow;
}
