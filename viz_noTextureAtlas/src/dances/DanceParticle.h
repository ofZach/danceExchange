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
    ofxTween            zoomTween;
    ofxTween            zoomOutTween;
    ofxEasingQuad       quadEasing;
    ofVec3f             vel;
    float               alpha;
    float               r,g,b;
    
    ofEvent<DanceParticle> zoomFinished;
    ofEvent<DanceParticle> zoomOutFinished;
    
    DanceParticle( danceVideo *dv ) {
        this->DV = dv;
        r = ofRandom( 0.0, 1.0 );
        g = ofRandom( 0.0, 1.0 );
        b = ofRandom( 0.0, 1.0 );
    }
    
   
    void startPosTween( int delay ) {
        posTween.setParameters( 0, quadEasing, ofxTween::easeInOut, 0, 1, 500, delay );
    }
    
    void zoomTweenFinished( int & theId ) {
        ofRemoveListener( zoomTween.end_E, this, &DanceParticle::zoomTweenFinished );
        this->vel.set( 0, 0, 0 );
        ofNotifyEvent( zoomFinished, *this );
    }
    
    void zoomOutTweenFinished( int & theId ) {
        ofRemoveListener( zoomOutTween.end_E, this, &DanceParticle::zoomOutTweenFinished );
        ofNotifyEvent( zoomOutFinished, *this );
    }
    
    void startZoomTween() {
        ofAddListener( zoomTween.end_E, this, &DanceParticle::zoomTweenFinished );
        zoomTween.setParameters( quadEasing, ofxTween::easeInOut, 0, 1, 300, 0 );
    }
    
    void startZoomOutTween( int delay ) {
        ofAddListener( zoomOutTween.end_E, this, &DanceParticle::zoomOutTweenFinished );
        zoomOutTween.setParameters( quadEasing, ofxTween::easeInOut, 0, 1, 300, delay );
    }
    
    void updateStarfield( int deltaMillis, bool paused ) {
        
        if ( zoomTween.isRunning() ) {
            zoomTween.update();
            alpha = zoomTween.getTarget( 0 );
            pos.set( startPos.getInterpolated( targetPos, zoomTween.getTarget( 0 ) ) );
        }
        else if ( zoomOutTween.isRunning() ) {
            zoomOutTween.update();
            alpha = 1.0 - zoomOutTween.getTarget( 0 );
            pos.set( startPos.getInterpolated( targetPos, zoomOutTween.getTarget( 0 ) ) );
        }
        else {
        
            if ( alpha < 1.0 ) {
                alpha = fminf( 1.0, alpha + .01 );
            }
            pos += vel;
        }
		
		DV->update(deltaMillis, paused);
    }
    
    void updateGlobe( int deltaMillis, bool paused ) {
        if ( posTween.isRunning() ) {
            posTween.update();
            pos.set( startPos.getInterpolated( targetPos, posTween.getTarget( 0 ) ) );
        }
		
		DV->update(deltaMillis, paused);
    }
    
};