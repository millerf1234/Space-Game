//
//  AudioInterface.h
//  Star Suzerian
//
//  Created by Forrest Miller on 6/21/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#ifndef AudioInterface_h
#define AudioInterface_h

//#include <stdio.h>
class AudioInterface {
public:
    AudioInterface();
    virtual ~AudioInterface();
    
    ///Audio Functions
    virtual void createAudioSource(long & handle /* , get a file handle for noise or something? */ ) = 0;
    virtual bool checkIfSourceActive(const long & handle) const = 0;
    //Add more functions, then have game do all it's interfacing through this class
    
    //No copying
    AudioInterface & operator=(const AudioInterface &) = delete;
    AudioInterface(const AudioInterface & other) = delete;
private:
    
    
};

#endif // AudioInterface_h
