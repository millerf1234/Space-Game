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

#include "OpenAL.h"

class AudioSource {
public:
    AudioSource() = delete;
    AudioSource(const AudioSource & other) = delete;
    AudioSource & operator=(const AudioSource & other) = delete;
    
    AudioSource(ALfloat x, ALfloat y, ALfloat z, bool looping = false);
    AudioSource(ALfloat x, ALfloat y, ALfloat z, ALfloat pitch, ALfloat gain, bool looping = false);
    AudioSource(ALfloat positionX, ALfloat positionY, ALfloat positionZ, ALfloat velocityX, ALfloat velocityY, ALfloat velocityZ, ALfloat pitch, ALfloat gain, bool looping = false);
    
private:
    ALuint source;
    ALuint buffer;
    
};



#endif // AudioSource_h
