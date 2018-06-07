//  NOTICE: The code contained in this class is incredibly sloppy and should be viewed by no-one.
//  I wrote this code very quickly while trying to get another part of the project working.
//  Ideally at some point this code can be rewritten, but since it is currently functional
//  rewriting this is low priority.
//
//  DESCRIPTION:
//  TextureWrapper.h
//  This file is the header for a struct that handles facilitaing the use of
//  textures. It was designed to be constructed with a string that is a filepath to
//  either a .png or .jpeg image to be loaded as a texture.
//
//  Currently it struggles to load anything besides a JPEG that is exactly
//  915x609 pixels. Maybe it can also do images that are a power of 2 in size.
//
//
//  Created by Forrest Miller on 2/16/18.
//

#ifndef TextureWrapper_h
#define TextureWrapper_h

#include <iostream>
#include <string>
#include <vector>
#include "glad.h"
//#include "stb_image.h"
#include "GameParameters.h"

enum ImgFormat{JPEG, PNG}; //Might add more image formats in the future

//Here is a small struct for wrapping up textures inside (it grew large unfortunatly)
//-------------------------------------------------------------------------
typedef struct TextWrapr { //This quickly became something that shouldn't be a struct.
    std::vector<unsigned char> *image;
    bool hasBeenFixed;
    int w, h, components; //width, height, componenents
    //ImgFormat format;
    GLuint * tex;
    //Constructors
    TextWrapr() = delete; //No Default Constructor
    TextWrapr(const TextWrapr &) = delete; //No Copy Constructor
    //TextWrapr(std::vector<unsigned char> *, int w, int h, int comp, bool hasBeenFixed = false); //Can construct from existing data
    TextWrapr(const std::string & imgLocation/*, ImgFormat format*/);
    //Destructor
    ~TextWrapr();
    
    bool operator=(const TextWrapr &) = delete; //No assignment operator
    
    void activate(); //i.e. use
    void use() {this->activate();}
    void deactivate();
    void stop_using() {this->deactivate();} //better function name than deactivate
    //Some Utility Functions
private:
    void fixTexture915x609(TextWrapr *);
    void fixTexture915x609(TextWrapr * tData, int offset){this->fixTexture915x609(&tData[offset]);}
    
} TextureWrapper;
//-------------------------------------------------------------------------




#endif /* TextureWrapper_h */
