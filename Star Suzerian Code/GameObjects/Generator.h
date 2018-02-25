//  Generator.h
//
//  I have rewritten/repurposed this class several times, and as a result it is a
//  bit of a mess. This class also does a lot of the behind-the-scenes work for the
//  game, so it is used a lot. At some point, this class ideally will be rewritten
//
// *********************************************************************************
//  Old comment headders:
//
//  This is a redone version of my first ObjectGeneration class. I want to have there
//  be a more distinct difference between an object (now called 'instance') and an objectGenerator.
//
//
//  Here is the header I had written for ObjectGenerator:
//     -------------------------------------------------------------------
//       ObjectGenerator.h
//
//       The way an object generator works is when the Game is first launched, for each
//       type of object in the game (player, ai-player, weapon, background, etc...)
//       a corrosponding objectGenerator will be initialized. The object generator will
//       load data from files for the object and will also handle making sure all of the
//       necessary shaders for the object are good to go. Instances of an object can then be
//       created on demand.
//       (was) Created by Forrest Miller on 2/15/18.
//     -------------------------------------------------------------------
//  New version Created by Forrest Miller on 2/17/18.
//

#ifndef Generator_h
#define Generator_h

#include <iostream>
#include <assimp/Importer.hpp> //Gives access to the aiVector3D
#include <vector>

#include "ShaderWrapper.h" //Contains class that provides interface for shaders
#include "SimpleObjLoader.h" //Contains class that handles '.obj' files
#include "TextureWrapper.h" //Contains struct that handles textures
#include "glad.h" //For GL types and functions
#include "GameParameters.h"
//#include "PlayerData.h"
//#include "WeaponData.h"
//#include "PlayerManager.h" //lots of circular includes I got. Oh well...
#include "Instance.h"

//constexpr int FILEPATH_BUFFER = 512;

//Note:
//InstanceType is assigned to individual instances to identify them
//specializationType is assigned to a GameEntityManager-derived class
enum specializationType {PLAYER, WEAPON, STAGE, NOSPECIALIZATION};

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
    
protected:
    //-------------------
    //Fields
    //-------------------
    //Master Copy of Object's necessary data:
public: //temporary for debug
    GLfloat * vertices; //Object's vertice data
    GLuint vbo, ebo, /*tex,*/ *elements; //Track this objects vertices and elements on GPU with trackers vbo and ebo, and also elements contains the order to draw vertices
protected: //temporary for debug
    int numberOfVertices, numberOfElements; //Size of the arrays 'vertices' and 'elements'
    
    //GLfloat stackVertices[1000]; //These 2 were just for debug testing,
    //GLuint stackElements[500];
    
    //Textures and Shaders
    ShaderWrapper ** shaderArray;
    TextWrapr ** textureArray;
    SimpleObjLoader * vertexData;
    int shaderArraySize, textureArraySize;
    
    bool drawTriangles, drawLines;
    
    //Instance data
    //int instanceCount; //Redundant with activeInstances
    Instance ** instances;
    std::vector<int> instancesCreatedByThisGenerator; //A vector of instance IDs
    
    //Uniform location variables
    GLint ulocTime, ulocZoom, ulocXTrans, ulocYTrans, ulocZTrans, ulocThetaX, ulocThetaY, ulocThetaZ;
    
    //Player uniform locations:
    GLint ulocRed, ulocGreen, ulocBlue; //Uniform locations for PLAYER rgb (color)
    GLint ulocRedLine, ulocGreenLine, ulocBlueLine;
    GLint ulocTimeLine, ulocZoomLine, ulocXTransLine, ulocYTransLine, ulocZTransLine, ulocThetaXLine, ulocThetaYLine, ulocThetaZLine;
    GLint ulocTimeEngine, ulocZoomEngine, ulocXTransEngine, ulocYTransEngine, ulocZTransEngine, ulocThetaXEngine, ulocThetaYEngine, ulocThetaZEngine;
     GLint ulocTimeEngineSide, ulocZoomEngineSide, ulocXTransEngineSide, ulocYTransEngineSide, ulocZTransEngineSide, ulocThetaXEngineSide, ulocThetaYEngineSide, ulocThetaZEngineSide;
    GLint ulocPDamage, ulocPHealthMax;
    GLint ulocPlayerRoll, ulocPlayerRollLine, ulocPlayerRollEngineSide; //Don't need to track roll for main engine 
    //Weapon uniform locations:
    
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
    void convertLinesIntoTriangles(); //Need a way to toggle shaders?
    Instance ** getArrayOfInstances() {return instances;}
    
    void removeInstance(const int & instantID); 
    void removeInstance(Instance *);
};

#endif /* Generator_h */
