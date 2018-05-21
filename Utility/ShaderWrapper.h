//
//  ShaderWrapper.hpp
//
//  Created by Forrest Miller on 1/29/18.
//

#ifndef ShaderWrapper_h
#define ShaderWrapper_h

#include <iostream>
#include <fstream>
#include "glad.h"

//If using ShaderWrapper within a project that has a ProjectSetup.h
//#include "ProjectSetup.h"
//If using ShaderWrapper within a project that has a GameParameters.h
#include "GameParameters.h"

#define MAX_TRACKED_UNIFORM_LOCATIONS 256  //


//Note to self:
//TO LEARN HOW TO ACTUALY DO NORMALS/TANGENTS/BITANGENTS, see: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
//

constexpr unsigned long LOCATIONS_PER_LETTER = 13ul;
constexpr unsigned long UL_HASHTABLE_SIZE = 26ul * LOCATIONS_PER_LETTER; //26 letters in alphabet * 6 locations per letter
static constexpr int NOT_SET = -1;



class ShaderWrapper {
private:
    //Fields
    bool isLinked;
    bool isReady;
    bool hasVert, hasGeom, hasFrag, hasTessl;
    bool vertSuccess, geomSuccess, fragSuccess, tesslSuccess;
    
    bool vertLayoutSet;
    std::string vert;
    std::string geom;
    std::string frag;
    std::string tessl;
    
    int numberOfTrackedUniformLocations;
    
    //Add static to track number of programs in use? (Actually programID will already be this counter)
    std::string loadSourceFile(char* filename) {  //I didn't write this function, it's from Glitter.
        std::ifstream infile{ filename };
        return{ std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() };
    }
    
    void Detatch(); //Handles cleaning up shader
    
    

    
public:
    //Enum for vertexLayoutFormats
    enum vertLayoutFormat{VERT2, VERT3, VERT2TEXEL2, VERT2COLOR3TEXEL2, VERT3TEXEL2, VERT3TEXEL3, VERT3NORML3, VERT2NORMAL2TEXEL2, VERT3NORMAL3TEXEL3, VERT3TEXEL3NORMAL3, VERT3NORMAL3TEXEL2};
    
    ShaderWrapper();
    ShaderWrapper(const ShaderWrapper&) = delete;
    
    ~ShaderWrapper();
    
    //No copy assignment operator
    ShaderWrapper operator=(const ShaderWrapper&) = delete;
    
    //Add a function that handles uniforms
    void setVAOExternally(GLuint);
    bool vaoWasSetExternally;
    
    GLuint * vertexShader;
    GLuint * geometryShader;
    GLuint * fragmentShader;
    GLuint * tesselationShader;
    
    GLuint * programID;
    GLuint * VAO;
    
    GLint * posAttrib;
    GLint * colAttrib;
    GLint * texAttrib;
    GLint * normAttrib;
    
    
    GLchar vertexAttribName[512];
    GLchar colorAttribName[512];
    GLchar textureAttribName[512];
    GLchar normalAttribName[512];
    
    bool attachVert(char * vertSource);
    bool attachGeom(char * geomSource);
    bool attachFrag(char * fragSource);
    bool attachTessl(char * tesslSource);
    
   
    
    //These next 4 functions are for letting this object know what the 'in' variables
    //in the vert shader are called
    void setVertexAttribName(const GLchar * name) {
        int i = 0;
        while (name[i] != '\0') {
            vertexAttribName[i] = name[i];
            ++i;
        }
        vertexAttribName[i] = '\0'; //Don't forget to add character signifying end of c-string
    }
    
    void setColorAttribName(const GLchar * name) {
        int i = 0;
        while (name[i] != '\0') {
            colorAttribName[i] = name[i];
            ++i;
        }
        colorAttribName[i] = '\0'; //Add terminating character to c-string
    }
    
    void setTextureAttribName(const GLchar * name) {
        int i = 0;
        while (name[i] != '\0') {
            textureAttribName[i] = name[i];
            ++i;
        }
        textureAttribName[i] = '\0'; //Add the termination to the cstring
    }
    
    void setNormalAttribName(const GLchar * name) {
        int i = 0;
        while (name[i] != '\0') {
            normalAttribName[i] = name[i];
            ++i;
        }
        normalAttribName[i] = '\0';
    }
    
    bool readyToLink() {
        if (this->hasVert && this->hasFrag) {
            return true;
        } //Else:
        return false;
    }
    
    bool link();
    
    bool checkIfReady() const {return this->isReady;}
    
    bool specifyVertexLayout(vertLayoutFormat);
    bool specifyVertexLayout(vertLayoutFormat, GLuint& vertData, GLuint& elemData);
    void turnOffVertexLayout(vertLayoutFormat);
    
    
    //--------------------------------------------------------------------------
    //       UNIFORM UPDATING FUNCTIONS
    //--------------------------------------------------------------------------

    //This code below is from my attempt at using a container of unique pointers.
    //The compiler doesn't seem to like me creating an empty container of unique_ptr's though...
    //Hmm the use of unique_ptr's here might not make sense. I'm just going to do it old-school
//    typedef struct ULDataVector
//    {
//    public:
//        ULDataVector(std::vector<std::unique_ptr<ULInfo>> ulData) : vec(std::move(ulData)) {;}
//        // Delete default copy constructor + assignment operator to allow use of unique_ptr
//        ULDataVector(const ULDataVector &) = delete;
//        ULDataVector& operator= (const ULDataVector &) = delete;
//    private:
//        std::vector<std::unique_ptr<ULInfo> > vec;
//    } ULDataVector;
    
    
    
    /*
     typedef struct ULInfo { //Struct of Uniform Location Information
     private:
     int hashtag; //For matching
     GLuint location;
     public:
     UniformType uniformType;
     ULInfo()
     
     } ULInfo;
     */
    
    //Use raw pointers on the heap:
    
    
    
public:
    
    
    
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    
    void use() {
        if (!this->isReady) {
            std::cout << "OOPS! This program isn't ready!";
            return;
        }
        glBindVertexArray(*(this->VAO));
        glUseProgram(*this->programID);
    }
    
    GLuint getID() {
        if (programID == nullptr) {
            std::cout << "\nError! getID() called on incomplete shader program\n";
            return ( (GLuint) 8742);
        }
        return *programID;
    }
};
#endif /* ShaderWrapper_h */
