////  Contains a class for a background of a map (might put in multiple backgrounds,
////  with a menu system to choose between them when program launches, or might
////  just stick with one background.
////
////  Background.h
////
////  Created by Forrest Miller on 2/14/18.
//
//#ifndef Background_h
//#define Background_h
//
//#include <iostream>
//#include "Object.h"
//#include "ObjectGenerator.h"
//#include "TextureWrapper.h"
//
//static constexpr int TEXTURESQUARE_VERTS_SIZE = 4*(2+3+2);
//static constexpr GLfloat TEXTURESQUARE_VERTS[] = {
//    // Position                     Color                      Texture (U/V)
//    -1.0f,  1.0f,            0.0f,   0.0f,   0.0f,            0.0f, 0.0f, //  Top-left corner   //0
//     1.0f,  1.0f,            0.0f,   0.0f,   0.0f,            1.0f, 0.0f, //  Top-right corner  //1
//     1.0f, -1.0f,            0.0f,   0.0f,   0.0f,            1.0f, 1.0f, // Lower-right corner //2
//    -1.0f, -1.0f,            0.0f,   0.0f,   0.0f,            0.0f, 1.0f  // Lower-left corner  //3
//};
//
//static constexpr int TEXTURESQUARE_ELEMENTCOUNT = 2*3;
//static constexpr GLuint TEXTURESQUARE_ELEMENTS[] {
//    0, 1, 2, //Triangle 1 (top-left, top-right, lower-right)
//    2, 3, 0  //Triangle 2 (lower-right, lower-left, top-left)
//};
//
////                   the ": public" means that this class inherits from ObjectGenerator
//class BackGroundGenerator : public ObjectGenerator {
//public:
//    //Constructor
//    BackGroundGenerator(); //Sets up a background
//    //Destructor
//    virtual ~BackGroundGenerator() override; //Handles deallocating memory claimed by background
//    
//    int getNumberOfTextures() {return this->numberOfTextures;}
//    void addTexture(const std::string &fileLocation);
//    //void addTexture(const std::string &fileLocation, bool trueIfJPEG_falseIfPNG, int w, int h);
//    void goToNextBackGround();
//    void goToPreviousBackGround();
//    void setBackGroundTo(int);
//    
//    int getActiveBackground() const {return this->activeBackground;}
//    
//    virtual Object*  generateInstance() override;
//    aiVector3D getStartingOrientation() override;
//    
//    
//    
////private:
//public:
//    int numberOfTextures;
//    TextureWrapper ** textures;
//    int activeBackground = 0;
//    
//    
//    /*
//     class ObjectGenerator {
//     protected:
//     GLfloat * vbo, * vertices; //Model data
//     GLuint * ebo, * elements; //Elements in the data to quote
//     int numberOfVertices, numberOfElements;
//     
//     bool hasModelData;
//     bool hasValidShader;
//     static int nextObjID; //For dealing with static class members, see: https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/cplr038.htm
//     
//     public:
//     //Public variables
//     ShaderWrapper * shader;
//     int numberOfShaders;
//     SimpleObjLoader * objFileData;
//     int activeInstances; // activeInstancesOfObject
//     
//     //Constructors
//     //Note that can't have virtual constructors. See //https://www.geeksforgeeks.org/advanced-c-virtual-constructor/
//     ObjectGenerator();
//     //ObjectGenerator(const SimpleObjLoader &);
//     //ObjectGenerator(const ShaderWrapper &);
//     //ObjectGenerator(const SimpleObjLoader &, const ShaderWrapper &);
//     //ObjectGenerator(const char * objFile);
//     //ObjectGenerator(const char * objFile, const ShaderWrapper &);
//     
//     //Destructor
//     ~ObjectGenerator();
//     
//     //Functions
//     virtual Object generateInstance() = 0;
//     virtual Object copy() = 0;
//     
//     bool checkIfReadyToGenerate() const {return (hasModelData && hasValidShader);}
//     
//     virtual void setStartingOrientation(Quaternion *xRot, Quaternion *yRot, Quaternion *zRot) = 0;
//     
//     virtual aiVector3D getStartingOrientation() = 0; //Returns a vector of whatever direction 'forward' is on the model
//     
//     void decreaseActiveInstances() {this->activeInstances -= 1;}
//     
//     
//     */
//};
//
//
//
//class Background : public Object {
//public:
//    BackGroundGenerator * bg;
//    
//    //Constructor
//    Background(int objID, ObjectGenerator *);
//    Background(int objID, BackGroundGenerator * bg);
//    //Destructor
//    virtual ~Background();
//    
//    virtual void draw() override;
//    
//    virtual void recalculateForward() override;
//    virtual void rotateXYZ() override;
//
//    //virtual void doUpkeep() override;
//    
//    
//    
//    //Uniforms
//    
////    float time;
////    float tex;
////    float zoom;
//    GLint textEnum;
//    GLfloat time;
//    GLfloat zoom;
//  
//
//    
//    };
//    
//
//#endif /* Background_h */

