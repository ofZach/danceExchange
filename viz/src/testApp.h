#pragma once

#include "ofMain.h"
#include "Pointilist.h"
#include "threadMovieLoader.h"
#include "networkManager.h"
#include "locationManager.h"
#include "DanceParticle.h"
#include "ofFbo.h"
#include "FrustumHelp.h"
#include "ofTexture.h"
#include "Globe.h"
#include "DPManager.h"

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
    
		networkManager NM;
		locationManager LM;
		int currentCityIndex;
	
	
        Globe globe;
        
        int lastMillis;
    
		void setup();
		void update();

		void draw();
        void updateCity();
        void switchMode( VizMode nextMode );

		void keyPressed  (int key);
        void keyReleased (int key);
        void mousePressed(int x, int y, int button);
		
};
