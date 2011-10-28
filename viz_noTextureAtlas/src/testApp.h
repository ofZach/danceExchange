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
#include "DVManager.h"
#include "VizModes.h"


class testApp : public ofBaseApp{

public:
        
		DVManager dvManager;
    
		networkManager NM;
		    
		void setup();
		void update();

		void draw();
       
		void danceVideoLoaded( danceVideo & dv );
        
		void keyPressed  (int key);
        void keyReleased (int key);
        void mousePressed(int x, int y, int button);
		
};
