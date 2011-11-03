#pragma once

#include "ofMain.h"
#include "DanceInfo.h"
#include "danceVideo.h"

#include "threadMovieLoader.h"


const int FRAME_WIDTH = 100;
const int FRAME_HEIGHT = 76;
const int TEX_WIDTH = 40 * FRAME_WIDTH;//2048;
const int TEX_HEIGHT = 53 * FRAME_HEIGHT;//2048;
const int FRAMES_PER_ROW = TEX_WIDTH / FRAME_WIDTH;
const int FRAMES_PER_COL = TEX_HEIGHT / FRAME_HEIGHT;
const int FRAMES_PER_TEX = FRAMES_PER_COL * FRAMES_PER_ROW;
const int NUM_TEXTURES = 5;
const int SIMULTANEOUS_LOADS = 5;


class DVManager {
public:
    
    
	ofFbo						offscreen;
	ofTexture					frame;
	ofPixels					pix;
	
	threadMovieLoader*			loaders[SIMULTANEOUS_LOADS];	
	DanceInfo					loadersInfo[SIMULTANEOUS_LOADS];	// dance info for this loader. 
	
	vector<danceVideo*>         danceVideos;
	vector <DanceInfo>			unloadedDanceVideos;
	
	
	
    vector<ofTexture>           textures;
    int                         frameCount;
    bool                        paused;
    
    ofEvent<danceVideo>        danceVideoLoadedEvent;
    
    void init(  );
    
    void update( int deltaMillis );
    void createDanceVideo( DanceInfo danceInfo );
    void loadLargeVideo( string hash );
    void addFramesToTextures( danceVideo * dv, threadMovieLoader * TL );

	
	
};