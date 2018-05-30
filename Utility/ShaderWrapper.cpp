//
//  ShaderWrapper.cpp
//  OpenGLSetupTestWithSTB
//
//  Created by Forrest Miller on 1/29/18.
//

#include "ShaderWrapper.h"


/*  I started to integrate the storage of uniform locations within the ShaderWrapper class
    on day before work, but unfortuantly I got only about half way through and then
    wasn't able to work on this project for a few days. So there is a class in place
    here that was intended to be local to this object which would handle 'remembering'
    a uniform location once one was specified. This would happen by the user calling
    the function set() with three parameters:
    1) a string for the name of the uniform in the shader,
    2) an enumeration so that the type of the object knew which OpenGL function to call,
    3) the program ID.
 
    There was also a hash table involved so that if a bunch of uniforms were stored on an object,
    lookup time would improve, but this really wasn't necessary because there is a small
    limit on the number of uniforms that can be passed this way without a buffer.
 
////Some utility stuff for handling uniform locations (I don't support uniform arrays yet)
////I have an enum for each type of supported Uniform Location
//enum UniformType{byteUniform, intUniform, floatUniform, unspecified};
//
////This is a tiny wrapper class that is used to track uniform locations internally
//typedef struct ULInfo { //Struct of Uniform Location Information
//private:
//    int hashkey; //For matching
//    GLuint location;
//    std::string ulName; //uniform location name
//public:
//    UniformType uniformType;
//    //Constructors
//    ULInfo() {
//        this->hashkey = NOT_SET;
//        uniformType = unspecified;
//        ulName = "";
//    }
//    ULInfo(std::string ulName, UniformType ulType, GLuint programID) : ULInfo() { //Call the default constructor
//        this->set(ulName, ulType, programID); //set self using function
//    }
//    //Interface functions
//    void set(std::string ulName, UniformType type, GLuint programID) {
//        //Check to make sure this wasn't already set for some reason
//        if (this->checkIfLocationValid()) { //i.e. check to see if a valid location has already been set
//            printf("\nDEBUG::WARNING! Attempting to overwrite uniform location for ");
//            if (uniformType == byteUniform) {
//                printf("Byte ");
//            }
//            else if (uniformType == intUniform) {
//                printf("Integer ");
//            }
//            else if (uniformType == floatUniform) {
//                printf("Float ");
//            }
//            else {
//                printf("!!!Unspecified!!! ");
//            }
//            printf("Uniform %s\n", ulName.c_str());
//            return;
//        }
//
//        //Else, try to set the uniform location
//        location = glGetUniformLocation(programID, ulName.c_str());
//        if (!checkIfLocationValid()) {
//            std::cout << "\nError retrieving uniform location for " << ulName << std::endl;
//            return;
//        }
//        this->hashkey = computeHashkey(ulName);
//        this->uniformType = type;
//        this->ulName = ulName;
//    }
//    int computeHashkey(std::string name) const {
//        if (name.length() == 0ul) {
//            std::cout << "\nDEBUG:WARNING! computeHashKey called on an empty string!\n";
//            return NOT_SET;
//        }
//        //convert name to be all lowercase
//        std::transform(name.begin(), name.end(), name.begin(), tolower); //Third parameter is target to write to
//
//        int firstLetterValue = (int) name.at(0) - (int)'a';
//        //Scale the first Letter Value to fit in the hash table
//        firstLetterValue *= LOCATIONS_PER_LETTER;
//
//        //Check to see if name contains additional letters
//        if (name.length() == 1ul) {
//            return firstLetterValue;
//        }
//
//        //Calculate the rest of the hashKey from the second letter
//        int hashKey = firstLetterValue + (((name.at(1) - 'a') * (26 / LOCATIONS_PER_LETTER)));
//        std::cout << "\nDEBUG::Computed hash key " << hashKey << " for uniform name " << name << "\n";
//        if (hashKey < 0) {
//            hashKey = UL_HASHTABLE_SIZE + hashKey;
//            std::cout << "\nDEBUG:Negative Hash key wrapped around to give key value: " << hashKey;
//        }
//        return hashKey;
//    }
//    int getHashKey() const {return this->hashkey;}
//    std::string getName() const {return this->ulName;}
//    bool checkIfLocationValid() const {return (this->location != NOT_SET);}
//    UniformType getType() const {return this->uniformType;}
//
//    //Equals operator
//    bool operator==(const ULInfo & other) {
//        if (this->location == other.location) {
//            if (this->getHashKey() == other.getHashKey()) {
//                return true;
//            }
//        } //else:
//        return false;
//    }
//    //Not Equal operator
//    bool operator!=(const ULInfo & other) {
//        if (this->location == other.location && this->getName() == other.getName()) {
//            return false;
//        }
//        return true;
//    }
//
//} ULInfo;
//
//
////Another struct for wrapping up each ULInfo struct
//
//typedef struct ULHashbucket {
//    int hashKey;
//    ULInfo data;
//    ULHashbucket * next; //For chaining the hash table if necessary
//
//    //Constructor:
//    ULHashbucket() { //Give parameters?
//        //this
//    }
//
//} ULHashbucket;
 */

//Constructors:
ShaderWrapper::ShaderWrapper() {
    //debug (DELETE THESE NEXT FEW LINES AFTER DONE DEBUG):
    //ULInfo tempForDebug;
    //tempForDebug.computeHashkey("a-HelloWorld");
    
    
    this->isReady = false;
    this->hasVert = this->hasGeom = this->hasFrag = this->hasTessl = false;
    this->vertSuccess = this->geomSuccess = this->fragSuccess = this->tesslSuccess = false;
    this->vertLayoutSet = false;
    
    //Set all the names to just empty null-terminated c-strings
    this->vertexAttribName[0] = '\0';
    this->colorAttribName[0] = '\0';
    this->textureAttribName[0] = '\0';
    this->normalAttribName[0] = '\0';
    
    this->vertexShader = nullptr;
    this->geometryShader = nullptr;
    this->fragmentShader = nullptr;
    this->tesselationShader = nullptr;
    this->programID = nullptr;
    
    this->VAO = nullptr;
    
    this->posAttrib = nullptr;
    this->colAttrib = nullptr;
    this->texAttrib = nullptr;
    this->normAttrib = nullptr;
    
    //Set up the variables used in tracking the uniform locations
    this->numberOfTrackedUniformLocations = 0;
    
    vaoWasSetExternally = false;
}

ShaderWrapper::~ShaderWrapper() {
    
    if (PRINT_DESTRUCTOR_CALLS) {
        std::cout << "\nDEBUG::ShaderWrapper Destructor Called!\n";
    }
    
    //Delete dynamic memory claimed by this object
    
    if (this->vertexShader != nullptr) {
        // glDeleteShader(*vertexShader); //I decided to delete the compiled shaders
        //                                  right after I finish linking, just to maintain
        //                                  consistancy.
        delete this->vertexShader;
        this->vertexShader = nullptr;
    }
    
    if (this->geometryShader != nullptr) {
        //glDeleteShader(*geometryShader);
        delete this->geometryShader;
        this->geometryShader = nullptr;
    }
    
    if (this->fragmentShader != nullptr) {
       // glDeleteShader(*fragmentShader);
        delete this->fragmentShader;
        this->fragmentShader = nullptr;
    }
    
    if (this->tesselationShader != nullptr) {
        //glDeleteShader(*tesselationShader);
        delete this->tesselationShader;
        this->tesselationShader = nullptr;
    }

    if (this->programID != nullptr) {
        delete this->programID;
        this->programID = nullptr;
    }
    
    if (this->VAO != nullptr) { //Delete the int representing this shader's VAO
        delete this->VAO ;
        this->VAO = nullptr;
    }
    
    if (this->posAttrib != nullptr) {
        delete this->posAttrib;
        this->posAttrib = nullptr;
    }
    
    if (this->colAttrib != nullptr) {
        delete this->colAttrib;
        this->colAttrib = nullptr;
    }
    
    if (this->texAttrib != nullptr) {
        delete this->texAttrib;
        this->texAttrib = nullptr;
    }
    
    if (this->normAttrib != nullptr) {
        delete this->normAttrib;
        this->normAttrib = nullptr;
    }
    
    
    //Template for deleting dynamic memory allocations I might add in the future
//    if (this-> != nullptr) {
//        delete this-> ;
//        this-> = nullptr;
//    }
}

bool ShaderWrapper::attachVert(char * vertSource) {
    if (hasVert) { //Make sure a vert shader is not already attached
        std::cout << "\nError attaching " << vertSource;
        std::cout << "\nThis ShaderWrapper already has a vertSource attached!";
        return false;
    }
    
    //Load the source file text as a C++ string using function loadSource
    this->vert = loadSourceFile((char *)vertSource);
    //Need to convert C++ string to a GLchar C string to be compiled onto the GPU
    const GLchar* vertexShaderSource = (const GLchar *)vert.c_str();
    //Tell OpenGL to compile the shader onto the GPU. VertexShader gets assigned a number to identify and track the shader.
    vertexShader = new GLuint;
    *vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(*vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(*vertexShader);
    
    //Check for compilation success
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(*vertexShader, GL_COMPILE_STATUS, &success);
    //If not successful, let user know and clean self up
    if (!success) {
        glGetShaderInfoLog(*vertexShader, 512, NULL, infoLog);
        
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertSource
        << std::endl << infoLog << std::endl;
        
        delete vertexShader;
        vertexShader = nullptr;
        return false;
    }
    this->hasVert = true;
    this->vertSuccess = true;
    
    return true;
}

bool ShaderWrapper::attachGeom(char * geomSource) {
    if (hasGeom) { //Make sure a geom shader is not already attached
        std::cout << "\nError attaching " << geomSource;
        std::cout << "\nThis ShaderWrapper already has a geomSource attached!";
        return false;
    }
    return false;
}

bool ShaderWrapper::attachFrag(char * fragSource){
    if (hasFrag) { //Make sure a Frag shader is not already attached
        std::cout << "\nError attaching " << fragSource;
        std::cout << "\nThis ShaderWrapper already has a fragSource attached!";
        return false;
    }
    //Load the source file text as a c++ string
    this->frag = loadSourceFile((char *)fragSource);
    //Need to convert C++ string to a GLchar C string to be compiled onto the GPU
    const GLchar* fragmentShaderSource = (const GLchar *)frag.c_str();
    //Tell OpenGL to compile the shader onto the GPU. FragmentShader gets assigned a number to identify and track the shader.
    fragmentShader = new GLuint;
    *fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(*fragmentShader);
    
    //Check for compilation success
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(*fragmentShader, GL_COMPILE_STATUS, &success);
    //If not successful, let user know and clean self up
    if (!success) {
        glGetShaderInfoLog(*fragmentShader, 512, NULL, infoLog);
        
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragSource
        << std::endl << infoLog << std::endl;
        
        delete fragmentShader;
        fragmentShader = nullptr;
        return false;
    }
    this->hasFrag = true;
    this->fragSuccess = true;
    
    return true;
}

bool ShaderWrapper::attachTessl(char * tesslSource){
    if (hasTessl) { //Make sure a Frag shader is not already attached
        std::cout << "\nError attaching " << tesslSource;
        std::cout << "\nThis ShaderWrapper already has a tesslSource attached!";
        return false;
    }
    return false;
}

//Note that a shader program must always have at least a vert and a frag. The
//Tesselation and Geometry shaders are optional
bool ShaderWrapper::link() {
    //Make sure there are enough shaders in place to create the shaderProgram set
    if (!readyToLink() || this->programID != nullptr) {
        if (this->programID != nullptr) {
            std::cout << "\nERROR LINKING! This shader program (programID = " << programID << ") has already been linked!"
            << std::endl;
        }
        return false;
    }
    
    //Create the shader program
    this->programID = new GLuint;
    *programID = glCreateProgram();
    
    //Attach the vertex shader to this object
    glAttachShader(*programID, *vertexShader);
    
    //Attach Geometry shader if there is one
    if (this->hasGeom) {
        //DO the work to attach the geom shader
        std::cout << "OOPS! Attaching geomtry shaders hasn't been implemented yet in the 'link()' step!\n";
    }
    
    //Attach Tesselation shader if there is one
    if (this->hasTessl) {
        //DO the work to attach the tesselation shader
        std::cout << "OOPS! Attaching tessl shaders hasn't been implemented yet in the 'link()' step!\n";
    }
    
    //Attach Frag shader
    glAttachShader(*programID, *fragmentShader);
    
    //Now that all the shaders have been attached, link the program
    glLinkProgram(*programID);
    
    //Check for errors and print out reason if there are errors:
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(*programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(*programID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }
    //So for all code beyond this point, we know that the shader program has
    //successfully been created
    
    //Detach shaders if they are linked
    glDetachShader(*programID, *vertexShader);
    if (hasTessl) {
        //Detatch the tesselation shaders
        //glDetachShader control
        //glDetachShader Evaluation
    }
    if (hasGeom) {
        //Detatch the Geometry Shaders
        glDetachShader(*programID, *geometryShader);
    }
    glDetachShader(*programID, *fragmentShader);
    
    //Delete shaders once they are linked
    if (vertexShader != nullptr) { //These nullptr checks aren't necessary but might as well, just to guarentee no issues down the road
        glDeleteShader(*vertexShader);
        vertexShader = nullptr;
    }
    if (this->hasGeom) {
        if (this->geometryShader != nullptr) {
            glDeleteShader(*geometryShader);
            geometryShader = nullptr;
        }
    }
    if (this->hasTessl) {
        if (this->tesselationShader != nullptr) {
            glDeleteShader(*tesselationShader);
            tesselationShader = nullptr;
        }
    }
    if (fragmentShader != nullptr) {
        glDeleteShader(*fragmentShader);
        fragmentShader = nullptr;
    }
    
    
    isLinked = true;
    
    //if (PRINT_DEBUG)
    
    return isLinked;
}

bool ShaderWrapper::specifyVertexLayout(vertLayoutFormat vlf) {
    if (!this->isLinked) {
        std::cout << "\nError! Need to finish the link step before specifing vertex layout format!\n";
        return false;
    }
    
    this->VAO = new GLuint;
    glGenVertexArrays(1, this->VAO);
    glBindVertexArray(*(this->VAO));
    
    
    if (vlf == VERT2) {
        this->posAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        glEnableVertexAttribArray(*(this->posAttrib));
        glVertexAttribPointer(*(this->posAttrib), 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    }
    
    else if (vlf == VERT3) {
        this->posAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        glEnableVertexAttribArray(*(this->posAttrib));
        std::cout << "\nposAttrib is " << *(this->posAttrib) << std::endl;
        glVertexAttribPointer(*(this->posAttrib), 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    }
    
    else if (vlf == VERT3TEXEL2) {
        this->posAttrib = new GLint;
        this->texAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        *(this->texAttrib) = glGetAttribLocation(*programID, this->textureAttribName);
        glEnableVertexAttribArray(*(this->posAttrib));
        std::cout << "\nposAttrib is " << *(this->posAttrib) << std::endl;
         glVertexAttribPointer(*(this->posAttrib), 5, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
        glEnableVertexAttribArray(*(this->texAttrib));
        std::cout << "\ntexAttrib is " << *(this->posAttrib) << std::endl;
        glVertexAttribPointer(*(this->texAttrib), 5, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)) );
    }
    
    else {
        std::cout << "\nOops! Due to Programmer lazyness, the vertex layout format you specified has not yet been implemented." << std::endl;
    }
    
    // glEnableVertexAttribArray(posAttrib3DLine);
   
    
    this->vertLayoutSet = true;
    this->isReady = true;
    return true;
}

void ShaderWrapper::setVAOExternally(GLuint value) {
    if (this->VAO != nullptr) {
        delete this->VAO;
        this->VAO = nullptr;
    }
    this->VAO = new GLuint;
    *(this->VAO) = value;
    vaoWasSetExternally = true;
}


//I am putting in this function quickly to do exactly what I want it to. Right now it only works on a VERT3 input layout
bool ShaderWrapper::specifyVertexLayout(vertLayoutFormat vlf, GLuint& vbo) {
    if (!this->isLinked) {
        std::cout << "\nError! Need to finish the link step before specifing vertex layout format!\n";
        return false;
    }
    if (!(this->vaoWasSetExternally)) { //Generate a VAO for this shader
        this->VAO = new GLuint;
        glGenVertexArrays(1, this->VAO);
        glBindVertexArray(*(this->VAO));
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    if (vlf != VERT3) {
        std::cout << "\nOOPS! This vertex format is not yet supported by this shaderWrapper object.\nTry layout with an ebo included!\n";
        return false;
    }
    
    //if vlf is VERT3:
    this->posAttrib = new GLint;
    *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
    glEnableVertexAttribArray(*(this->posAttrib));
    if (PRINT_DEBUG_MESSAGES) {
        std::cout << "\nposAttrib is " << this->vertexAttribName << " and was assigned to offset " << *(this->posAttrib) << " in the data."  << std::endl;
    }
    glVertexAttribPointer(*(this->posAttrib), 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    
    this->vertLayoutSet = true;
    this->isReady = true;
    
    return true;
}

bool ShaderWrapper::specifyVertexLayout(vertLayoutFormat vlf, GLuint& vertData, GLuint& elemData) {
    if (!this->isLinked) {
        std::cout << "\nError! Need to finish the link step before specifing vertex layout format!\n";
        return false;
    }
    
    if (!(this->vaoWasSetExternally)) { //Generate a VAO for this shader
        this->VAO = new GLuint;
        glGenVertexArrays(1, this->VAO);
        glBindVertexArray(*(this->VAO));
    }
   
    
    glBindBuffer(GL_ARRAY_BUFFER, vertData);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemData);
    
    if (vlf == VERT2) {
        this->posAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        glEnableVertexAttribArray(*(this->posAttrib));
        glVertexAttribPointer(*(this->posAttrib), 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    }
    
    else if (vlf == VERT3) {
        this->posAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        glEnableVertexAttribArray(*(this->posAttrib));
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nposAttrib is " << this->vertexAttribName << " and was assigned to offset " << *(this->posAttrib) << " in the data."  << std::endl;
        }
        glVertexAttribPointer(*(this->posAttrib), 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    }
    
    else if (vlf == VERT3TEXEL2) {
        this->posAttrib = new GLint;
        this->texAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        *(this->texAttrib) = glGetAttribLocation(*programID, this->textureAttribName);
        glEnableVertexAttribArray(*(this->posAttrib));
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::posAttrib is " << *(this->posAttrib) << std::endl;
        }
//        glVertexAttribPointer(*(this->posAttrib), 5, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
        glVertexAttribPointer(*(this->posAttrib), 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
        glEnableVertexAttribArray(*(this->texAttrib));
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::texAttrib is " << *(this->texAttrib) << std::endl;
        }
        //glVertexAttribPointer(*(this->texAttrib), 5, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)) );
        glVertexAttribPointer(*(this->texAttrib), 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)) );
    }
    else if (vlf == VERT3NORML3) {
        this->posAttrib = new GLint;
        this->normAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        *(this->normAttrib) = glGetAttribLocation(*programID, this->normalAttribName);
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\n\nDEBUG STATEMENT:NORMAL ATTRIB NAME IS: " << this->normalAttribName;
        }
        
        //Let GPU know how the data in vertices is laid-out
        //Position data:
        glEnableVertexAttribArray(*(this->posAttrib));
        glVertexAttribPointer(*(this->posAttrib), 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG STATEMENT: Vertex Attrib was assigned: " << *(this->posAttrib);
        }
        //Normal vector data:
        glEnableVertexAttribArray(*(this->normAttrib));
        glVertexAttribPointer(*(this->normAttrib), 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(GLfloat)));
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG STATEMENT: Normal Attrib was assigned: " << *(this->normAttrib);
        }
    }
    else if (vlf == VERT2COLOR3TEXEL2) { //2 vertices, RGB color and 2 Texel coords
        //glBindVertexArray(*(this->VAO));
        //std::cout << "\nDEBUG STATEMENT: USING VERTEX ATTRIB FORMAT VERT2COLOR3TEXEL2";
        this->posAttrib = new GLint;
        this->colAttrib = new GLint;
        this->texAttrib = new GLint;
        *(this->posAttrib) = glGetAttribLocation(*programID, this->vertexAttribName);
        *(this->colAttrib) = glGetAttribLocation(*programID, this->colorAttribName);
        *(this->texAttrib) = glGetAttribLocation(*programID, this->textureAttribName);
        
        //Let GPU know how the vertex coordinates are layed out (first 2 values)
        glEnableVertexAttribArray(*(this->posAttrib));
        glVertexAttribPointer(*(this->posAttrib), 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "DEBUG STATEMENT: Vertex Attrib was assigned: " << *(this->posAttrib);
        }
        
        //Let GPU know how the color rgb values are layed out (next 3 values)
        glEnableVertexAttribArray(*(this->colAttrib));
        glVertexAttribPointer(*(this->colAttrib), 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2 * sizeof(GLfloat)));
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG STATEMENT: Color Attrib was assigned: " << *(this->colAttrib);
        }
        
        //Let GPU know how the texture sampling (u/v) coordinates are laid out (last 2 values)
        //The GPU interpolates between these values,
        glEnableVertexAttribArray(*(this->texAttrib));
        glVertexAttribPointer(*(this->texAttrib), 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)) );
        if (PRINT_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG STATEMENT: Tex Attrib was assigned: " << *(this->texAttrib);
            std::cout << std::endl << std::endl;
        }
        
        
    }
    
    else {
        std::cout << "\nOops! Due to Programmer lazyness, the vertex layout format you specified has not yet been implemented." << std::endl;
        return false;
    }
    
    // glEnableVertexAttribArray(posAttrib3DLine);
    
    this->vertLayoutSet = true;
    this->isReady = true;
    return true;
}

void ShaderWrapper::turnOffVertexLayout(vertLayoutFormat vlf) {
    std::cout << "Warning! Don't call this function because it should never be ";
    std::cout << "needed if you\nare using the VAO correctly." << std::endl;
    std::cout << "(UNLESS YOU IMPLEMENT A PROPER CLEAN-UP ON EXITING THE PROGRAM... YOU SHOULD LOOK INTO IF THIS IS NECESSARY AT SOME POINT!)" << std::endl;
    if (vlf == VERT3) {
        glDisableVertexAttribArray(*(this->posAttrib));
    }
    else if (vlf == VERT3TEXEL2) {
        glDisableVertexAttribArray(*(this->posAttrib));
        glDisableVertexAttribArray(*(this->texAttrib));
    }
    else {
        std::cout << "\nExtra Warning! You are attempting to turn off a vertex layout\n";
        std::cout << "that due to programmer laziness has not yet been implemented!" << std::endl;
        std::cout << "Please implement this format before you try to turn it off!" << std::endl;
    }
}




/*  (THIS IS FROM AN EXAMPLE FROM THE INTERNET OF HOW THINGS ARE SUPPOSED TO GO DOWN)
 Initialisation:
 Code :
 glGenVertexArrays(1, &asteroid_vao);  //First off, create a VAO
 glBindVertexArray(asteroid_vao); //Bind dat VAO dawg (NOTE: Bind just means to tell OpenGL the location of something)
 glBindBuffer(GL_ARRAY_BUFFER, asteroid_buffer_object); //Bind yo Vertex Buffer next
 glEnableVertexAttribArray(0); //Here there is just plain vertex position data, so can use 0
 glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); //Tell OpenGL how the vertex data is laid out
 
 //Now going to use a different VAO for a different object to be drawn
 glGenVertexArrays(1, &ship_vao);
 glBindVertexArray(ship_vao);
 glBindBuffer(GL_ARRAY_BUFFER, ship_buffer_object);
 glEnableVertexAttribArray(0);
 glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
 
 glBindVertexArray(0);
 
 Rendering:
 Code :
 glUseProgram(theProgram);
 glBindVertexArray(asteroid_vao);
 glDrawArrays(GL_LINE_LOOP, 0, num_asteroid_vertices);
 glBindVertexArray(ship_vao);
 glDrawArrays(GL_LINE_LOOP, 0, num_ship_vertices);
 glBindVertexArray(0);
 glUseProgram(0);
 */
