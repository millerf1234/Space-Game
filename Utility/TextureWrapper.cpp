//  Quite honestly this implementation file is a complete mess, because I was/am
//  still not fully familiar with how textures work. It started as a small struct to hold
//  just a small amount of information then slowly grew to be out of control
//
//  TextureWrapper.cpp
//
//  Created by Forrest Miller on 2/16/18.
//

#include "TextureWrapper.h"

//stb_image stuff needs be be hidden in the implementation files to work
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Loads the texture and all that
//TextWrapr::TextWrapr(const std::string & imageFilePath, ImgFormat format) {
TextWrapr::TextWrapr(const std::string & imageFilePath) {
    this->tex = new GLuint;
    glGenTextures(1, this->tex);
    glBindTexture(GL_TEXTURE_2D, tex[0]); //Makes this texture active
    
    
    //Might want to add the option to customize these shader parameters later on down the line:
    //            WRAPPING
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    //           FILTERING
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //           MIPMAP GENERATION
    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    char * img = (char *)imageFilePath.c_str(); //Convert the filePath into the form stb_image read from
    w = h = components = -1; //-1 for NOT_SET_PROPERLY or some constant like that
    this->hasBeenFixed = false; //The image hasn't been corrected yet
    
    //Here is where we send the image data to stb_image to be parsed:
    //NOTE: void(*) (void*) is a function pointer to a function that takes a void pointer as parameter. See: https://stackoverflow.com/questions/12830860/what-is-voidvoid
    std::unique_ptr<unsigned char, void(*)(void*)> stbi_data( stbi_load( img, &this->w, &this->h, &this->components, STBI_rgb ), stbi_image_free);
    
    //Check to see if stbi_data is happy after processing the image data
    if (stbi_data == nullptr) { //Will be nullptr if something went wrong
        std::cout << std::endl << "\nDEBUG:: Oops! It looks like STB_image was unable to load";
        std::cout << " the image:\n" << imageFilePath << std::endl;
        return;
    }
    
    //Need to copy the data stb_image loaded into a vector
    //Find out how much data there is (I should rewrite this next line to match line in Vulkan cookbook)
    int dataSize = w * h * (0 < STBI_rgb ? STBI_rgb : this->components); //figure out how much data there is
    
    //std::cout << "\n\nDEBUG::stb_image successfully loaded image:\n    " << imageFilePath << std::endl;
    //std::cout << "DEBUG::Size of loaded image: " << dataSize << std::endl;
    
    this->image = new std::vector<unsigned char>; //create the vector in memory
    this->image->resize(dataSize); //Preallocate the vector to the right size
    std::memcpy(this->image->data(), stbi_data.get(), dataSize); //Copy the data over
    
//    if (this->w == 915 && this->h == 609) {
//        fixTexture915x609(this);
//    }
//    else { //Try to fix texture using the hint from the front of the openGL book
//
//    }
}

TextWrapr::~TextWrapr() {
    //Delete all heap memory claimed by this object
    if (this->image != nullptr) {
        delete this->image;
        this->image = nullptr;
    }
    if (this->tex != nullptr) {
        delete this->tex;
        this->tex = nullptr;
    }
}

void TextWrapr::activate() {
    if (this->tex == nullptr) {return;}
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    //glGenerateMipmap; //Try uncommenting this once everything is working
    
    if(components == 3) {
        
        //Test:
        //glPixelStorei(GL_PACK_ALIGNMENT, 1);
        
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2745/3 - 3 , h, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data.data());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h-1, 0, GL_RGB, GL_UNSIGNED_BYTE, this->image->data());
        //NOTE!!! Doing w-1 shouldn't cause the RGB values to shift...
        //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, w, h-1, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data.data());
        
        //glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
    }
    else if(components == 4) {
        //Test:
        //glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->image->data());
        
         //glPixelStorei(GL_PACK_ALIGNMENT, 2);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
    }
    else {
        std::cout << "\nDEBUG::What the heck texture are you trying to load? Unable to load this texture...\n";
    }
}

void TextWrapr::deactivate() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

//I wrote this function to fix a texture that is 915x609 pixels, so it gets most
//of the image lined up. It still isn't perfect.
void TextWrapr::fixTexture915x609(TextureWrapper * tData) { 
    if (tData == nullptr) { //If the data is a nullptr, don't try to do anything with it
        std::cout << "\nDEBUG: OOPS! fixTexture915x609 was given a nullptr to fix! This is most likely\ndue to an error in the TextureWrapper class!" << std::endl;
        return; //Leave function
    }
    else if (tData->image == nullptr) {
        std::cout << "\nDEBUG: OOPS! fixTexture915x609 was given a valid TextureWrapper, but\nthe image inside the TextureWrapper is NULL!!!" << std::endl;
        return;
    }
    else {
        // Here is the code that fixes a .jpg that is 915x609 pixels
        // (I came up with this after a few hours of experimentation)
        int w = tData->w;
        int h = tData->h;
        //lineFixerCounter is used to realign shifted edge-case pixels
        int lineFixerCounter = 3; //Start counter at 3 (for Red Green Blue for each pixel)
        if (w == 915 && h == 609) { //Check width and height to make sure before running
            std::vector<unsigned char>::iterator it; //it will iterate through the actual image data.
            for (int i = 0 ; i < w*h * 3 ; ++i) { //For each data point in the image
                //if the first pixel data slot from i is the last pixel from the end of the row
                if ((i+1) == (w-1)*3) {
                    it = tData->image->begin() + i+7;
                    //tData->image->insert(it, '\xfe');
                    tData->image->insert(it, *(it+3));
                    it = tData->image->begin() + i+7;
                    //tData->image->insert(it, '\0');
                    tData->image->insert(it, *(it+3));
                    it = tData->image->begin() + i+7;
                    //tData->image->insert(it, '\0');  //??
                    tData->image->insert(it, *(it+3));
                }
                if ((i+1) == (w-1)*6) {
                    it = tData->image->begin() + i+7;
                    //tData->image->insert(it, '\0');
                    tData->image->insert(it, *(it+3));
                    it = tData->image->begin() + i+7;
                    //tData->image->insert(it, '\0');
                    tData->image->insert(it, *(it+3));
                    it = tData->image->begin() + i+7;
                    //tData->image->insert(it, '\xfe');  //??
                    tData->image->insert(it, *(it+3));
                }
                //Aha! I have discovered a pattern that almost almost works
                //At this point there is a noticeable line in the image due to
                //pixels having been shifted, so need a special counter to try to
                //remove the line
                if ((i+1) == (914-1) * 3 * lineFixerCounter) {
                    if (lineFixerCounter < 10) {
                        lineFixerCounter++;
                        it = tData->image->begin() + i+7;
                        //tData->image->insert(it, '\xfe');
                        tData->image->insert(it, *(it+3));
                        it = tData->image->begin() + i+7;
                        //tData->image->insert(it, '\xfe');
                        tData->image->insert(it, *(it+3));
                        it = tData->image->begin() + i+7;
                        //tData->image->insert(it, '\xfe');  //??
                        tData->image->insert(it, *(it+3));
                    }
                    else { //This is the finishing touch!
                        lineFixerCounter++;
                        it = tData->image->begin() + i+7;
                        //tData->image->insert(it, '\xfe');
                        tData->image->insert(it, *(it+3));
                        it = tData->image->begin() + i+7;
                        //tData->image->insert(it, '\xfe');
                        tData->image->insert(it, *(it+3));
                        it = tData->image->begin() + i+7;
                        //tData->image->insert(it, '\xfe');
                        tData->image->insert(it, *(it+3));
                    }
                    //There, after all this runs, you can hardly tell the tiny
                    //remaining differences between the actual file and what stb
                    //loaded (Just don't look closely!)
                }
            }
            tData->hasBeenFixed = true;
        }
    }
}


//Don't use this constructor, unless it gets changed so that the TextWrapr it constructs assumes ownership of the imageData pointer. Otherwise this objects destructor could interfer with any of destructors of imageData.
//TextWrapr::TextWrapr(std::vector<unsigned char> * imageData, int w, int h, int comp, bool hasBeenFixed){
//    this->image = imageData;
//    if (this->image == nullptr) {
//        std::cout << "\nDEBUG: A TextWrapr object was constructed with a nullptr as image data!\n";
//        this->hasBeenFixed = false;
//        this->w = this->h = this->componenets = -1;
//    }
//    else {
//        this->hasBeenFixed = hasBeenFixed;
//        this->w = w;
//        this->h = h;
//    }
//}
