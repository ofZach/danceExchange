#pragma once

#include "ofMain.h"
#include "Pointilist.h"
#include "DanceInfo.h"
#include "danceVideo.h"

class DVManager {
public:
    
    Pointilist                  *pointilist;
    
	ofFbo						offscreen;
	ofTexture					frame;
	ofPixels					pix;
	
	
	
    vector<danceVideo*>         unloadedDanceVideos;
    vector<danceVideo*>         danceVideos;
    vector<danceVideo*>         loadingVideos;
    vector<ofTexture>           textures;
    danceVideo                  *loadingVideo;
    int                         frameCount;
    bool                        paused;
    
    ofEvent<danceVideo>        danceVideoLoadedEvent;
    
    
    void init( Pointilist * pointilist );
    void update( int deltaMillis );
    void createDanceVideo( DanceInfo & danceInfo );
    void loadLargeVideo( string hash );
    void addFramesToTextures( danceVideo * dv );
    void allocateTexture(ofTexture &texture, int w, int h, int internalGlDataType, bool bUseARBExtention) {
        
        
        ofTextureData &texData = texture.texData;
        texData.tex_w = w;
        texData.tex_h = h;
        texData.tex_t = 1.0f;
        texData.tex_u = 1.0f;
        texData.textureTarget = GL_TEXTURE_2D;
        texData.glTypeInternal = internalGlDataType;
        // get the glType (format) and pixelType (type) corresponding to the glTypeInteral (internalFormat)
        ofGetGlFormatAndType(texData.glTypeInternal, texData.glType, texData.pixelType);
        // attempt to free the previous bound texture, if we can:
        texture.clear();
        
        
        glGenTextures(1, (GLuint *)&texData.textureID);   // could be more then one, but for now, just one
        //        retain(texData.textureID);
        
        glEnable(texData.textureTarget);
        glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
        glTexImage2D(texData.textureTarget, 0, texData.glTypeInternal, (GLint)texData.tex_w, (GLint)texData.tex_h, 0, texData.glType, texData.pixelType, 0);  // init to black...
        
        
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
        glDisable(texData.textureTarget);
        
        texData.width = w;
        texData.height = h;
        texData.bFlipTexture = false;
        texData.bAllocated = true;
    }
};