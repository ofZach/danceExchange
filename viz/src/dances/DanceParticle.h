#pragma once

#include "ofMain.h"
#include "DanceInfo.h"
#include "threadMovieLoader.h"

class DanceParticle {
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
    
    ofVec3f             pos;
    ofVec3f             vel;
    float               alpha;
    
    DanceParticle( DanceInfo info ) {
        this->info = info;
        smallVideoLoaded = false;
        smallVideoLoader = 0;
        texIndex = -1;
        firstFrame = -1;
        numFrames = info.numFrames;
        
        currentFrame = 0;
        frameDelay = 50;
        milliCounter = 0;
        alpha = 0;
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
//            cout << "small video loaded in " << diff / 1000.0 << " seconds" << endl;
            smallVideoLoaded = true;
        }
        
        milliCounter += deltaMillis;
        if ( milliCounter >= frameDelay ) {
            milliCounter -= frameDelay;
            if ( ++currentFrame == numFrames )
                currentFrame = 0;
        }
        
        if ( !paused && firstFrame > 0 ) {
            if ( alpha < 1.0 ) {
                alpha = fminf( 1.0, alpha + .01 );
            }
            pos += vel;
        }
    }
    
};