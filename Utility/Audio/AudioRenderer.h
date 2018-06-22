//
//  AudioRenderer.hpp
//  Star Suzerian
//
//  This code was highly inspired by the tutorial code at:
//                                   https://ffainelli.github.io/openal-example/
//
//  Created by Forrest Miller on 6/20/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#ifndef AudioRenderer_h
#define AudioRenderer_h

#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <memory>
#include <sstream>

#include "OpenAL.h" //This include includes all the other includes
//#include "al.h"   // <AL/al.h>   if not on mac?
//#include "alc.h"  // <AL/alc.h>  if not on mac?
//#include <OpenAL/al.hs> //?? Not needed?

#include "AudioSource.h"


class AudioRenderer {
public:
    AudioRenderer(bool printMessages = true);
    ~AudioRenderer();
    
    bool getIsValid() const { return this->mIsValid; }
    
    void getListenerPosition(float & x, float & y, float & z);
    void getListenerVelocity(float & x, float & y, float & z);
    void getListenerOrientation(ALfloat * vectorOfSixFloats);
    float getListenerGain();
    
    void setListenerGain(float gain);
    
    //AudioSource* spawnAudioSource(float x, float y, float z, ALCchar* filepath);//Use this to spawn audio noises
    
    ///Idea for interface:
    /*
     create audio buffer -- return a handle to reference created buffer
     create audio source -- a source (position, parameters)
     
     
    
    */
private:
    ALCdevice * mDevice;
    ALCcontext * mContext;
    bool mIsValid;
    
    std::list<AudioSource *> sources;
    
    //Helper functions
    bool initialize(bool printMessages);
    
    bool checkForError(); ///Returns false if no error, true if there is an error
    
    void enumerateAudioDevices(std::vector<const ALCchar*>& devices);
    bool createAudioDevice(const ALCchar* deviceName);
    bool createAudioContext();
    
    bool configureListenerPosition();
    bool configureListenerGain(float gain);
    
    void parseDevicesIntoVector(std::vector<const ALchar*>&);
    void printNamesOfEnumeratedDevices(size_t, const std::vector<const ALCchar*> &) const;
};



#endif // AudioRenderer_h
