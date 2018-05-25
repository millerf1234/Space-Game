//


//Code based off https://www.raywenderlich.com/48293/how-to-export-blender-models-to-opengl-es-part-1
//  SimpleObjLoader.h
//  OpenGLSetupTestWithSTB
//
//  Created by Forrest Miller on 1/25/18.
//

//Note for model analysis: I wrote this next snipit of code to find out the shortest vector in a model.
//Note that this was copy-pasted from main, so vertices is assumed to be 5 components, 3 positions and 2 tex coordinates. Thats why 4 and 5 (3 and 4 in array indexing) are skipped in forming the vector 'tmp'
//Find the point on the spaceship that is closest to the center:
//float shortestVecInVerts = 100.0f;
//int countr = 0;
//int posCounter = 0;
//aiVector3D tmp(0.0f, 0.0f, 0.0f);
////for (float f : vertices) {
//if (countr == 0) {
//    tmp.x = f;
//}
//else if (countr == 1) {
//    tmp.y = f;
//}
//else if (countr == 2) {
//    tmp.z = f;
//    if (tmp.Length() < shortestVecInVerts) {
//        shortestVecInVerts = tmp.Length();
//        std::cout << "posCounter is: " << posCounter << " and magnitude here is: ";
//        std::cout << shortestVecInVerts << std::endl;
//    }
//    ++posCounter;
//}
//++countr;
//countr %= 5;
//// }
//

#ifndef SimpleObjLoader_h
#define SimpleObjLoader_h

//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

// Model Structure
typedef struct /*Model*/ {
    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
} Model;

class SimpleObjLoader {
    
private:
    std::string filePath;
public:
    //Variables that are a part of this class
    Model model;
    float ** positions;
    float **texels;
    float **normals;
    int **faces;
    
    bool isLoaded;
    
    SimpleObjLoader() = delete; //Can't create a loader without a destination
    
    //SimpleObjLoader() {;} //uncomment this line and comment above line if want to allow construction without file location
    
    SimpleObjLoader(std::string fileLocation) {
        this->filePath = fileLocation;
        model = getOBJinfo(filePath);
        
        
//        // Model Data
//        positions[model.positions][3];    // XYZ
//        texels[model.texels][2];          // UV
//        normals[model.normals][3];        // XYZ
//        faces[model.faces][9];              // PTN PTN PTN //'PTN' is 'pos, texture, normal'
        
        //Use dynamic memory
        positions = new float*[model.positions];
        for (int i = 0; i < model.positions; i++) {
            positions[i] = new float[3];
        }
        
        texels = new float*[model.texels];
        for (int i = 0; i < model.texels; i++) {
            texels[i] = new float[2];
        }
       
        normals = new float*[model.normals]; //Make normal an array of pointers to float (i.e. an array of float arrays)
        for (int i = 0; i < model.normals; ++i) {
            normals[i] = new float[3];
        }
        
        faces = new int*[model.faces];
        for (int i = 0; i < model.faces; ++i) {
            faces[i] = new int[9];
        }
    
        
        this->extractOBJdata(filePath, positions, texels, normals, faces);
        
        isLoaded = true;
    }
    
    
    ~SimpleObjLoader() {
        //free dynamic memory
        //Free vertex data:
        for (int i = 0; i < model.positions; i++) {
            delete [] positions[i];
        }
        delete [] positions;
        
        //Delete texel data:
        for (int i = 0; i < model.texels; i++) {
            delete [] texels[i];
        }
        delete [] texels;
        
        //return to the heap memory used for normals
        for (int i = 0; i < model.normals; ++i) {
            delete[] normals[i];
        }
        delete [] normals;
        
        for (int i = 0; i < model.faces; ++i) {
            delete[] faces[i];
        }
        delete [] faces;
        
    }
    
    Model getOBJinfo(std::string fp)
    {
        // 2
        Model model = {0}; //The ' = {0}; '  means to initialize all of model's fields to 0
        
        // 3
        // Open OBJ file
        std::ifstream inOBJ;
        inOBJ.open(fp);
        if(!inOBJ.good())
        {
            std::cout << "ERROR OPENING OBJ FILE" << std::endl;
            std::cout << fp << std::endl;
            isLoaded = false;
            exit(EXIT_FAILURE);
        }
        
        // 4
        // Read OBJ file
        while(!inOBJ.eof()) //while not end of file
        {
            // 5
            //Get the first 2 characters of each line
            std::string line;
            getline(inOBJ, line);
            std::string type = line.substr(0,2);
            
            // 6
            if(type.compare("v ") == 0)
                model.positions++;
            else if(type.compare("vt") == 0)
                model.texels++;
            else if(type.compare("vn") == 0)
                model.normals++;
            else if(type.compare("f ") == 0)
                model.faces++;
        }
        
        // 7
        model.vertices = model.faces*3;
        
        // 8
        // Close OBJ file
        inOBJ.close();
        
        // 9
        return model;
    }
    
    
    
//    void extractOBJdata(std::string fp, float positions[][3], float texels[][2], float normals[][3], int faces[][9]) {
    void extractOBJdata(std::string fp, float **positions, float ** texels, float ** normals, int **faces) {
        // Counters
        int p = 0;
        int t = 0;
        int n = 0;
        int f = 0;
        
        // Open OBJ file
        std::ifstream inOBJ;
        inOBJ.open(fp);
        if(!inOBJ.good())
        {
            std::cout << "ERROR OPENING OBJ FILE" << std::endl;
            isLoaded = false;
            exit(1);
        }
        
        // Read OBJ file
        while(!inOBJ.eof())
        {
            std::string line;
            getline(inOBJ, line);
            std::string type = line.substr(0,2);
            
            // Positions
            if(type.compare("v ") == 0)
            {
                // 1
                // Copy line for parsing
                char* l = new char[line.size()+1];
                memcpy(l, line.c_str(), line.size()+1);
                
                // 2
                // Extract tokens
                strtok(l, " ");
                for(int i=0; i<3; i++)
                    positions[p][i] = atof(strtok(NULL, " "));
                
                // 3
                // Wrap up
                delete[] l;
                p++;
            }
            
            // Texels
            else if(type.compare("vt") == 0)
            {
                char* l = new char[line.size()+1];
                memcpy(l, line.c_str(), line.size()+1);
                
                strtok(l, " ");
                for(int i=0; i<2; i++)
                    texels[t][i] = atof(strtok(NULL, " "));
                
                delete[] l;
                t++;
            }
            
            // Normals
            else if(type.compare("vn") == 0)
            {
                char* l = new char[line.size()+1];
                memcpy(l, line.c_str(), line.size()+1);
                
                strtok(l, " ");
                for(int i=0; i<3; i++)
                    normals[n][i] = atof(strtok(NULL, " "));
                
                delete[] l;
                n++;
            }
            
            // Faces
            else if(type.compare("f ") == 0)
            {
                char* l = new char[line.size()+1];
                memcpy(l, line.c_str(), line.size()+1);
                
                strtok(l, " ");
                
                //I added this to prevent my cube test thing from crashing:
                if (this->model.faces == 12) {
                    for (int i = 0; i < 3; i++) {
                        faces[f][i] = atof(strtok(NULL, " /"));
                    }
                }
                else {
                for(int i=0; i<9; i++)
                    faces[f][i] = atof(strtok(NULL, " /"));
                }
                delete[] l;
                f++;
            }
        }
        
        // Close OBJ file
        inOBJ.close();
    }
    
    
};



#endif /* SimpleObjLoader_h */
