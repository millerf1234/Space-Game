////  Implementation of the background class
////  Consists of 2 parts, the BackGroundGenerator and instances of the background
////
////  Background.cpp
////
////  Created by Forrest Miller on 2/14/18.
//
//#include "Background.h"
//
//constexpr int INDEX_SHIFT = 1;
//
//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///////////////////////  Background Generator Implementation   ///////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////The ": ObjectGenerator()" means objectGenerator's constructor is called before this begins to execute
//BackGroundGenerator::BackGroundGenerator() : ObjectGenerator() {
//    //So ObjectGenerator's constructor initialiazed most fields to 0 or nullptr,
//    //so we need to set all the fields needed for BackgroundGenerator:
//    
//    //Set non-pointer variables
//    this->numberOfVertices = TEXTURESQUARE_VERTS_SIZE;
//    this->numberOfElements = TEXTURESQUARE_ELEMENTCOUNT;
//    this->numberOfTextures = 1; //To start out with
//    
//    //Load Textures
//    this->textures = new TextureWrapper * [numberOfTextures];
//    //stb_image is smart enough to deduce filetypes on its own
//    //this->textures[0] = new TextureWrapper(BACKGROUND_TEXTURE_IMAGES[0], ImgFormat::JPEG);
//    this->textures[0] = new TextureWrapper(BACKGROUND_TEXTURE_IMAGES[0]);
//    
//    //Load Model Vertices (here the background data is just a static array defined in Background.h)
//    //Create a vbo on the heap
//    this->vbo = new GLuint;
//    glGenBuffers(1, this->vbo);
//    
//    //Create an ebo on the heap
//    this->ebo = new GLuint;
//    glGenBuffers(1, this->ebo);
//    
////    //Create a tex on the heap (actually let TextureWrapper handle that on second thought)
////    this->tex = new GLuint;  //Put this stuff in textureWrapper?
////    glGenTextures(1, this->tex);
//    
//    
//    //Load the vertices
//    this->vertices = new GLfloat [TEXTURESQUARE_VERTS_SIZE];
//    for (int i = 0; i < TEXTURESQUARE_VERTS_SIZE; ++i) {
//        vertices[i] = TEXTURESQUARE_VERTS[i];
//    }
//    
//    //Load element data
//    this->elements = new GLuint [TEXTURESQUARE_ELEMENTCOUNT];
//    for (int i = 0; i < TEXTURESQUARE_ELEMENTCOUNT; ++i) {
//        elements[i] = TEXTURESQUARE_ELEMENTS[i];
//    }
//    
//    this->hasModelData = true;
//    
//    //Load Shader
//    this->shader = new ShaderWrapper();
//    this->shader->attachVert(BACKGROUND_VERT_SHADER);
//    this->shader->attachFrag(BACKGROUND_FRAG_SHADER);
//    if (this->shader->readyToLink()) {
//        this->shader->link();
//    }
//    else {
//        std::cout << "\n\nDEBUG::For some reason, the Frag or Vert did not attach correctly!\n";
//        return;
//    }
//    this->shader->setVertexAttribName("position");
//    this->shader->setColorAttribName("colr");
//    this->shader->setTextureAttribName("texCoord");
//    
//    this->shader->specifyVertexLayout(ShaderWrapper::VERT2COLOR3TEXEL2);
//    
//    this->hasValidShader = true;
//    
//}
//
//BackGroundGenerator::~BackGroundGenerator() {
//    //Delete everything that was claimed by this class with 'new'
//    if (this->vbo != nullptr) {
//        glDeleteBuffers(1, this->vbo); //Won't be a problem if vbo is invalid
//        delete this->vbo;
//        this->vbo = nullptr;
//    }
//    if (this->ebo != nullptr) {
//        glDeleteBuffers(1, this->ebo); //Won't be a problem if ebo is invaid
//        delete this->ebo;
//        this->ebo = nullptr;
//    }
//    //if (this->tex != nullptr) {
//    //    glDeleteTextures(1, this->tex);
//    //    delete this->tex;
//    //    this->tex = nullptr;
//    //}
//    if (this->vertices != nullptr) {
//        delete this->vertices;
//        this->vertices = nullptr;
//    }
//    if (this->elements != nullptr) {
//        delete this->elements;
//        this->elements = nullptr;
//    }
//    
//    //Free up the textures too
//    for (int i = 0; i < this->numberOfTextures; ++i) {
//        if (this->textures[i] != nullptr) {
//            delete this->textures[i];
//            this->textures[i] = nullptr;
//        }
//    }
//    
//    //Delete the shader wrapper
//    if (this->shader != nullptr) {
//        delete this->shader;
//        this->shader = nullptr;
//    }
//}
//
//Object * BackGroundGenerator::generateInstance() {
////    Object * temp = (Object *)new Background;
////    return temp;
//    return new Background(++(this->nextObjID), this);
//}
//
//aiVector3D BackGroundGenerator::getStartingOrientation() {
//    return aiVector3D(0.0f, 0.0f, 0.0f); //Z might actually be negative
//}
//
//void BackGroundGenerator::addTexture(const std::string &fileLocation) {
//    
//    std::cout << "\n\nDEBUG::feature addTexture not yet implemented!\n";
//}
//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////  Background Implementation   ////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
//Background::Background(int objID, ObjectGenerator * gen) : Object(objID) {
//    this->makeStationary(); //Make it so background won't move
//   // this->thetaX = this->thetaY = this->thetaZ = 0.0f;//Set all angles to 0.0f
//    this->objData = gen;
//    this->bg = nullptr;
//    
//    std::cout << "OOPS. It looks like Background was called with the wrong constructor\n";
//}
//
//Background::Background(int objID, BackGroundGenerator * bg) : Object(objID) {
//    this->makeStationary(); //Make it so background won't move
//    // this->thetaX = this->thetaY = this->thetaZ = 0.0f;//Set all angles to 0.0f
//    this->objData = bg;
//    this->bg = bg;
//    
//    //Uniforms
//    //    float time;
//    //    float tex;
//    //    float zoom;
//    GLint textEnum = glGetUniformLocation(*(bg->shader[0].programID), "tex");
//    GLfloat time = glGetUniformLocation(*(bg->shader[0].programID), "time");
//    GLfloat zoom = glGetUniformLocation(*(bg->shader[0].programID), "zoom");;
//}
//
//Background::~Background() {
//    
//}
//
//void Background::recalculateForward() {
//  //  this->forward =
//}
//
//void Background::rotateXYZ() {
//    //Do nothing for the moment...
//}
//
//void Background::draw() {
//    if (this->objData == nullptr) {
//        std::cout << "\nDEBUG::Uh-oh, it looks like the background got unpaired from\n";
//        std::cout << "its generator. Unable to draw background" << std::endl;
//        return;
//    }
//    
//    
//    //Set this texture program as active
//    this->objData->shader->use(); //Turn on the shader for this object
//    
//    //Bind texture
//    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (*this->bg->textures[this->bg->getActiveBackground()]->tex), 0);
//    //Bind uniforms
//    glUniform1f(textEnum, (*this->bg->textures[this->bg->getActiveBackground()]->tex));//I think?
//    glUniform1f(time, this->timeAlive);
//    glUniform1f(zoom, this->w);
//    
//    //Bind Vertices
//    glBufferData(GL_ARRAY_BUFFER, sizeof(this->objData->vertices), this->objData->vertices, GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->objData->elements), this->objData->elements, GL_STATIC_DRAW);
//    
//    
//    //Draw
//    glDrawElements(GL_TRIANGLES, this->objData->numberOfElements, GL_UNSIGNED_INT, 0);
//    
//}

