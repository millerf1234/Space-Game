//  This is a more robust version of the SimpleObjLoader object.
//  However this time around it is implemented just as a function instead of as
//  an entire object.
//
///IMPORTANT NOTE! THIS CLASS IS NO WHERE NEAR FINISHED! DON"T USE THIS CLASS AT ALL UNTIL IT IS FINISHED!
//
//  LoadObjFile.h
//
//  Created by Forrest Miller on 6/4/18.

#pragma once

#ifndef OBJ_LOADER
#define OBJ_LOADER

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h> //Gives access to C functions such as atoi, atof, etc...
#include <sstream>
#include <vector>


namespace ModelUtility {
    
    typedef struct ModelStatistics {
        int vertices; ///A vertex is a position, texel and normal
        int positions;
        int texels;
        int normals;
        int faces; ///A face is composed of 3 vertices (assuming no quads)
        
        ///Constructor
        ModelStatistics() {
            vertices = positions = texels = normals = faces = 0;
        }
        ///Member Function
        std::string getModelInfo(std::string indent = "") const {
            std::stringstream ss;
            ss << indent << "Number of positions: " << positions << "\n";
            ss << indent << "Number of texels:    " << texels    << "\n";
            ss << indent << "Number of normals:   " << normals   << "\n";
            ss << indent << "Number of vertices:  " << vertices  << "\n";
            ss << indent << "Number of faces:     " << faces     << "\n";
            return ss.str();
        }
    } ModelStatistics;
    
    typedef struct ModelData {
        //Fields
        ModelStatistics modelInfo;
    
        bool hasBeenLoaded;
        std::vector<float>    positions;
        std::vector<float>    texels;
        std::vector<float>    normals;
        std::vector<float>    vertices; ///This is the one that should be buffered
        std::vector<uint32_t> elements; ///i.e. faces
        
        ///Constructor
        ModelData() {
            hasBeenLoaded = false;
        }
        ///Member Function
        std::string getModelInfo(std::string indent = "") const {
            if (hasBeenLoaded) {
                return modelInfo.getModelInfo(indent);
            }
            else {
                return (indent + "No Model Loaded\n");
            }
        }
        
    } ModelData;
    
    
    
    bool loadObjFile(std::string fileLocation, ModelData & modelData);
    
    
    
} //Namespace ModelUtility
#endif // OBJ_LOADER
