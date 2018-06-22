//
//  AudioData.h
//  Star Suzerian
//
//  Created by Forrest Miller on 6/21/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//
//The Audio Decoder library I am using has documentation at: https://github.com/asantoni/libaudiodecoder/blob/master/README.md

#ifndef AudioData_h
#define AudioData_h

#include <stdio.h> //for FILE*
#include <iostream>
#include <string>
#include <vector>
//#include <fstream>
#include "audiodecoder.h" //"audiodecoder/audiodecoder.h" //libaudiodecoder library include


class AudioData {
public:
    AudioData() = delete;
    AudioData(const AudioData & other) = delete;
    AudioData & operator=(const AudioData & other) = delete;
    
    AudioData(const char * audioFile);
    ~AudioData();
    
    int getCurrentPlaybackSamplePostion() const;
    
    bool validData() const { return this->validAudioData; }
    bool validDecoder() const { return this->validAudioDecoder; }
    int getNumSamples() const { return this->numSamples; }
    int getNumChannels() const { return this->numChannels; }
    int getSampleRate() const { return this->sampleRate;}
    float getDurationInSeconds() const { return this->durationInSeconds; }
    bool operator==(const AudioData & other) {
        if (strcmp(this->filepath, other.filepath) == 0)  { return true; }
        else { return false; }
    }
    
    void printToScreen();
    
    ///Seek doesn't do anything currently since the entire audio file is parsed at construction
    bool seek(int sampleIdx); //Skips ahead to sample number index. Index should be in range of sample plz
    
    int getRawDataSampleCount() const { return this->numberOfSamplesInRawData; }
    ///'SAMPLE' is just an alias for 'float'
    std::vector<SAMPLE>* getRawData() const { return this->rawData; }
    
private:
    
    bool validAudioData;
    const char * filepath;
    bool validAudioDecoder;
    AudioDecoder * decoder;
    std::vector<SAMPLE> * rawData;
    
    int numSamples;
    int numChannels;
    int sampleRate;
    float durationInSeconds;
    int numberOfSamplesInRawData;
    
    int sampleIndex;
    void initialize();
    
    
};


#endif // AudioFileData_h
