//  Class for holding all of the game's shaders. Not sure if I am going to use this...
//  Probably not going to use this...
//  GameShaders.h
//
//  Created by Forrest Miller on 2/13/18.
//

#ifndef GameShaders_h
#define GameShaders_h

#include <iostream>
#include <vector>
#include "ShaderWrapper.h"

typedef struct ShaderNode {
    int shaderID;
    ShaderWrapper effect;
    
} ShaderNode;

class GameShaders {
public:
    //Constructor
    GameShaders();
    
    //Destructor
    ~GameShaders();
    
    
private:
    std::vector<ShaderNode *> shaders;
    static int shaderCount;
    
};


#endif /* GameShaders_h */
