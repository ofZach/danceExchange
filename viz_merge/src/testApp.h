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
#include "brandMode.h"
#include "mapMode.h"
#include "burstMode.h"



class testApp : public ofBaseApp{

public:
        
		VizMode mode;
        ofTrueTypeFont tradeGothic;
    
		bool drawTextures;
        ofEasyCam cam;
        float fov, nearClip, farClip;
        FrustumHelp frustumHelp;
        bool paused;
        
        Pointilist pointilist;
    
        DPManager dpManager;
        DVManager dvManager;
    
		networkManager NM;
		locationManager LM;
		int currentCityIndex;
	
        ofxTween cityTextTween;
        float cityTextX;
        string cityName;
        ofRectangle cityTextRect;
        ofxEasingQuad easingQuad;
        Globe globe;
        
        int lastMillis;
    
		void setup();
		void update();

		void draw();
        void updateCity();
        void globeToRandomCity( int delay );
        void globeLatLonTweenEnded( int & theId );
        void danceVideoLoaded( danceVideo & dv );
        void switchMode( VizMode nextMode );

		void keyPressed  (int key);
        void keyReleased (int key);
        void mousePressed(int x, int y, int button);
		void exit(){
			dvManager.exit();
			// TODO: let's kill all threads?
			// dvManager.exit()
			ofSleepMillis(200);
		}
		ofImage bg;
	
		brandMode BM;
		mapMode MM;
		burstMode BRSTM;
	
	
	
		
};
