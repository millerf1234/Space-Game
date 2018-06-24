//
//  AudioData.cpp
//  Star Suzerian
//
//  Created by Forrest Miller on 6/21/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#include "AudioData.h"

static constexpr int NOT_SET = -1;


AudioData::AudioData(const char * audioFile) {
    this->filepath = audioFile;
    initialize();
    
    ///Create an audio decoder to decode the file
    std::string filename = audioFile;
    decoder = new AudioDecoder(filename);
    
    ///Tell the audio decoder to open the file
    int openResult = decoder->open();
    if (openResult != 0) {
        std::cout << "\nERROR OPENING AUDIO FILE: " << filepath << "\n";
        std::cout << "\nNo audio will be played for this object!\n";
        return;
    }
    this->validAudioDecoder = true;
    
    //for debug:
    //std::cout << "Decoder was passed the file: " << filename;
    //std::cout << "\nThe result of calling open() is: " << openResult << std::endl;
    
    ///If the file opened successfully, get information from the file
    this->numSamples = decoder->numSamples();
    this->numChannels = decoder->channels();
    this->sampleRate = decoder->sampleRate();
    this->durationInSeconds = decoder->duration();
    
    ///Extract the raw data as samples
    rawData = new std::vector<SAMPLE>();
    ///Note that numSamples if guarenteed to be completely accurate.
    ///To accomodate for under-reporting on the number of samples, we can reserve some extra space in our vector.
    ///Files above 'tolerance' samples in length will be expanded at a smaller percentage
    int tolerance = 44100;
    if (numSamples < tolerance) {
        rawData->reserve(static_cast<size_t>(ceil(numSamples * 1.15))); //1.15 for some extra space
    }
    else { ///If the file is big enough then adding just 5% more space should give enough
        rawData->reserve(static_cast<size_t>(ceil(numSamples * 1.05))); //1.05 for some extra space
    }
    numberOfSamplesInRawData = decoder->read(numSamples, rawData->data());
    this->validAudioData = true;
    
    //Debug:
    this->printToScreen();
    std::cout << "\nRead " << numberOfSamplesInRawData << " samples!\n";
    
}

AudioData::~AudioData() {
    delete this->decoder;
    this->decoder = nullptr;
    validAudioData = validAudioDecoder = false;
    
    delete this->rawData;
    rawData = nullptr;
}

int AudioData::getCurrentPlaybackSamplePostion() const {
    if (decoder == nullptr) {
        return NOT_SET;
    }
    return decoder->positionInSamples();
}

void AudioData::printToScreen() {
    std::cout << "\nDetails of Audio File: \n";
    std::cout << "Samples: " << numSamples << "\n";
    std::cout << "Channels: " << numChannels << "\n";
    std::cout << "Sample rate: " << sampleRate << "\n";
    std::cout << "Duration: " << durationInSeconds << " seconds\n";
}

bool AudioData::seek(int sampleIdx) {
    if (decoder == nullptr) {
        return false;
    }
    
    if ( (sampleIdx < 0) || (sampleIdx >= this->numSamples)  ) {
        std::cout << "\nOops! Being asked to seek to a position that is outside\n";
        std::cout << "the size of this audio file!\n\n";
        decoder->seek(0);
        sampleIndex = 0;
        return true;
    }
    //else
    decoder->seek(sampleIdx);
    sampleIndex = sampleIdx;
    return true;
}

void AudioData::initialize() {
    decoder = nullptr;
    rawData = nullptr;
    numSamples = NOT_SET;
    numChannels = NOT_SET;
    sampleRate = NOT_SET;
    sampleIndex = NOT_SET;
    durationInSeconds = 0.0f;
    numberOfSamplesInRawData = 0;
    
    validAudioData = false;
    validAudioDecoder = false;
}


//class AudioData {
//public:
//    AudioData() = delete;
//    AudioData(const AudioData & other) = delete;
//    AudioData & operator=(const AudioData & other) = delete;
//
//    AudioData(const char * audioFile);
//    ~AudioData();
//
//    bool validData() const { return validAudioData; }
//
//    bool operator==(const AudioData & other) {
//        if (strcmp(this->filepath, other.filepath) == 0)  { return true; }
//        else { return false; }
//    }
//
//private:
//    bool validAudioData;
//    const char * filepath;

//    int numSamples;
//    int numChannels;
//    int sampleRate;
//    float durationInSeconds;
//};
