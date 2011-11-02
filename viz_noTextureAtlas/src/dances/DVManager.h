#pragma once

#include "ofMain.h"
#include "DanceInfo.h"
#include "danceVideo.h"

class DVManager {
public:
    
    
	ofFbo						offscreen;
	ofTexture					frame;
	ofPixels					pix;
	
	
	
    vector<danceVideo*>         unloadedDanceVideos;
    vector<danceVideo*>         danceVideos;
    vector<danceVideo*>         loadingVideos;
    vector<ofTexture>           textures;
    danceVideo                  *loadingVideo;
    int                         frameCount;
    bool                        paused;
    
    ofEvent<danceVideo>        danceVideoLoadedEvent;
    
    void init(  );
    
    void update( int deltaMillis );
    void createDanceVideo( DanceInfo & danceInfo );
    void loadLargeVideo( string hash );
    void addFramesToTextures( danceVideo * dv );

};