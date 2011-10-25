#pragma once

#include "ofMain.h"
#include "DanceParticle.h"
#include "Pointilist.h"
#include "FrustumHelp.h"
#include "ofxTween.h"
#include "VizModes.h"

enum DPManagerTween {
    DPMANAGER_GLOBAL_SCALE
};

class DPManager {
public:
    
    VizMode                                 mode, nextMode;
    
    FrustumHelp                             frustumHelp;
    vector<DanceParticle*>                  dpVector;
    map<string,DanceParticle*>              dpMap;
    bool                                    paused;
    
    vector<DanceParticle*>                  cityParticles;
    
    Pointilist                              *pointilist;
    float                                   globalScale;
    ofxTween                                globalScaleTween;
    ofxTween                                modeChangeTween;
    ofxEasingQuad                           quadEasing;
    ofxEasingLinear                         linearEasing;
    
    
    void init( Pointilist *pointilist );
    void update( int deltaMillis );
    void updateGlobe( int deltaMillis );
    void updateStarfield( int deltaMillis );
    void draw();
    void transitionToGlobeMode( int duration, int delay );
    void transitionToStarfieldMode( int duration, int delay );
    void tweenParticlesToScale( float desiredScale, float duration, float delay = 0 );
    void createParticle( danceVideo *dv, bool shouldZoom );
    void tweenEnded( int & theId );
    void modeChanged( int & theId );
    void animateParticlesForCity( string cityName, ofVec3f worldPos );
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