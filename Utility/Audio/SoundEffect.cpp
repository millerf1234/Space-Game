//
//  SoundEffect.cpp
//  Star Suzerian
//
//  Created by Forrest Miller on 6/23/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#include "SoundEffect.h"

static unsigned long NextSoundEffectIDNumber = 0;

SoundEffect::SoundEffect() {
    initialize();
}

SoundEffect::SoundEffect(const char * filepath) {
    initialize();
    
    ///This might work
    audioBufferPtrs.push_back(std::make_unique<AudioBufferWrapper>());
    audioBufferPtrs[0]->filepath = filepath;
    audioBufferPtrs[0]->audioData = new AudioData(filepath); ///Make this unique_ptr?
    ///If there was an error in the audio file, report it and clean up
    if (!(audioBufferPtrs[0]->audioData->validData()) ||
        !(audioBufferPtrs[0]->audioData->validDecoder())) {
        std::cout << "\nAn error occured while parsing the audio data for: ";
        std::cout << filepath << std::endl;
        this->mEncounteredError = true;
        //Pop the invalid AudioBuffer off the end of the vector
        audioBufferPtrs.pop_back();
        return;
    }
    ///Otherwise, if there wasn't an error, then the file must be valid
    alGetError(); //Clear any existing errors before starting (hmm I guess there is an error before this...)
    this->mHasFilepath = true;
    
    ///Now we must set up a buffer on the OpenAL side of things for the data...
    ALuint tempBuffer = 0u;
    alGenBuffers(static_cast<ALuint>(1), &tempBuffer);
    audioBufferPtrs[0]->bufferID = tempBuffer;
    this->mEncounteredError = mEncounteredError || checkIfErrorInOpenAL();
    std::cout << "\nBuffer is: " << audioBufferPtrs[0]->bufferID << "\n";
    ///Here we send the data to OpenAL's buffer  (can either be Mono16 or Steroeo16)
    alBufferData(audioBufferPtrs[0]->bufferID, AL_FORMAT_MONO16, audioBufferPtrs[0]->audioData->getRawData()->data(), audioBufferPtrs[0]->audioData->getRawDataSampleCount()*4,  audioBufferPtrs[0]->audioData->getSampleRate());
    
    alBufferData(audioBufferPtrs[0]->bufferID, AL_FORMAT_STEREO16, audioBufferPtrs[0]->audioData->getRawData()->data(), audioBufferPtrs[0]->audioData->getRawDataSampleCount()*4,  audioBufferPtrs[0]->audioData->getSampleRate());
    
    this->mEncounteredError = mEncounteredError || checkIfErrorInOpenAL();
    
    if (!mEncounteredError) {
        this->mValid = true;
    }
    
    /*
    ///This doesn't work:
    ///Construct a new AudioBufferWrapper object onto the end of the vector
    audioBuffers.push_back(AudioBufferWrapper()); //Construct into the vector
    std::vector<AudioBufferWrapper>::reverse_iterator newAudioBuffer = audioBuffers.rend(); //retrieve
    //set up
    (*newAudioBuffer).filepath = filepath;
    (*newAudioBuffer).audioData = new AudioData(filepath);
    ///If there was an error in the audio file, report it and clean up
    if (!((*newAudioBuffer).audioData->validData()) ||
        !((*newAudioBuffer).audioData->validDecoder())) {
        std::cout << "\nAn error occured while parsing the audio data for: ";
        std::cout << filepath << std::endl;
        this->mEncounteredError = true;
        //Pop the invalid AudioBuffer off the end of the vector
        audioBuffers.pop_back();
        return;
    }
    ///Otherwise, if there wasn't an error, then the file must be valid
    alGetError(); //Clear any existing errors before starting (hmm I guess there is an error before this...)
    this->mHasFilepath = true;
    
    ///Now we must set up a buffer on the OpenAL side of things for the data...
    ALuint tempBuffer = 0u;
    //alGenBuffers(static_cast<ALuint>(1), &((*newAudioBuffer).bufferID));
    alGenBuffers(static_cast<ALuint>(1), &tempBuffer);
    (*newAudioBuffer).bufferID = tempBuffer;
    this->mEncounteredError = mEncounteredError || checkIfErrorInOpenAL();
    std::cout << "\nBuffer is: " << (*newAudioBuffer).bufferID;
    ///Here we send the data to OpenAL's buffer  (can either be Mono16 or Steroeo16)
    alBufferData((*newAudioBuffer).bufferID, AL_FORMAT_MONO16, (*newAudioBuffer).audioData->getRawData()->data(), (*newAudioBuffer).audioData->getRawDataSampleCount()*4, (*newAudioBuffer).audioData->getSampleRate());
    
    //alBufferData((*newAudioBuffer).bufferID, AL_FORMAT_STEREO16, (*newAudioBuffer).audioData->getRawData()->data(), (*newAudioBuffer).audioData->getRawDataSampleCount()*4, (*newAudioBuffer).audioData->getSampleRate());
    
    this->mEncounteredError = mEncounteredError || checkIfErrorInOpenAL();
    
    if (!mEncounteredError) {
        this->mValid = true;
    }   */
}

SoundEffect::~SoundEffect() {
    delete this->mName;
    this->mName = nullptr;
    
}

//SoundEffect::SoundEffect(const SoundEffect&) {
//    this->initialize();
//}

//SoundEffect::SoundEffect(const SoundEffect&&) {
//    this->initialize();
//}

//SoundEffect& SoundEffect::operator=(const SoundEffect&) {
//    this->initialize();
//}

//SoundEffect& SoundEffect::operator=(const SoundEffect&&) {
//    this->initialize();
//}

const char * SoundEffect::name() const {
    return ( mName->c_str() );
}

///This is just a temp function for now...
ALuint SoundEffect::getFirstBuffer() const {
    if (!this->mValid) {
        return 0u;
    }
    //return this->audioBuffers.data()[0].bufferID;
    return this->audioBufferPtrs[0]->bufferID;
}
        
void SoundEffect::initialize() {
    this->mSoundID = NextSoundEffectIDNumber++;
    std::ostringstream defaultName;
    defaultName << "Unnammed Sound " << this->mSoundID;
    this->mName = new std::string(defaultName.str());
    
    //Initialize all the booleans
    this->mValid = false;
    this->mHasFilepath = false;
    this->mHasName = false;
    this->mHasMultipleBuffers = false;
    this->mEncounteredError = false;
    
}

bool SoundEffect::checkIfErrorInOpenAL() {
    ALCenum error;
    error = alGetError(); //This pops the error stack
    if (error != AL_NO_ERROR) {
        std::cout << "\nWarning! An error has been detected in OpenAL!\n";
        std::cout << "\nOpenAL encountered error number: " << error << "\n";
        return true;
    }
    return false;
}
