//  This class is a wrapper for an OpenAL audio source and OpenAl audio buffer
//
//  AudioSource.h
//  Star Suzerian
//
//  Created by Forrest Miller on 6/20/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#ifndef AudioSource_h
#define AudioSource_h

#include <iostream>
#include <vector>
#include <array>

///For playing sounds:
#include <thread>
#include <future>
#include <chrono>


#include "OpenAL.h"
#include "AudioData.h"

//To use AudioSource, must first construct an audio source and then call
// setAudio() with the audio file path

class AudioSource {
public:
    AudioSource() = delete;
    AudioSource(const AudioSource & other) = delete;
    AudioSource & operator=(const AudioSource & other) = delete;
    
    ///Constructors
    AudioSource(ALfloat x, ALfloat y, ALfloat z, bool looping = false);
    AudioSource(ALfloat x, ALfloat y, ALfloat z, ALfloat pitch, ALfloat gain, bool looping = false);
    AudioSource(ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping = false);
    ///Pool Constructor (for if multiple sources/buffers were generated at once)
    AudioSource(ALuint buffer, ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping = false);
    AudioSource(ALuint source, ALuint buffer, ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping = false);
    ~AudioSource();
    
    bool setAudio(const char * filepath);
    
    void updatePosition(ALfloat x, ALfloat y, ALfloat z);
    void getPosition(ALfloat * x, ALfloat * y, ALfloat * z);
    
    ALuint getSource() const {return this->source;}
    ALuint getBuffer() const {return this->buffer;}
    
    bool isLooping() const { return this->looping; }
    
    bool getIfErrorWasEncountered() const { return this->encounteredError; }
    
    void playSource();
    //void playSource(bool * kill); //It doesn't work like this...
    
    ///Todo
    //bool bindAudioFromExistingBuffer(ALuint bufferID);
    
private:
    ALuint source;
    ALuint buffer;
    
    bool looping;
    
    AudioData * audioData;
    bool hasAudioData;
    
    bool encounteredError;
    
    void initialize(ALuint * source, ALuint * buffer, ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping);
    
    bool checkForError(); ///Returns false if no error, true if there is an error
    
    static inline ALenum convert_to_AL_format(short channels, short samples);
    
    ///This next function is to be run as a seperate thread and basically handles
    ///ensuring the audio plays
    static void playSourceThread(ALuint source) {
        alSourcePlay(source);
        //checkForError();
        
        
        ///ALL THIS BELOW HERE MAKES THIS FUNCTION BLOCK FROM RETURNING! DON'T DO THIS!
        //ALint source_state = static_cast<ALint>(0); ///val doesn't actually do anything
        //alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        ////checkForError();
        //
        //while (source_state == AL_PLAYING) {
            //alGetSourcei(source, AL_SOURCE_STATE, &source_state);
            //usleep(10000);
            ////checkForError();
        //}
    }
};


///UPDATE: Instead of using this function, check out p.246 in Effective Modern C++
/*
/////Function I got directly from: https://stackoverflow.com/questions/42041197/c-threads-for-background-loading/42042110#42042110
/////This Function checks whether an async task which returns a std::future has been completed
//template<typename R>
//bool isReady(const std::future<R> &f)
//{
//    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
//}
 */



#endif // AudioSource_h
