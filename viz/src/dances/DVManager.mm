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
	//offscreen
}

void DVManager::update( int deltaMillis ) {
    
    // update the loading video
    if ( loadingVideo )
        loadingVideo->update( deltaMillis, paused );
    
//    // update the loaded videos
//    for ( vector<danceVideo*>::iterator it = danceVideos.begin(); it != danceVideos.end(); it++ ) {
//        (*it)->update( deltaMillis, paused );
//    }
    
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
    if ( texIndex < NUM_TEXTURES ) {
        ofTexture &activeTexture = textures.at( texIndex );
        for ( int i = 0; i < dv->info.numFrames; i++ ) {
            texFrame = frameCount % FRAMES_PER_TEX;
            
            int col = texFrame % FRAMES_PER_ROW;
            int row = texFrame / FRAMES_PER_ROW;
            
            int theX = col * FRAME_WIDTH;
            int theY = row * FRAME_HEIGHT;
            
            //            GLvoid * pixels = (GLvoid *)(dp->smallVideoLoader->pixels + ( i * 100 * 76 * 3 ));
            unsigned char * pixels = dv->smallVideoLoader->pixels + ( i * 100 * 76 * TH_MOVIE_CHANNELS );
			frame.loadData(pixels, 100,76, GL_RGBA);
			
			//offscreen.setFromPixels(pixels, FRAME_WIDTH, FRAME_HEIGHT, 100, 76);
			offscreen.begin();
			ofClear(0,0,0,0);
			ofSetColor(255,255,255);
			frame.draw(0,0);
            ofPushMatrix();
            ofScale( 3, 3 );
            ofSetColor( 255, 255, 255 );
            ofDrawBitmapString( ofToString( danceVideos.size(), 3 ), 5, 12 );
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