#pragma once

#include "ofMain.h"
#include "DanceInfo.h"
#include "threadMovieLoader.h"
#include "ofxTween.h"

class danceVideo {
public:
    
	DanceInfo info;
    
    bool                smallVideoLoaded;
    threadMovieLoader*  smallVideoLoader;
    int                 smallVideoLoadStartMillis;
    
    int                 texIndex;
    int                 firstFrame;
    int                 numFrames;
    int                 currentFrame;
    int                 frameDelay;
    int                 milliCounter;
    
   
    danceVideo( DanceInfo info ) {
		printf("here \n");
        this->info = info;
        smallVideoLoaded = false;
        smallVideoLoader = 0;
        texIndex = -1;
        firstFrame = -1;
        numFrames = info.numFrames;
        currentFrame = 0;
        frameDelay = 50;
        milliCounter = 0;
    }
    
    bool needsSmallVideo() {
        return !smallVideoLoaded && !smallVideoLoader;
    }
    
    void loadSmallVideo() {
        smallVideoLoader = new threadMovieLoader( info.numFrames, 100, 76 );
        string filename = "videos/" + info.hash + "_s.mov";
        smallVideoLoadStartMillis = ofGetElapsedTimeMillis();
        smallVideoLoader->start( filename );
    }
    
  
    
    void update( int deltaMillis, bool paused ) {
        
        if ( !smallVideoLoaded && smallVideoLoader && smallVideoLoader->state == TH_STATE_JUST_LOADED ) {
            int now = ofGetElapsedTimeMillis();
            int diff = now - smallVideoLoadStartMillis;
			smallVideoLoaded = true;
        }
        
        milliCounter += deltaMillis;
        if ( milliCounter >= frameDelay ) {
            milliCounter -= frameDelay;
            if ( ++currentFrame == numFrames )
                currentFrame = 0;
        }
        
	}
    
};