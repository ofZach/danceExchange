#include "DVManager.h"



void DVManager::createDanceVideo( DanceInfo danceInfo ) {
    
	printf("creating %i \n", danceInfo.id);
	
    //danceVideo *dv = new danceVideo( danceInfo );
    unloadedDanceVideos.push_back( danceInfo );
}

void DVManager::init( Pointilist *pointilist ) {
    // set the pointilist pointer
    this->pointilist = pointilist;
    
    // push the textures into the vector first
    for ( int i = 0; i < NUM_TEXTURES; i++ )
        textures.push_back( ofTexture() );
    // this has to be a separate second step
    for ( int i = 0; i < textures.size(); i++ ) {
        ofTexture &tex = textures[i];
        allocateTexture( tex, TEX_WIDTH, TEX_HEIGHT, GL_RGBA8, false );
        // it would be good to fill the just-allocated texture with transparent black
        // sometimes the texture sampler will pick up pixels from the next row down and it might have some ugly junk in it
        pointilist->addTexture( ofToString(i), tex.getTextureData().textureID, FRAMES_PER_ROW, FRAMES_PER_COL );
        pointilist->setTexture( ofToString(i), i );
    }
    
    frameCount = 0;
    loadingVideo = 0;
    paused = false;
	
	pix.allocate(FRAME_WIDTH, FRAME_HEIGHT, OF_PIXELS_RGBA);
	frame.allocate(FRAME_WIDTH, FRAME_HEIGHT, GL_RGBA8);
	offscreen.allocate(FRAME_WIDTH, FRAME_HEIGHT,  GL_RGBA8, 4);
	
	for (int i = 0; i < SIMULTANEOUS_LOADS; i++){
		loaders[i] = new threadMovieLoader( 24, FRAME_WIDTH, FRAME_HEIGHT );
	}
	
	
	
}

void DVManager::update( int deltaMillis ) {
    
	for (int i = 0; i < SIMULTANEOUS_LOADS; i++){
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
	}
	
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
					printf("loading %s %s \n", filename.c_str(), fileId.c_str());
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
    if ( texIndex < NUM_TEXTURES ) {
        ofTexture &activeTexture = textures.at( texIndex );
        for ( int i = 0; i < dv->info.numFrames; i++ ) {
            texFrame = frameCount % FRAMES_PER_TEX;
            
            int col = texFrame % FRAMES_PER_ROW;
            int row = texFrame / FRAMES_PER_ROW;
            
            int theX = col * FRAME_WIDTH;
            int theY = row * FRAME_HEIGHT;
            
            //            GLvoid * pixels = (GLvoid *)(dp->smallVideoLoader->pixels + ( i * 100 * 76 * 3 ));
            unsigned char * pixels = TL->pixels + ( i * 100 * 76 * TH_MOVIE_CHANNELS );
			frame.loadData(pixels, 100,76, GL_RGBA);
			
			//offscreen.setFromPixels(pixels, FRAME_WIDTH, FRAME_HEIGHT, 100, 76);
			offscreen.begin();
			ofClear(0,0,0,0);
			ofSetColor(255,255,255);
			frame.draw(0,0);
            ofPushMatrix();
            ofScale( 3, 3 );
            ofSetColor( 255, 255, 255 );
            //ofDrawBitmapString( ofToString( danceVideos.size(), 3 ), 5, 12 );
            ofPopMatrix();
			offscreen.end();
			offscreen.readToPixels(pix);
			
            glEnable( activeTexture.getTextureData().textureTarget );
            glBindTexture( activeTexture.getTextureData().textureTarget, activeTexture.getTextureData().textureID );
            //            glTexSubImage2D( activeTexture.getTextureData().textureTarget, 0, theX, theY, 100, 76, activeTexture.getTextureData().glType, activeTexture.getTextureData().pixelType, pixels );
            glTexSubImage2D( activeTexture.getTextureData().textureTarget, 0, theX, theY, 100, 76, GL_RGBA, GL_UNSIGNED_BYTE, pix.getPixels() );
            glDisable( activeTexture.getTextureData().textureTarget );
            
            
            frameCount++;
        }
    }
}