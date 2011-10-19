#pragma once

#include "ofMain.h"
#include "Pointilist.h"
#include "threadMovieLoader.h"
#include "DBHelper.h"
#include "DanceParticle.h"
#include "ofFbo.h"
#include "FrustumHelp.h"
#include "ofTexture.h"
#include "Globe.h"
#include "DPManager.h"

//const int FRAME_WIDTH = 100;
//const int FRAME_HEIGHT = 76;
//const int TEX_WIDTH = 40 * FRAME_WIDTH;//2048;
//const int TEX_HEIGHT = 53 * FRAME_HEIGHT;//2048;
//const int FRAMES_PER_ROW = TEX_WIDTH / FRAME_WIDTH;
//const int FRAMES_PER_COL = TEX_HEIGHT / FRAME_HEIGHT;
//const int FRAMES_PER_TEX = FRAMES_PER_COL * FRAMES_PER_ROW;
//const int NUM_TEXTURES = 5;

enum VizMode {
    STARFIELD_MODE,
    GLOBE_MODE
};

class testApp : public ofBaseApp{

public:
        VizMode mode;
    
        bool upKey, downKey, leftKey, rightKey;
    
        bool drawTextures;
        ofEasyCam cam;
        float fov, nearClip, farClip;
        FrustumHelp frustumHelp;
        bool paused;
        
        DPManager dpManager;
    
        DBHelper *dbHelper;
        bool isRequestingRecentDances;
        
        Globe globe;
        vector<string> cities;
        map<string,ofVec2f> cityLatLonHash;
        int currentCityIndex;
    
        vector<DanceParticle*> dpVector;
        map<string,DanceParticle*> dpMap;
        DanceParticle *loadingParticle;
    
        vector<ofTexture> textures;
        int frameCount;
    
    
        int lastMillis;
        
        Pointilist pointilist;
        
        int numPoints;
    
		void setup();
		void update();
        void updateParticles( int deltaMillis );
        void updateNetworkIO();
		void draw();
        void addFramesToTextures( DanceParticle * dp );
        void updateCity();

		void keyPressed  (int key);
        void keyReleased (int key);
        void mousePressed(int x, int y, int button);
    
        // added this so i could allocate a non-power of two texture without modifying the core of OF
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
