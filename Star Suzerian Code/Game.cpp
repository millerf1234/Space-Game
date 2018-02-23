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
    std::cout << INDENT << "Generating Global Rotation Quaternions..." << std::endl;
    xRot = new Quaternion(1.0f, 0.0f, 0.0f, 0.0f); //rotation about x axis
    yRot = new Quaternion(0.0f, 1.0f, 0.0f, 0.0f); //rotation about y axis
    zRot = new Quaternion(0.0f, 0.0f, 1.0f, 0.0f); //rotation about z axis
    this->levels = nullptr;
    this->numberOfLevels = 0; //Start with 0 levels
    counter = 0.0f; //Set game clock to 0
    
    this->mWindow = mPtr.activeMonitor;
    
    
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
    //I replaced 'background' with 'stage', and 'stage' will be an array
//    if (this->background != nullptr) {
//        delete this->background;
//        this->background = nullptr;
//    }
    
    //Not sure if this next part should be removed or not?
/*    //Delete levels
    if (this->levels != nullptr) {
        //Delete each level
        for (int i = 0; i < this->numberOfLevels; ++i) {
            if (this->levels[i] != nullptr) {
                delete this->levels[i];
                this->levels[i] = nullptr;
            }
        }
        //Delete the array of levels
        delete [] this->levels;
        this->levels = nullptr;
    } */
 
    //Delete game entities in the gameEntityManager array
    std::vector<GameEntityManager *>::iterator iter;
    for (iter = gEntities.begin(); iter != gEntities.end(); ++iter) {
        if (*iter != nullptr) {
            delete (*iter);
            (*iter) = nullptr;
        }
    }
    //Gotta delete the pointer at the end of the vector as well
//    if (*iter != nullptr) {
//        delete (*iter);
//        (*iter) = nullptr;
//    }
}

void Game::playIntroMovie() {  }

void Game::loadGameObjects() {
    std::cout << std::endl << INDENT << "Loading Stages...\n";
    for (int i = 0; i < NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD; ++i) {
        gEntities.push_back(new Stage);
        std::cout << INDENT << "    Level " << i+1 << " loaded..." << std::endl;
    }
    
    std::cout << std::endl << INDENT << "Loading PlayerModels...\n";
    gEntities.push_back(new PlayerManager);
    
    
    
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
    //Turns out I need a to bind an objectGenerators ->tex first before setting parameters on it
    //    std::cout << std::endl << INDENT << "Configuring Texture Parameters..." << std::endl;
    //    std::cout << INDENT << "    " << "Setting Edge-Wrap: Set to CLAMP_TO_EDGE" << std::endl;
    
}

bool Game::launch() {
    
    
    //bool loopOver = false;
    
    
    while (glfwWindowShouldClose(mWindow) == false) {
        //Reset() {
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //}
        
        //Handle Input() {
        //Exit loop by pressing the escape key
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(mWindow, true);
        }
        //}
        
        //Logic () //Includes Upkeep and collision detection and everything else
        //In logic, do:
        std::vector<GameEntityManager*>::iterator it2 = gEntities.begin();
        for (; it2 < gEntities.end(); ++it2) {
            (*it2)->doUpkeep();
        }
        //levels[0]->generator->doUpkeep();
        
        
        //Draw() {
        //Draw background
        /// this->background
        //...
        //levels[0]->generator->drawInstances();
        std::vector<GameEntityManager*>::iterator it = gEntities.begin();
        for ( ; it < gEntities.end(); ++it) {
            (*it)->drawInstances();
        }
        
        glBindVertexArray(0); //Vertex attribute array
        glUseProgram(0);
        //}
        
        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Reset depth/color buffer bits
        ++frameNumber;
    }
    
    return true;
}



//   OLDE DEBUG CODE:
////----------------------------------------------------------------------------
////  FOR SOME REASON MY CODE ISN'T WORKING SO I'm Going to quickly set up a debug
////  Triangle to draw to see if i can figure out what is going wrong...
////----------------------------------------------------------------------------
//ShaderWrapper * debugShader = new ShaderWrapper;
//debugShader->attachVert((char*)("/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/debug.vert"));
//debugShader->attachFrag((char*)("/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/background.frag"));
//
//debugShader->link();
//debugShader->setVertexAttribName("position");
////debugShader->specifyVertexLayout(ShaderWrapper::VERT2); //Move to after vbo/ebo creation
//
//GLfloat vertices[10000 * 5];
//for (GLfloat f : vertices) {
//    f = 0.56f;
//}
//for (int i = 0; i < 100 * 2; ++i) {
//    if (i % 2 == 0) {
//        vertices[i] = -1.0f + (((float)i)/100.0f);
//    }
//    else {
//        vertices[i] = 1.0f - (((float)i)/100.0f);
//    }
//}
//GLuint vbo;
//glGenBuffers(1, &vbo);
//glBindBuffer(GL_ARRAY_BUFFER, vbo);
////glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//GLuint ebo; //element buffer element object
//glGenBuffers(1, &ebo);
//
//int faceCounter = 0;
////This is the version of elements that actually works properly
//GLuint elements[100];
//for (unsigned int i = 0; i < 100u; ++i) {
//    elements[i] = i;
//}
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
////glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_DYNAMIC_DRAW);
//
//debugShader->specifyVertexLayout(ShaderWrapper::VERT2, vbo, ebo);
//
////----------------------------------------------------------------------------
//// END DEBUG TRIANGLE SETUP
////----------------------------------------------------------------------------
//
// Then to draw it:
//debugShader->use(); //Use the debug shader
//glBindBuffer(GL_ARRAY_BUFFER, vbo);
//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_DYNAMIC_DRAW);
//
//glDrawElements(GL_LINES, 100*2, GL_UNSIGNED_INT, 0);

