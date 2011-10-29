#include "DVManager.h"

const int FRAME_WIDTH = 100;
const int FRAME_HEIGHT = 76;
const int TEX_WIDTH = 40 * FRAME_WIDTH;//2048;
const int TEX_HEIGHT = 53 * FRAME_HEIGHT;//2048;
const int FRAMES_PER_ROW = TEX_WIDTH / FRAME_WIDTH;
const int FRAMES_PER_COL = TEX_HEIGHT / FRAME_HEIGHT;
const int FRAMES_PER_TEX = FRAMES_PER_COL * FRAMES_PER_ROW;
const int NUM_TEXTURES = 5;

void DVManager::createDanceVideo( DanceInfo & danceInfo ) {
    
    danceVideo *dv = new danceVideo( danceInfo );
    unloadedDanceVideos.push_back( dv );
    
}

void DVManager::init(  ) {
    
    frameCount = 0;
    loadingVideo = 0;
    paused = false;
	
	pix.allocate(FRAME_WIDTH, FRAME_HEIGHT, OF_PIXELS_RGBA);
	frame.allocate(FRAME_WIDTH, FRAME_HEIGHT, GL_RGBA8);
	offscreen.allocate(FRAME_WIDTH, FRAME_HEIGHT,  GL_RGBA8, 4);
	//offscreen
}

void DVManager::update( int deltaMillis ) {
    
    // update the loading video
    if ( loadingVideo )
        loadingVideo->update( deltaMillis, paused );
    
    // if there is a loading video, see if it has finished loading yet
    if ( loadingVideo && loadingVideo->smallVideoLoaded ) {
        // this is where we'll integrate it into the texture stuff
        addFramesToTextures( loadingVideo );
        // now put it in the main dance video vector
        danceVideos.push_back( loadingVideo );
        // dispatch an event
        ofNotifyEvent( danceVideoLoadedEvent, *loadingVideo, this );
        // and set loading video to zero
        loadingVideo = 0;
    }
    
    // if there is no currently loading video and there are unloaded videos present
    // start the next one in line loading
    if ( !loadingVideo && unloadedDanceVideos.size() > 0 ) {
        // set loading video to the first dv in the unloaded videos vector
        loadingVideo = *(unloadedDanceVideos.begin());
        // now erase it from the vector
        unloadedDanceVideos.erase( unloadedDanceVideos.begin() );
        // now start it loading
        loadingVideo->loadSmallVideo();
    }


	for (int i = 0; i < danceVideos.size(); i++){
		danceVideos[i]->update(	deltaMillis, false);
	}
    
}

void DVManager::loadLargeVideo( string hash ) {
    
}

void DVManager::addFramesToTextures( danceVideo * dv ) {
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
            unsigned char * pixels = dv->smallVideoLoader->pixels + ( i * 100 * 76 * TH_MOVIE_CHANNELS );
			frame.loadData(pixels, 100,76, GL_RGBA);
			
			glDisable(GL_DEPTH_TEST);
			//offscreen.setFromPixels(pixels, FRAME_WIDTH, FRAME_HEIGHT, 100, 76);
			offscreen.begin();
			ofClear(0,0,0,0);
			ofSetColor(255,255,255);
			frame.draw(0,0);
            ofSetColor( 255, 255, 255 );
			//ofLine(0,0,100,76);
            ofDrawBitmapString( ofToString( i ), 5, 12 );
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