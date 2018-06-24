//  This class is a wrapper for an OpenAL audio source and OpenAl audio buffer
//
//  AudioSource.cpp
//  Star Suzerian
//
//  Created by Forrest Miller on 6/20/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#include "AudioSource.h"

static constexpr ALfloat zerof = 0.0f; //"Zero Float"

AudioSource::AudioSource(ALfloat x, ALfloat y, ALfloat z, bool looping) {
    this->initialize(nullptr, nullptr, x, y, z, zerof, zerof, zerof, zerof, zerof, looping);
}
AudioSource::AudioSource(ALfloat x, ALfloat y, ALfloat z, ALfloat pitch, ALfloat gain, bool looping) {
    this->initialize(nullptr, nullptr, x, y, z, zerof, zerof, zerof, pitch, gain, looping);
}
AudioSource::AudioSource(ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping){
    this->initialize(nullptr, nullptr, positionX, positionY, positionZ, velocityX, velocityY, velocityZ, zerof, zerof, looping);
}

AudioSource::AudioSource(ALuint buffer, ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping) {
    this->initialize(nullptr, &buffer, positionX, positionY, positionZ, velocityX, velocityY, velocityZ, pitch, gain, looping);
}
AudioSource::AudioSource(ALuint source, ALuint buffer, ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping) {
    this->initialize(&source, &buffer, positionX, positionY, positionZ, velocityX, velocityY, velocityZ, pitch, gain, looping);
}

AudioSource::~AudioSource() {
    alDeleteSources(1, &(this->source));
    alDeleteBuffers(1, &(this->buffer)); //Need a way for this not to happen if buffer was provided externally
    delete this->audioData;
    this->audioData = nullptr;
}

///If initialize() gets nullptr for source or buffer, then it knows to generate these itself
void AudioSource::initialize(ALuint * source, ALuint * buffer, ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping) {
    this->audioData = nullptr;
    this->hasAudioData = false;
    this->encounteredError = false;
    this->source = 0;
    
    bool error = false;
    ///Set source
    if (nullptr == source) {
        alGenSources((ALuint)1, &(this->source));
        //alGenSources(static_cast<ALuint>(1u), &(this->source));
    }
    else {
        this->source = *source;
    }
    error = (error || checkForError());
    
    ///Set buffer
    if (nullptr == buffer) {
        alGenBuffers(static_cast<ALuint>(1), &this->buffer);
    }
    else {
        this->buffer = *buffer;
    }
    ///Set looping
    this->looping = looping;
    
    
    
    ///Provide data to OpenAL
    ///  (SHOULD CHECK PITCH AND GAIN TO MAKE SURE THEY ARE IN THE CORRECT RANGE OF VALUES)
    alSourcef(this->source, AL_PITCH, pitch);
    error = (error || checkForError());
    alSourcef(this->source, AL_GAIN, gain);
    error = (error || checkForError());
    alSource3f(this->source, AL_POSITION, positionX, positionY, positionZ);
    error = (error || checkForError());
    alSource3f(this->source, AL_VELOCITY, velocityX, velocityY, velocityZ);
    error = (error || checkForError());
    if (this->looping) {
        alSourcei(this->source, AL_LOOPING, AL_TRUE);
    }
    else {
        alSourcei(this->source, AL_LOOPING, AL_FALSE);
    }
    error = (error || checkForError());
    
    if (error) {
        std::cout << "\nAn error occured setting up this AudioSource object!\n";
        this->encounteredError = true;
    }
}

bool AudioSource::setAudio(const char * filepath) {
    if (this->hasAudioData) {
        std::cout << "\nError setting audio data for this AudioSource!\nThis AudioSource ";
        std::cout << "already has audio data!\n\n";
        return false;
    }
    
    this->audioData = new AudioData(filepath);
    
    if (!this->audioData->validData() || !(this->audioData->validDecoder())) {
        std::cout << "\nAn error occured while parsing the audio data for this AudioSource!\n";
        this->encounteredError = true;
        return false;
    }
    
    this->hasAudioData = true;
    
    ///Need to buffer the audio data!
    //alBufferData(buffer, format, data, size, freq);
    //For format, should I use function convert_to_AL_format?
    //I would need to know the sample rate per channel though...
    alBufferData(this->buffer, AL_FORMAT_MONO16, this->audioData->getRawData()->data(), this->audioData->getRawDataSampleCount()*4, this->audioData->getSampleRate());
    
    //alBufferData(this->buffer, AL_FORMAT_STEREO16, this->audioData->getRawData()->data(), this->audioData->getRawDataSampleCount()*4, this->audioData->getSampleRate());
    
    this->encounteredError = (this->encounteredError || checkForError());
    
    ///Bind the source and the buffer together
    alSourcei(this->source, AL_BUFFER, this->buffer);
    return (!(this->encounteredError));
}

void AudioSource::playSource() {
    ///This works, but it blocks the rest of the program's execution...
    std::thread audio(playSourceThread, this->source);
    std::cout << "\nPlaying audio from source: " << this->source << ", Buffer: " << this->buffer << "\n";
    ///Print out the source's position
    std::array<ALfloat, 4> posVec = { {0.0f, 0.0f, 0.0f, 0.0f} }; //aggregate initialization
    //see: https://stackoverflow.com/questions/8192185/using-stdarray-with-initialization-lists
    this->getPosition(&posVec[0], &posVec[1], &posVec[2]);
    std::cout << "       Source Position:  \n";
    std::cout << "           < " << posVec[0] << ", " << posVec[1] << ", " <<
                 posVec[2] << " >" << std::endl;
    
    
    ///Join the thread
    audio.join();
    
//    ///Instead I am going to try to use std::future and std::async
//    std::future<void> test;
//    //test = std::async(std::launch::async, playSourceThread, this->source);
//    test = std::async(std::launch::async, playSourceThread, this->source);
}

/*
void AudioSource::playSource(bool * kill) {
    if (kill == nullptr) {
        playSource();
        return;
    }
    else {
        std::thread audio(playSourceThread, this->source);
    }
}
*/

///The function convert_to_AL_format could be necessary for a different
///audio loading library than LibAudioDecoder. LibAudioDecoder (claims it) always
///produces the same formatted output. I have hardly tested this yet though
//inline ALenum AudioSource::convert_to_AL_format(short channels, short samples) {
//    bool stereo = (channels > 1) ;
//    switch (samples) {
//        case 16:
//            if (stereo)
//                return AL_FORMAT_STEREO16;
//            else
//                return AL_FORMAT_MONO16;
//        case 8:
//            if (stereo)
//                return AL_FORMAT_STEREO8;
//            else
//                return AL_FORMAT_MONO8;
//        default:
//            return -1;
//    }
//}

void AudioSource::updatePosition(ALfloat x, ALfloat y, ALfloat z) {
    alSource3f(this->source, AL_POSITION, x, y, z);
    this->encounteredError = (this->encounteredError || checkForError());
}

void AudioSource::getPosition(ALfloat* x, ALfloat* y, ALfloat* z) {
    alGetSource3f(this->source, AL_POSITION, x, y, z);
    this->encounteredError = (this->encounteredError || checkForError());
}
               
bool AudioSource::checkForError() {
    ALCenum error;
    error = alGetError(); //This pops the error stack
    if (error != AL_NO_ERROR) {
        std::cout << "\nDetected that an error has occured in the audio language. Disabling audio...\n";
        std::cout << "The Library required to print out the error message explaining what the ";
        std::cout << "\nerror was has not been updated in over 10 years, so no more information is\n";
        std::cout << "available on the error at this time...\n"; ///Maybe try that code for printing enums?
        ///Print Error? Need alutGetErrorString()  ///ALUT hasn't been updated in 10 years on github...
        //this->mIsValid = false;
        return true;
    }
    return false;
}


