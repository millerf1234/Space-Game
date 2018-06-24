//
//  SoundEffect.h
//  Star Suzerian
//
//  Created by Forrest Miller on 6/23/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#ifndef SoundEffect_h
#define SoundEffect_h

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <sstream>

///For playing sounds:
#include <thread>
#include <future> ///Could do either FUTURE or THREAD... depends
//#include <chrono>

#include "OpenAL.h"
#include "AudioData.h"

///Use a struct to keep Audio Buffers and Filepath and Data all together
typedef struct AudioBufferWrapper {
    const char * filepath;
    AudioData * audioData;
    ALuint bufferID;
    
    AudioBufferWrapper() {
        this->audioData = nullptr;
    }
    ~AudioBufferWrapper(){
        alDeleteBuffers(1, &(this->bufferID));
        //Check with OpenAL for error?
        delete this->audioData;
        this->audioData = nullptr;
    }
} AudioBufferWrapper;

class SoundEffect {
public:
    SoundEffect();
    SoundEffect(const char * filepath);
    ~SoundEffect();
    
    SoundEffect(const SoundEffect&) = delete;
    SoundEffect(const SoundEffect&&) = delete;
    SoundEffect& operator=(const SoundEffect&) = delete;
    SoundEffect& operator=(const SoundEffect&&) = delete;
    
    void addSound(const char * filepath);
    
    bool checkIsValid() const { return this->mValid; }
    bool checkIfHasAValidFilepath() const { return this->mHasFilepath; }
    bool checkIfHasName() const { return this->mHasName; }
    bool checkIfHasMultipleBuffers() const { return this->mHasMultipleBuffers; }
    bool error() const { return this->mEncounteredError; }
    
    size_t getNumberOfBuffers() const { return this->audioBuffers.size(); }
//    const char * getFilePath() const {
//        if (!(this->mHasFilepath)) {
//            return "\0";
//        }
//        else {
//                return mName->c_str();
//        }
//    }
    
    const char * name() const; 
    
    ALuint getFirstBuffer() const;
    
    ///unsigned long getSoundID() const { return this->mSoundID; }  ///I was confusing this function with the function to getFirstBuffer!
    
private:
    unsigned long mSoundID;
    std::string * mName;
    
    bool mValid; ///True if this SoundEffect is ready to go
    bool mHasFilepath; ///True if this sound effect has a filepath set
    bool mHasName;
    bool mHasMultipleBuffers; //True if multiple buffers are set up
    
    bool mEncounteredError;
    
    std::vector<AudioBufferWrapper> audioBuffers;
    std::vector<std::unique_ptr<AudioBufferWrapper>> audioBufferPtrs;
    
    ///Looping? Y/N  (thinking 'no')

    //Private functions
    void initialize();
    
    bool checkIfErrorInOpenAL();
};



#endif // SoundEffect_h
