#include "DVManager.h"



void DVManager::createDanceVideo( DanceInfo danceInfo ) {
    cout << "create! " << endl;
    //danceVideo *dv = new danceVideo( danceInfo );
    unloadedDanceVideos.push_back( danceInfo );
}

void DVManager::init(  ) {
    
    frameCount = 0;
    paused = false;
	
	pix.allocate(FRAME_WIDTH, FRAME_HEIGHT, OF_PIXELS_RGBA);
	frame.allocate(FRAME_WIDTH, FRAME_HEIGHT, GL_RGBA8);
	offscreen.allocate(FRAME_WIDTH, FRAME_HEIGHT,  GL_RGBA8, 4);
	
	
	for (int i = 0; i < SIMULTANEOUS_LOADS; i++){
		 loaders[i] = new threadMovieLoader( 24, FRAME_WIDTH, FRAME_HEIGHT );
	}
		
	//offscreen
}

void DVManager::update( int deltaMillis ) {
    
	/*for (int i = 0; i < SIMULTANEOUS_LOADS; i++){
		printf("threadedLoaders %i :", i);
		switch (loaders[i]->state){
			case TH_STATE_LOADING:
				printf(" -- loading \n");
				break;
			case TH_STATE_JUST_LOADED:
				printf(" -- just loading \n");
				break;
			case TH_STATE_LOADED: 
				printf(" -- loaded \n");
				break;
			case TH_STATE_UNLOADED:
				printf(" -- unloaded \n");
				break;
		}
	}*/
	
	int howManyAvailableForLoading = 0;
	for (int i = 0; i < SIMULTANEOUS_LOADS; i++){
		if (loaders[i]->state == TH_STATE_UNLOADED){
			howManyAvailableForLoading++;
		}
	}
		 
	//cout << howManyAvailableForLoading << endl;
	
	for ( int i=0; i<danceVideos.size(); i++ ) {
        danceVideos[i]->update( deltaMillis, paused );
    }
	
	
	
    for (int i = 0; i < SIMULTANEOUS_LOADS; i++){
		if (loaders[i]->state == TH_STATE_LOADED || 
			loaders[i]->state == TH_STATE_JUST_LOADED ){
			
			// cool !! let's do this. 
			danceVideo * dv;
			dv = new danceVideo(loadersInfo[i]);
			//dv.info = loadersInfo[i];
			
			addFramesToTextures(dv, loaders[i]);
			loaders[i]->unloadMovie();
			danceVideos.push_back(dv);
			ofNotifyEvent( danceVideoLoadedEvent, *dv, this );
            
		}
	}
	
	
	//cout << unloadedDanceVideos.size() << " --- unloadedDanceVideos " << endl;
	// this was originally a while loop, but I think it's ok to do this once per frame. 
	if ( howManyAvailableForLoading > 0 && unloadedDanceVideos.size() > 0 ) {
        
		
		DanceInfo toLoad = unloadedDanceVideos[0];
		
		//may number of frames be different?  please do something here if so. 
		
		bool bLoadingOne = false;
		
		string fileId = ofToString( toLoad.id );
		string filename = "videos/" + fileId + "_s.mov";
		for (int i = 0; i < SIMULTANEOUS_LOADS; i++){
			if (loaders[i]->state == TH_STATE_UNLOADED){
				if (loaders[i]->start(filename, fileId)){		// in case we don't "start" let's not erase. 
					loadersInfo[i] = toLoad;
					bLoadingOne = true;
					unloadedDanceVideos.erase( unloadedDanceVideos.begin() );
					break;
				}
			}
		}
		
	}
	
	
	
}

void DVManager::loadLargeVideo( string hash ) {
    
}

void DVManager::addFramesToTextures( danceVideo * dv, threadMovieLoader * TL) {
    // figure out which texture we are drawing into right now
    int texIndex = frameCount / FRAMES_PER_TEX;
    // figure out which frame on that texture we are starting at
    int texFrame = frameCount % FRAMES_PER_TEX;
    // is the number of frames in this animation going to overflow? we don't deal with that
    if ( dv->info.numFrames + texFrame >= FRAMES_PER_TEX ) {
        texIndex++;
        frameCount = texIndex * FRAMES_PER_TEX;
    }
    
    dv->firstFrame = frameCount % FRAMES_PER_TEX;
    dv->texIndex = texIndex;
    
    //    cout << "adding frames into texIndex: " << texIndex << endl;
    
    // if the texIndex is less than our maximum number of textures, we can proceed
    if ( true ) {
       // ofTexture &activeTexture = textures.at( texIndex );
        for ( int i = 0; i < dv->info.numFrames; i++ ) {
            texFrame = frameCount % FRAMES_PER_TEX;
            
            int col = texFrame % FRAMES_PER_ROW;
            int row = texFrame / FRAMES_PER_ROW;
            
            int theX = col * FRAME_WIDTH;
            int theY = row * FRAME_HEIGHT;
            
            //            GLvoid * pixels = (GLvoid *)(dp->smallVideoLoader->pixels + ( i * 100 * 76 * 3 ));
            unsigned char * pixels = TL->pixels + ( i * 100 * 76 * TH_MOVIE_CHANNELS );
			frame.loadData(pixels, 100,76, GL_RGBA);
			
			glDisable(GL_DEPTH_TEST);
			//offscreen.setFromPixels(pixels, FRAME_WIDTH, FRAME_HEIGHT, 100, 76);
			offscreen.begin();
			ofClear(0,0,0,0);
			ofSetColor(255,255,255);
			frame.draw(0,0);
            ofSetColor( 255, 255, 255 );
			//ofLine(0,0,100,76);
           // ofDrawBitmapString( ofToString( i ), 5, 12 );
			offscreen.end();
			offscreen.readToPixels(pix);
			
			ofTexture newTex;
			newTex.allocate(100,76, GL_RGBA);
			newTex.loadData(pix.getPixels(), 100,76, GL_RGBA);
			dv->textures.push_back(newTex);
            
            frameCount++;
        }
    }
}