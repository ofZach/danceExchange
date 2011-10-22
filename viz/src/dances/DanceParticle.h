#pragma once

#include "ofMain.h"
#include "DanceInfo.h"
#include "threadMovieLoader.h"
#include "ofxTween.h"
#include "danceVideo.h"


class DanceParticle {
public:
    
	danceVideo * DV;
    
    ofVec3f             startPos;
    ofVec3f             targetPos;
    ofVec3f             pos;
    ofxTween            posTween;
    ofxEasingQuad       quadEasing;
    ofVec3f             vel;
    float               alpha;
    
    DanceParticle( DanceInfo info ) {
		DV = new danceVideo (info);
	}
    
   
    void startPosTween( int delay ) {
        posTween.setParameters( 0, quadEasing, ofxTween::easeInOut, 0, 1, 500, delay );
    }
    
    void updateStarfield( int deltaMillis, bool paused ) {
        
    }
    
    void updateGlobe( int deltaMillis, bool paused ) {
        if ( posTween.isRunning() ) {
            posTween.update();
            pos.set( startPos.getInterpolated( targetPos, posTween.getTarget( 0 ) ) );
        }
    }
    
    void update( int deltaMillis, bool paused ) {
		DV->update(deltaMillis, paused);
		
		if ( !paused && DV->firstFrame > 0 ) {
			if ( alpha < 1.0 ) {
				alpha = fminf( 1.0, alpha + .01 );
			}
			pos += vel;
		}
		
	}
    
};