//  Generator.h
//
//  IMPORTANT!!! Generator will 'own' all of its dynamic memory at all times. It
//               is very important to NEVER delete any pointers that are given
//               out by Generator. Additionally, if you get instance pointers from Generator
//               and you ask generator to delete one of them, this will invalidate
//               every pointer given out by Generator, so all the current instance
//               pointers in use outside of generator must be reaquired. (could
//               move aquiring instance pointers into a wrapper class interface
//               (i.e. a wrapper for each pointer) such that it can be marked
//               whenever all of the pointers given out by generator are invalidated)
//
//  Description
//  What is the Generator class, you might ask? Unfortunatly, the answer
//  is not a simple one. In a broad sense, a generator is responsible for
//  _______ (Too many things to write). Yes, I realize that generator is not at all
//  good design and eventually if/when I get time I will break up Generator into
//  more distinct classes. Until then, the idea behind the architecture is that
//  every GameEntityManager class will have its own Generator from which it uses
//  to initialize all the required information for that type of entity it is managing,
//  and then can generate instances of that object on command. In reality though
//  I've made so many changes to the design during the process that right now generator is a huge mess.
//
//  Currently initialization templates only support 1 texture, 1 shader program,
//  1 model and 1 collision box. Since each player requires multiple shaders, I
//  wound up hardcoding a lot of the extra stuff required for player right into
//  the generator code. Eventually this will all be moved out into a special loader
//  class or something else like that.
//
//
//  Version history:
//                   version -0.1 Created by Forrest Miller on 2/15/2018  (version is negative
//                                            because I completely scrapped this version)
//                   version 0.1  Rewrite started on 2/17/2018
//                   version 1.0  Got it working with stage and Player managers by 2/28/2018
//                   version 1.09 Started working on getting weapons working with generator  3/13/2018
//                   version 1.1  Got weapons working on 3/21/2018 (on Wednesday of finals week!)
//                   version 1.11 Added rng to weapon spawning 3/27/2018


#ifndef Generator_h
#define Generator_h

#include <iostream>
#include <assimp/Importer.hpp> //Gives access to the aiVector3D
#include <vector>
#include <random>

#include "ShaderWrapper.h" //Contains class that provides interface for shaders
#include "SimpleObjLoader.h" //Contains class that handles '.obj' files
#include "TextureWrapper.h" //Contains struct that handles textures
#include "glad.h" //For GL types and functions
#include "GameParameters.h"
#include "CollisionBox.h"

//#include "PlayerData.h"
//#include "WeaponData.h"
//#include "PlayerManager.h" //lots of circular includes I got. Oh well...
#include "Instance.h"
#include "Kinetic.h"
#include "MathFunctions.h"

//constexpr int FILEPATH_BUFFER = 512;

//Note:
//specializationType is assigned to individual instances to identify them
//specializationType is assigned to a GameEntityManager-derived class
enum specializationType {PLAYER, WEAPON, STAGE, NOSPECIALIZATION};

//The idea here is to have this large class called Generator that
typedef struct InitializationTemplate{
    bool hasVert, hasTessl, hasFrag, hasGeom;
    bool hasVertsAlreadyLoaded;
    bool hasTexture;
    GLfloat * vertices;
    int numVerts, numElems;
    GLuint * elements;
    bool vert2, vert3, vert2tex2, vert2col3tex2, vert3tex2, vert3tex3, vert3norml3, vert2norml2tex2, vert3norml3tex3, vert3tex3norml3, vert3norml3tex2;
    
    std::string modelFilePath;
    std::string textureFilePath;
    std::string vertShaderPath;
    std::string tesslShaderPath;
    std::string geomShaderPath;
    std::string fragShaderPath;
    
    std::string vertAttribName;
    std::string texAttribName;
    std::string normalAttribName;
    std::string colAttribName;
    specializationType typeDataRequired;
    //Constructor
    InitializationTemplate() {
        //Set all bools to false
        hasVert = hasTessl = hasFrag = hasGeom = false;
        hasVertsAlreadyLoaded = false;
        hasTexture = false;
        vertices = nullptr;
        elements = nullptr;
        numVerts = numElems = 0;
        vert2 = vert3 = vert2tex2 = vert2col3tex2 = vert3tex2 = vert3tex3 = vert3norml3 = vert2norml2tex2 = vert3norml3tex3 = vert3tex3norml3 = vert3norml3tex2 = false;
        typeDataRequired = specializationType::NOSPECIALIZATION;
        //Set all pointers to nullptr
       // modelFilePath = textureFilePath = vertShaderPath = tesslShaderPath = geomShaderPath = fragShaderPath = vertAttribName = texAttribName = normalAttribName = colAttribName = nullptr;
    }
} InitializationTemplate;


class Generator{
private:
    static int nextObjID; //The ID for the next generated object (is static so each object gets own unique ID)
    int activeInstances; //Number of active instances of this object
    
    
    
    
    //Special cases to be called within the drawInstances() function
    void doDrawPlayerShipInstance(int); //This function currently
    void doDrawWeaponInstance(int); //This function draws all instances at onces
    //void drawKineticInstances(WeaponInstance **); //Called by doDrawWeaponInstances to draw the Kinetic instances
    void drawKineticInstance(Kinetic *);
    
protected:
    //-------------------
    //Fields
    //-------------------
    
    //This section is all data necessary for rendering (shaders, models, textures, etc..)
    //Master Copy of Object's necessary data:
public: //temporary for debug
    GLfloat * vertices; //Object's vertice data
    GLuint vbo, ebo, /*tex,*/ *elements; //Track this objects vertices and elements on GPU with trackers vbo and ebo, and also use the array 'elements' to contain the order to draw vertices
protected: //temporary for debug
    int numberOfVertices, numberOfElements; //Size of the arrays 'vertices' and 'elements'
    
    //GLfloat stackVertices[1000]; //These 2 were just for debug testing,
    //GLuint stackElements[500];
    
    //Textures and Shaders
    ShaderWrapper ** shaderArray; //Array of pointer to ShaderWrapper
    TextWrapr ** textureArray; //array of pointer to TextWrapr
    SimpleObjLoader * modelLoader;
    int shaderArraySize, textureArraySize;
    
    //bool drawTriangles, drawLines; //No longer used
    
    ///Really I am lame and didn't think to incorperate uniform locations into my shadder wrapper
    ///when I was first writing this code. So that's why I have this:
    //Player uniform locations (I need extra uniform locations to accomodate the extra shaders):
    GLint ulocStageAspectRatio;
    
    typedef struct PlayerUniformLocations {
    GLint ulocRed, ulocGreen, ulocBlue; //Uniform locations for PLAYER rgb (color)
    GLint ulocRed_Line, ulocGreen_Line, ulocBlue_Line;
    GLint ulocTime_Line, ulocZoom_Line, ulocXTrans_Line, ulocYTrans_Line, ulocZTrans_Line, ulocThetaX_Line, ulocThetaY_Line, ulocThetaZ_Line;
    GLint ulocTimeEngine, ulocZoomEngine, ulocXTransEngine, ulocYTransEngine, ulocZTransEngine, ulocThetaXEngine, ulocThetaYEngine, ulocThetaZEngine;
    GLint ulocTimeEngineSide, ulocZoomEngineSide, ulocXTransEngineSide, ulocYTransEngineSide, ulocZTransEngineSide, ulocThetaXEngineSide, ulocThetaYEngineSide, ulocThetaZEngineSide;
    GLint ulocPDamage, ulocPHealthMax;
        
    GLint ulocEngineMainRedinate, ulocEngineMainBlueify, ulocEngineSideRedinate, ulocEngineSideBlueify; //lol these names. Basically these are red/blue tinge factors for making the engines flames unique for each ship color
    GLint ulocRoll, ulocPlayerRoll_Line, ulocPlayerRollEngineSide; //Don't need to track roll for main engine
    } PlayerUniformLocations;
    
    PlayerUniformLocations pul;
    
    //Weapon uniform locations:
    //Kinetic weapons
    typedef struct KineticUniformLocations {
        //GLint ulocXSpawnOffset, ulocYSpawnOffset, ulocZSpawnOffset; //Not sure if I need these?
    } KineticUniformLocations;
    KineticUniformLocations kul;
    
    // This section
    //Instance data
    //int instanceCount; //Redundant with activeInstances
    Instance ** instances;
    std::vector<int> instancesCreatedByThisGenerator; //A vector of instance IDs
    
    //Uniform location variables
    GLint ulocTime, ulocZoom, ulocXTrans, ulocYTrans, ulocZTrans, ulocThetaX, ulocThetaY, ulocThetaZ;
    
public:
    //-------------------
    // Constructor and Destructor
    //-------------------
    Generator();
    Generator(const Generator &) = delete;//Remove copy constructor from class
    ~Generator();
    
    //-------------------
    // Initialization functions
    //-------------------
    void initializeFromTemplate(const InitializationTemplate &);
    void setModel(const char * modelFilePath);
    void setModelFromVertArray(const GLfloat * verts, const GLuint * elems, int numberOfVerts, int numberOfElems);
    void setTexture(const char * texImgFilePath);
    void setShader(const char * vertShader, const char * fragShader);
    //Check initialization
    bool /* hasVbo(), hasEbo(),*/ hasVerts(), hasElems(), hasShader(), hasTexture(), hasModel(), readyToGenerate();
    bool wasInitialized;
    
    specializationType specialization ;
    void setSpecialization(specializationType expansionType);
    
    bool shouldGenerateEntities;
    
    void setDrawCollisionDetails(bool drawCollision);
    bool getDrawCollisionDetails() const; 
    
    //--------------------
    // Instance Controls
    //--------------------
    void generateSingle(); //Create a single new instance
    //void generate(int numberToGenerate); //Create a number of new instance
    void doUpkeep();
    void processUserInput();
    int getInstanceCount() const {return this->activeInstances;}
    void drawInstances();
    void convertTrianglesIntoLines();
    void convertLinesIntoTriangles(); //Need a way to toggle shaders? Nah
    Instance ** getArrayOfInstances() {return instances;}
    
    void removeInstance(const int & instantID); 
    void removeInstance(Instance *);
    
    
    //I am adding a function to retrieve the loaded model from this generator so that it can be used for
    //calculations
    SimpleObjLoader * getModel() {
        return (this->hasModel() ? (this->modelLoader) : (nullptr) );
    }
};

#endif /* Generator_h */
