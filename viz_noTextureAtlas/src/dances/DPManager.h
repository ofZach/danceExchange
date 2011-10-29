#pragma once

#include "ofMain.h"
#include "DanceParticle.h"
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
    
    float                                   globalScale;
    ofxTween                                globalScaleTween;
    ofxTween                                modeChangeTween;
    ofxEasingQuad                           quadEasing;
    ofxEasingLinear                         linearEasing;
    
    
    void init(  );
    void update( int deltaMillis );
    void updateGlobe( int deltaMillis );
    void updateStarfield( int deltaMillis );
    void draw();
    void transitionToGlobeMode( int duration, int delay );
    void transitionToStarfieldMode( int duration, int delay );
    void tweenParticlesToScale( float desiredScale, float duration, float delay = 0 );
    void createParticle( danceVideo *dv, bool shouldZoom );
    void zoomParticle( DanceParticle *dp );
    void particleZoomed( DanceParticle &dp );
    void particleZoomedOut( DanceParticle &dp );
    void tweenEnded( int & theId );
    void modeChanged( int & theId );
    int getNumDancesInCity( string cityName );
    void animateParticlesForCity( string cityName, ofVec3f worldPos );
  
};