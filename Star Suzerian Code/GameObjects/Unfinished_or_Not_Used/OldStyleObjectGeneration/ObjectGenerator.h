////  ObjectGenerator.h
////
////  The way an object generator works is when the Game is first launched, for each
////  type of object in the game (player, ai-player, weapon, background, etc...)
////  a corrosponding objectGenerator will be initialized. The object generator will
////  load data from files for the object and will also handle making sure all of the
////  necessary shaders for the object are good to go. Instances of an object can then be
////  created on demand.
////
////
////  Created by Forrest Miller on 2/15/18.
////
//
//#ifndef ObjectGenerator_h
//#define ObjectGenerator_h
//
//#include <iostream>
//#include "glad.h"
//#include "SimpleObjLoader.h"
//#include "ShaderWrapper.h"
//#include "Quaternion.h"
//#include "Object.h"
//#include "GameParameters.h" //Contains constants for the game
//#include "TextureWrapper.h"
//
//class Object; //Need to do this because of circular headder reference I have.
////              See Object.h for more details
//
//class ObjectGenerator {
////protected: //Need to access some of this stuff, add accessors to this class at a later date 
//public:
//    GLfloat * vertices; //Model data
//    GLuint * vbo; //Model Data Buffer location
//    //GLfloat *vbo, * vertices; //I had vbo as a float for some reason
//    GLuint * ebo, * elements; //Elements in the data to quote
//    //GLuint * tex; //Moved this to be inside TextureWrapper
//    int numberOfVertices, numberOfElements;
//    
//    bool hasModelData;
//    bool hasValidShader;
//    static int nextObjID; //For dealing with static class members, see: https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/cplr038.htm
//    
//public:
//    //Public variables
//    ShaderWrapper * shader;
//    int numberOfShaders;
//    SimpleObjLoader * objFileData;
//    int activeInstances; // activeInstancesOfObject
//    
//    //Constructors
//    //Note that can't have virtual constructors. See //https://www.geeksforgeeks.org/advanced-c-virtual-constructor/
//    ObjectGenerator();
//    //ObjectGenerator(const SimpleObjLoader &);
//    //ObjectGenerator(const ShaderWrapper &);
//    //ObjectGenerator(const SimpleObjLoader &, const ShaderWrapper &);
//    //ObjectGenerator(const char * objFile);
//    //ObjectGenerator(const char * objFile, const ShaderWrapper &);
//    
//    //Destructor
//    virtual ~ObjectGenerator();
//    
//    //Functions
//    virtual Object* generateInstance() = 0;
//    //virtual Object copy() = 0;
//    
//    bool checkIfReadyToGenerate() const {return (hasModelData && hasValidShader);}
//    
//   // virtual void setStartingOrientation(Quaternion *xRot, Quaternion *yRot, Quaternion *zRot) = 0;
//    
//    virtual aiVector3D getStartingOrientation() = 0; //Returns a vector of whatever direction 'forward' is on the model
//    
//    //void decreaseActiveInstances() {this->activeInstances -= 1;}
//    
//    //Some generic getter functions:
//    bool hasVbo() const {return (this->vbo != nullptr);}
//    bool hasVertices() const {return (this->vertices != nullptr);}
//    bool hasEbo() const {return (this->vbo != nullptr);}
//    bool hasElements() const {return (this->elements != nullptr);}
//    //bool hasTexBuffer() const {return (this->tex != nullptr);}
//    
//    //Returns -1 if not found, and also glGetUniformLocation returns -1 if not found
//    GLint getUniformLocation(int shaderProgramToUse, GLchar *name) {
//        if (this->hasValidShader) {
//            GLuint * programID = (this->shader[shaderProgramToUse].programID);
//            if (programID != nullptr) { //This check should never fail, but checking prevents program from crashing if it does fail
//                return glGetUniformLocation(*programID, name); //Dereference the pointer
//            }
//            else { //If this check went off, then 99% of the time it will be because of wrong shaderProgramToUse input
//                std::cout << "\nError attempting to access Uniform Location for " << (char*)name << " in shader because provided shaderProgramID was null" << std::endl;
//                std::cout << "This should never happen if code was written correctly, so check your code.\n";
//                std::cout << "(Try subtracting one from your programToUse parameter in function 'getUniformLocation()') " << std::endl;
//                return -1;
//            }
//        }
//        else {
//            std::cout << "Error getting uniform location. Shader is invalid" << std::endl;
//            return -1;
//        }
//        return -1;
//    }
//};
//
////class Object {
////private:
////    float x,y,z,w; //Coordinates of centerpoint
////    //Collision box * collisionBox //Add this in later
////
////    GLfloat * vbo, * vertices;
////    GLuint * ebo, * elements;
////    SimpleObjLoader * objFileData;
////
////    ShaderWrapper * shaders;
////
////    Quaternion *xRot, *yRot, *zRot; //rotation quaternions
////
////    bool hasModelData, modelValid;
////    bool hasShaderWrapper, shaderValid;
////    bool stationary; //Says whether this object should move or not
////    int shaderCount;
////
////
////    //collisionBox * collisionBox
////
////public:
////    Object();
////    Object(const SimpleObjLoader &);
////    Object(const ShaderWrapper &);
////    Object(const SimpleObjLoader &, const ShaderWrapper &);
////    Object(const char * objFile);
////    Object(const char * objFile, const ShaderWrapper &);
////
////
////    //Functions for building model
////    virtual void loadObjFile(const char * file);
////    virtual void attachObjFile();
////    virtual void attachShaderWrapper();
////
////    //Getters
////    bool hasModel();
////    bool CheckModelValid();
////    bool hasShader();
////    bool checkShaderValid();
////    bool isStationary(); //Returns true if an object is not supposed to move
////    //getCollisionBox
////
////    //Model functions
////    virtual void translate(aiVector2D) = 0;
////    virtual void translate(aiVector3D) = 0;
////
////    virtual void drawSelf() = 0;
////    virtual void wasDestroyed() = 0;
////
////};
//
//
//
//#endif /* ObjectGenerator_h */

