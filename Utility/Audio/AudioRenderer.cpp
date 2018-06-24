//
//  AudioRenderer.cpp
//  Star Suzerian
//
//  Created by Forrest Miller on 6/20/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

#include "AudioRenderer.h"

///OpenAL docuemntation at: https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf

///Run audio in a seperate detatched thread? see: http://drdobbs.com/go-parallel/article/showArticle.jhtml?articleID=225700095
///Or do i need std::async with std::future? see: https://stackoverflow.com/questions/30810305/confusion-about-threads-launched-by-stdasync-with-stdlaunchasync-parameter


static constexpr int DEVICE_TO_USE = 1u; ///Change this value to select a different default audio device

static constexpr size_t MAX_DEVICE_NAME_LENGTH = 512u;
static constexpr size_t MAX_DEVICE_NAMES_TO_RECORD = 32u;

AudioRenderer::AudioRenderer(bool printMessages) {
    mDevice = nullptr;
    mContext = nullptr;
    mIsValid = initialize(printMessages);
}

AudioRenderer::~AudioRenderer() {
    if (this->mIsValid) {
        //Will need to free every single 'generated' object
        //glDeleteSources(1, &source);
        //glDeleteBuffers(1, &buffer);
        mDevice = alcGetContextsDevice(mContext);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(mContext);
        alcCloseDevice(mDevice);
    }
}

void AudioRenderer::getListenerPosition(float & x, float & y, float & z) {
    x = y = z = 0.0f;
    if (this->getIsValid()) {
        alGetListener3f(AL_POSITION, &x, &y, &z);
    }
    if ( checkForError() ) {
        std::cout << "\nError occured during function call to getListenerPosition!\n";
    }
}

void AudioRenderer::getListenerVelocity(float & x, float & y, float & z) {
    x = y = z = 0.0f;
    if (this->getIsValid()) {
        alGetListener3f(AL_VELOCITY, &x, &y, &z);
    }
    if ( checkForError() ) {
        std::cout << "\nError occured during function call to getListenerVelocity!\n";
    }
}

void AudioRenderer::getListenerOrientation(ALfloat * vectorOfSixFloats) {
    vectorOfSixFloats[0] = 0.0f;
    vectorOfSixFloats[1] = 0.0f;
    vectorOfSixFloats[2] = 0.0f;
    vectorOfSixFloats[3] = 0.0f;
    vectorOfSixFloats[4] = 0.0f;
    vectorOfSixFloats[5] = 0.0f;
    if (this->getIsValid()) {
        alGetListenerfv(AL_ORIENTATION, vectorOfSixFloats);
    }
    if ( checkForError() ) {
        std::cout << "\nAn Error occured during function call to getListenerOrientation!\n";
    }
}

float AudioRenderer::getListenerGain() {
    float result = -100.0f;
    alGetListenerf(AL_GAIN, &result); //alGetListenerfv(AL_GAIN, &result) //returns same thing
    checkForError();
    if (result < 0.0f) {
        std::cout << "\nAn error occured getting the gain!\n";
    }
    return result;
}

void AudioRenderer::setListenerGain(float gain) {
    if (gain < 0.0f) {
        std::cout << "\nError! Unable to set the listener's gain to a negative value!\n";
        std::cout << "A function call was made to set gain to " << gain << " but gain\n";
        std::cout << "must be a positive value!\n";
        return;
    }
    this->configureListenerGain(gain);
}

bool AudioRenderer::initialize(bool shouldPrintMessages) {
    if (shouldPrintMessages) {
        std::cout << "Loading Audio Language...\n";
    }
    
    /// Enumerate all of the avaialable audio devices
    if (shouldPrintMessages) {
        std::cout << "    Enumerating Available Audio Devices...\n  ";
    }
    std::vector<const ALCchar*> devices;
    enumerateAudioDevices(devices);
    size_t numDetectedDevices = devices.size();
    if (shouldPrintMessages) {
        printNamesOfEnumeratedDevices(numDetectedDevices, devices);
    }
    if (numDetectedDevices == 0) {
        std::cout << "\nERROR!\nNo Valid Audio Devices were detected for this system!\n";
        return false;
    }
    
    ///Choose a device from the enumerated devices
    if (numDetectedDevices == 1) {  ///If only 1 device was detected
        if (shouldPrintMessages) {
            std::cout << "    Only one audio-capable device was detected!\n";
            std::cout << "        Using device: ";
            std::cout << devices[0] << std::endl;
        }
        static const ALCchar* empty = "\0";
        if (!createAudioDevice(empty) ) {//passing in empty string just opens up the default device
            std::cout << "\nError creating audio device!\n";
            return false;
        }
    }
    else {
        if (DEVICE_TO_USE > devices.size() - 1u) { ///If class is set to use a device that is off the end of the list of available devices
            std::cout << "\nThis class is set to try to use device " << DEVICE_TO_USE + 1u;
            std::cout << " but only " << devices.size() << "\ndevices are available\n";
            std::cout << "The default device will be used!\n";
            static const ALCchar* empty = "\0";
            if (!createAudioDevice(empty) ) { //passing in empty string just opens up the default device
                std::cout << "\nError creating audio device!\n";
                return false;
            }
        }
        else {
            if (shouldPrintMessages) {
                std::cout << "    Multiple available audio devices were detected. This class is\n";
                std::cout << "       currently set to try to use the device in position ";
                std::cout << DEVICE_TO_USE << "\n";
                std::cout << "       which is: " << devices[DEVICE_TO_USE] << "\n";
            }
            if (!(createAudioDevice(devices[DEVICE_TO_USE]))) {
                std::cout << "\nError creating audio device!\n";
                return false;
            }
            else if (shouldPrintMessages) {
                std::cout << "\n    Audio Device SUCCESSFULLY Created!\n";
            }
        }
    }
    
    //Do a quick reality check
    if (!mDevice) {
        std::cout << "\nOOPS! It looks like mDevice in NULL even though this should\n";
        std::cout << "not ever happen if I wrote my code right. Check code for where\n";
        std::cout << "the null device creation is slipping through!\n";
        return false;
    }
    
    
    ///Now create a context
    if (shouldPrintMessages) {
        std::cout << "    Creating Audio Language Context!\n";
    }
    if ( !createAudioContext() ) {
        std::cout << "\nError creating Audio Language Context!\n";
        return false;
    }
    //Do another quick reality check
    if (!mContext) {
        std::cout << "\nOOPS! It looks like mContext in NULL even though this should\n";
        std::cout << "not ever happen if I wrote my code right. Check code for where\n";
        std::cout << "the null context creation is slipping through!\n";
        return false;
    }
    
    ///Set up the position for the listener object
    if (shouldPrintMessages) {
        std::cout << "    Creating a listener object...\n";
        std::cout << "    Setting the position for the listener object...\n";
    }
    if ( !configureListenerPosition() ) {
        std::cout << "\nAn error occured while configuring the listener object's\n";
        std::cout << "position!\n";
        return false;
    }
    
    if (shouldPrintMessages) {
        std::cout << "    Listener Position successfully matched to\n";
        std::cout << "    static camera's position!\n";
        
        std::cout << "    Setting gain for listener!\n";
    }
    
    ///Set the gain for the listener object   (This acts as a 'Master Gain')
    float defaultGain = 5.0f;
    if ( !configureListenerGain(defaultGain) ) {
        std::cout << "\nAn ERROR occured while setting the default listener's gain\n";
        return false;
    }
    
    if (shouldPrintMessages) {
        std::cout << "    Listener set-up complete!\n";
    }

    return true;
}

bool AudioRenderer::checkForError() {
    ALCenum error;
    error = alGetError(); //This pops the error stack
    if (error != AL_NO_ERROR) {
        std::cout << "\nDetected that an error has occured in the audio language. Disabling audio...\n";
        std::cout << "The Library required to print out the error message explaining what the ";
        std::cout << "\nerror was has not been updated in over 10 years, so no more information is\n";
        std::cout << "available on the error at this time...\n"; ///Maybe try that code for printing enums?
        ///Print Error? Need alutGetErrorString()  ///ALUT hasn't been updated in 10 years on github...
        this->mIsValid = false;
        return true;
    }
    return false;
}

bool AudioRenderer::createAudioDevice(const ALCchar* deviceName) {
    if (*deviceName == '\0') {
        mDevice = alcOpenDevice(NULL);
    }
    else {
        mDevice = alcOpenDevice(deviceName);
        if (!mDevice) {
            std::cout << "\nError creating device for named device: " << deviceName;
            std::cout << "\nAttempting to use default device...";
            mDevice = alcOpenDevice(NULL);
            if (mDevice) {
                std::cout << "\nSuccess using the default device!\n";
            }
        }
    }
    if (checkForError()) { return false; } //Need to do checkForError() after each AL func call to clear error stack
    if (!mDevice) {
        std::cout << "\nWARNING! THERE WAS AN ERROR WHILE ATTEMPTING TO RETRIEVE\n"
        "A HANDLE TO THE AUDIO DEVICE. This means no audio will function\n"
        "correctly. Press enter to continue...";
        std::cin.get();
        return false;
    }
    return true;
}

void AudioRenderer::enumerateAudioDevices(std::vector< const ALCchar*>& devices) {
    ALboolean enumeration;
    //Check to see if this OpenAL implementation supports device enumeration
    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");///First parameter as NULL means we want to retrieve information from all available devices
    if (checkForError()) {
        std::cout << "\nError during enumeration of available audio devices!\n";
        return;
    }
    if (enumeration == AL_FALSE) {
        std::cout << "\nWARNING! ENUMERATION IS NOT SUPPORTED BY THIS OPENAL \n"
        "IMPLEMENTATION! Default device will be selected!\n";
        //std::cout << "\nPress enter to continue...\n";
        //std::cin.get();
        static const ALchar * defaultDeviceName = const_cast<const ALchar*>("r(default\0)"); //Could be a pitfall down the road...
        devices.push_back(defaultDeviceName);
        return;
    }
    
    //If enumeration is supported, List the available audio devices
    parseDevicesIntoVector(devices);
    return;
}

bool AudioRenderer::createAudioContext() {
    mContext = alcCreateContext(mDevice, NULL);
    if (checkForError()) {
        std::cout << "\nError during Creation of Audio Context!\n";
        mIsValid = false;
        return false;
    }
    
    //make the context current
    if (!alcMakeContextCurrent(mContext)) {
        std::cout << "\nAn error occured while attempting to make the OpenAL context current!\n";
        mIsValid = false;
        return false;
    }
    if (checkForError()) {
        std::cout << "\nError while making OpenAL context current!\n";
        mIsValid = false;
        return false;
    }
    auto temp = alGetError();
    
    return true;
}

///Configures the starting position and orintation of the 'listener' for incoming audio
bool AudioRenderer::configureListenerPosition() {
    //See: https://stackoverflow.com/questions/7861306/clarification-on-openal-listener-orientation
    // for information on listener orientation
    ALfloat listenerOrientation[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f}; 
    ///Set the listener's position and check for errors
    alListener3f(AL_POSITION, 0.0f, 0.0f, 1.0f);
    if (checkForError() ) {
        std::cout << "\nAn error occured while setting the listener's position "
        "in the audio language!\n";
        return false;
    }
    ///Set the listener's velocity and check for errors
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    if (checkForError() ) {
        std::cout << "\nAn error occured while setting the listener's velocity "
        "in the audio language!\n";
        return false;
    }
    
    ///Set the listener's orientation and check for errors
    alListenerfv(AL_ORIENTATION, listenerOrientation);
    if (checkForError() ) {
        std::cout << "\nAn error occured while setting the listener's orientation "
        "in the audio language!\n";
        return false;
    }
    return true;
}

bool AudioRenderer::configureListenerGain(float gain) {
    ///It is assumed that 'gain' has already been checked to make sure it is positive 
    alListenerf(AL_GAIN, gain);
    checkForError();
    return true;
}

//The list of devices we recieve from the OpenAL implementation will be a contiguous
//string of characters with each device seperated by a single '\0' character and the
//end of the list being signified by two '\0' characters.
//i.e.  the format is   "device1\0device2\0device3\0\0"
void AudioRenderer::parseDevicesIntoVector(std::vector<const ALchar*>& devices) {
    const ALchar * deviceNames = alcGetString(NULL, ALC_DEVICE_SPECIFIER); //Get device list
    if (checkForError()) { return; }
    bool reachedEndOfList = false;
    size_t nameLength = 0;
    std::ostringstream deviceName;
    const ALchar * nameIterator, *nextAfterNameIterator;
    nameIterator = deviceNames;
    nextAfterNameIterator = (deviceNames + 1);
    do {
        if ( (*nameIterator) == '\0') {
            nameLength = 0;
            static std::vector<std::string> names;
            names.push_back(deviceName.str());
            std::vector<std::string>::iterator mostRecentName = names.end();
            mostRecentName--; //Move iterator to be at the last item in the vector
            devices.push_back(static_cast<const ALchar *>((*mostRecentName).c_str()));
            if (devices.size() > MAX_DEVICE_NAMES_TO_RECORD) {
                std::cout << "\n\nWarning! Excessive number of Audio Devices detected!\n";
                std::cout << "Only the first " << MAX_DEVICE_NAMES_TO_RECORD;
                std::cout << " devices will be recorded!" << std::endl;
                return;
            }
            //clear the stringstream
            std::ostringstream().swap(deviceName);
            
            //Check to see if we reached the end of the list
            if ( (*nextAfterNameIterator) == '\0' ) {
                reachedEndOfList = true;
                continue;
            }
        }
        else {
            deviceName << (*nameIterator);
            if (deviceName.str().size() > MAX_DEVICE_NAME_LENGTH) {
                std::cout << "\nWARNING! One of the detected audio devices ";
                std::cout << "has a name that exceeds the maximum name limit\n";
                deviceName << '\0'; //Give the device name a terminating character
                std::cout << "\nThe device name that is causing the issue is: \n";
                std::cout << deviceName.str() << std::endl;
                return;
            }
            nameIterator = nextAfterNameIterator++;
        }
    } while (!reachedEndOfList);
}

void AudioRenderer::printNamesOfEnumeratedDevices(size_t numDetectedDevices,
                                                  const std::vector<const ALCchar*>&devices) const {
    if (numDetectedDevices == 0) {
        std::cout << "\nERROR! No Audio Devices were detected on this system!\n";
        return;
    }
    else if (numDetectedDevices == 1) {
        std::cout << "    One Audio Device was detected by this system.\n";
        std::vector<const ALchar*>::const_iterator it = devices.begin();
        std::cout << "    DEVICE NAME: " << (*it) << std::endl;
    }
    else { //Multiple audio devices were detected
        std::cout << "    A total of " << numDetectedDevices << " audio devices "
        "were detected!\n";
        std::cout << "    DEVICE NAMES:\n";
        std::vector<const ALchar*>::const_iterator it = devices.begin();
        for ( ; it != devices.end(); it++) {
            std::cout << "      " << (*it) << "\n";
        }
    }
}
