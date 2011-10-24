#pragma once

#include "ofMain.h"
#include "DanceInfo.h"
#include "threadMovieLoader.h"
#include "ofxTween.h"

class danceVideo {
public:
    
	DanceInfo info;
    
    // dance info stuff
    int                 id; // database id
    string              hash; // database hash value
    string              city; // city name
    string              url; // some url... i forget!
    string              creationTime; // the time it was created
    int                 numFrames; // number of frames in the loop
    
    
    bool                smallVideoLoaded;
    threadMovieLoader*  smallVideoLoader;
    int                 smallVideoLoadStartMillis;
    
    int                 texIndex;
    int                 firstFrame;
    int                 currentFrame;
    int                 frameDelay;
    int                 milliCounter;
    
   
    danceVideo( DanceInfo info ) {
//		printf("here \n");
        this->info = info;
        
        this->id              = info.id;
        this->hash            = info.hash;
        this->city            = info.city;
        this->url             = info.url;
        this->creationTime    = info.creationTime;
        this->numFrames       = info.numFrames;
        
        
        smallVideoLoaded = false;
        smallVideoLoader = 0;
        texIndex = -1;
        firstFrame = -1;
        currentFrame = 0;
        frameDelay = 100;
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
        else {
            milliCounter += deltaMillis;
            if ( milliCounter >= frameDelay ) {
                milliCounter -= frameDelay;
                if ( ++currentFrame == numFrames )
                    currentFrame = 0;
            }
        }
        
	}
    
};