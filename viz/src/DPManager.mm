#include "DPManager.h"

void DPManager::init() {
    
//    this->frustumHelp = frustumHelp;
    
    pointilist.init( 1000, true );
    
    // push the textures into the vector first
    for ( int i = 0; i < NUM_TEXTURES; i++ )
        textures.push_back( ofTexture() );
    // this has to be a separate second step
    for ( int i = 0; i < textures.size(); i++ ) {
        ofTexture &tex = textures[i];
        allocateTexture( tex, TEX_WIDTH, TEX_HEIGHT, GL_RGBA8, false );
        // it would be good to fill the just-allocated texture with transparent black
        // sometimes the texture sampler will pick up pixels from the next row down and it might have some ugly junk in it
        pointilist.addTexture( ofToString(i), tex.getTextureData().textureID, FRAMES_PER_ROW, FRAMES_PER_COL );
        pointilist.setTexture( ofToString(i), i );
    }
    
    frameCount = 0;
    loadingParticle = 0;
    paused = false;
    globalScale = 1.0;
    
    ofAddListener( globalScaleTween.end_E, this, &DPManager::tweenEnded );
}

void DPManager::update( int deltaMillis ) {    
    
    if ( globalScaleTween.isRunning() ) {
        globalScale = globalScaleTween.update();
    }
    
    for ( int i=0; i<dpVector.size(); i++ ) {
        DanceParticle *dp = dpVector[i];
        //        if ( i==0 ) cout << dp->alpha << endl;
        if ( dp == loadingParticle && dp->smallVideoLoaded && dp->firstFrame < 0 ) {
            // integrate the frames into the textures
            addFramesToTextures( dp );
            // the frames are now integrated into the texture so this particle can now be a part of the scene
            // this might be where we initialize its position and flip a switch so it can animate, etc.
            dp->pos.set( ofVec3f( ofRandom( -ofGetWidth()/2, ofGetWidth()/2 ),
                                 ofRandom( -ofGetHeight()/2, ofGetHeight()/2 ),
                                 0 ) );
            dp->pos.set( frustumHelp.getRandomPointOnFarPlane() );
            dp->vel.set( 0, 0, -10 );
            //            dp->pos.set( -100, -100, 0 );
            // set loadingParticle to 0 so we can load the next one
            loadingParticle = 0;
        }
        else if ( !loadingParticle && !dp->smallVideoLoaded ) {
            dp->loadSmallVideo();
            loadingParticle = dp;
        }
        else {
            dp->update( deltaMillis, paused );
            if ( dp->firstFrame < 0 )
                continue;
            
            if ( !frustumHelp.isPointInFrustum( dp->pos ) ) {
                
                dp->pos.set( frustumHelp.getRandomPointOnFarPlane() );
                dp->alpha = 0;
            }
            
            pointilist.addPoint( dp->pos.x, dp->pos.y, dp->pos.z, // 3D position
                                200.0 * globalScale, // size
                                //                                1.0, 1.0, 1.0, dp->alpha, // rgba
                                1.0, 1.0, 1.0, dp->alpha, // rgba
                                dp->texIndex, 0, dp->firstFrame + dp->currentFrame // texture unit, rotation (not used in this), cell number
                                );
        }
    }
}

void DPManager::draw() {
    
    pointilist.draw();
}

void DPManager::tweenParticlesToScale( float desiredScale, float duration, float delay ) {
    
    globalScaleTween.setParameters( DPMANAGER_GLOBAL_SCALE, quadEasing, ofxTween::easeInOut, globalScale, desiredScale, duration, delay );
    
}

void DPManager::createParticle( DanceInfo &danceInfo ) {
    
    DanceParticle *dp = new DanceParticle( danceInfo );
    dpVector.push_back( dp );
    dpMap[dp->info.hash] = dp;
    
}

void DPManager::addFramesToTextures( DanceParticle * dp ) {
    // figure out which texture we are drawing into right now
    int texIndex = frameCount / FRAMES_PER_TEX;
    // figure out which frame on that texture we are starting at
    int texFrame = frameCount % FRAMES_PER_TEX;
    // is the number of frames in this animation going to overflow? we don't deal with that
    if ( dp->info.numFrames + texFrame >= FRAMES_PER_TEX ) {
        texIndex++;
        frameCount = texIndex * FRAMES_PER_TEX;
    }
    
    dp->firstFrame = frameCount % FRAMES_PER_TEX;
    dp->texIndex = texIndex;
    
    //    cout << "adding frames into texIndex: " << texIndex << endl;
    
    // if the texIndex is less than our maximum number of textures, we can proceed
    if ( texIndex < NUM_TEXTURES ) {
        ofTexture &activeTexture = textures.at( texIndex );
        for ( int i = 0; i < dp->info.numFrames; i++ ) {
            texFrame = frameCount % FRAMES_PER_TEX;
            
            int col = texFrame % FRAMES_PER_ROW;
            int row = texFrame / FRAMES_PER_ROW;
            
            int theX = col * FRAME_WIDTH;
            int theY = row * FRAME_HEIGHT;
            
            //            GLvoid * pixels = (GLvoid *)(dp->smallVideoLoader->pixels + ( i * 100 * 76 * 3 ));
            unsigned char * pixels = dp->smallVideoLoader->pixels + ( i * 100 * 76 * TH_MOVIE_CHANNELS );
            glEnable( activeTexture.getTextureData().textureTarget );
            glBindTexture( activeTexture.getTextureData().textureTarget, activeTexture.getTextureData().textureID );
            //            glTexSubImage2D( activeTexture.getTextureData().textureTarget, 0, theX, theY, 100, 76, activeTexture.getTextureData().glType, activeTexture.getTextureData().pixelType, pixels );
            glTexSubImage2D( activeTexture.getTextureData().textureTarget, 0, theX, theY, 100, 76, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
            glDisable( activeTexture.getTextureData().textureTarget );
            
            
            frameCount++;
        }
    }
}

void DPManager::tweenEnded( int & theId ) {
    
}