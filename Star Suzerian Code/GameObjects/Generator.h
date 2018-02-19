//  Generator.h
//
//  This is a redone version of my first ObjectGeneration class. I want to have there
//  be a more distinct difference between an object and an objectGenerator.
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
#include "ShaderWrapper.h" //Contains class that provides interface for shaders
#include "SimpleObjLoader.h" //Contains class that handles '.obj' files
#include "TextureWrapper.h" //Contains struct that handles textures
#include <assimp/Importer.hpp> //Gives access to the aiVector3D
#include "glad.h" //For GL types and functions
#include "GameParameters.h"


constexpr int FILEPATH_BUFFER = 512;

//Here is a typedef for an instance of what was generated
typedef struct Instance {
public:
    int identifierNumber; //Unique number attached to each instance of any object
    aiVector3D position;
    float zoom;
    float thetaX, thetaY, thetaZ; //Euler Rotation Angles
    //collisionBox
    float timeAlive;
    
    //One constructor to give this instance an ID number
    Instance() {;}
    Instance(int id) {this->identifierNumber = id;}
    
    //One function attached to getID()
    int getID() const {return this->identifierNumber;}
} Instance;

typedef struct InitializationTemplate{
    //Hold filepaths
//    char modelFilePath[FILEPATH_BUFFER];
//    char textureFilePath[FILEPATH_BUFFER];
//    char vertShaderPath[FILEPATH_BUFFER];
//    char tesslShaderPath[FILEPATH_BUFFER];
//    char geomShaderPath[FILEPATH_BUFFER];
//    char fragShaderPath[FILEPATH_BUFFER];
    bool hasVert, hasTessl, hasFrag, hasGeom;
    bool hasVertsAlreadyLoaded;
    GLfloat * vertices;
    int numVerts, numElems;
    GLuint * elements;
    bool vert2, vert3, vert2tex2, vert2col3tex2, vert3tex2, vert3tex3, vert2norml2tex2, vert3norml3tex3, vert3tex3norml3, vert3norml3tex2;
//    char * vertAttribName[FILEPATH_BUFFER];
//    char * texAttribName[FILEPATH_BUFFER];
//    char * normalAttribName[FILEPATH_BUFFER];
//    char * colAttribName[FILEPATH_BUFFER];
    
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
    
//
    InitializationTemplate() { //Constructor
        //Set all bools to false
        hasVert = hasTessl = hasFrag = hasGeom = false;
        hasVertsAlreadyLoaded = false;
        vertices = nullptr;
        elements = nullptr;
        numVerts = numElems = 0;
        vert2 = vert3 = vert2tex2 = vert2col3tex2 = vert3tex2 = vert3tex3 = vert2norml2tex2 = vert3norml3tex3 = vert3tex3norml3 = vert3norml3tex2 = false;
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
    GLfloat * vertices; //Object's vertice data
    GLuint vbo, ebo, /*tex,*/ *elements; //Track this objects vertices and elements on GPU with trackers vbo and ebo, and also elements contains the order to draw vertices
    int numberOfVertices, numberOfElements; //Size of the arrays 'vertices' and 'elements'
    
    GLfloat stackVertices[1000];
    GLuint stackElements[500];
    
    //Textures and Shaders
    ShaderWrapper ** shaderArray;
    TextWrapr ** textureArray;
    SimpleObjLoader * vertexData;
    int shaderArraySize, textureArraySize;
    
    //Instance data
    //int instanceCount; //Redundant with activeInstances
    Instance * instances;
    
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
    
    
    //--------------------
    // Instance Controls
    //--------------------
    void generateSingle(); //Create a single new instance
    void generate(int numberToGenerate); //Create a number of new instance
    void doUpkeep();
    void processUserInput();
    int getInstanceCount() const {return this->activeInstances;}
    void drawInstances();
    Instance * getArrayOfInstances();
    
    
    
};

#endif /* Generator_h */
