#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    drawTextures = false;
    paused = false;
    
    fov = 60.0;
    nearClip = 1.0;
    farClip = 5000.0;
    
    cam.setFov( fov );
    frustumHelp.fov = fov;
    
    cam.setNearClip( nearClip );
    frustumHelp.nearClipVal = nearClip;
    
    cam.setFarClip( farClip );
    frustumHelp.farClipVal = farClip;
    frustumHelp.aspectRatio = ofGetWidth()/(float)ofGetHeight();
    
    cam.cacheMatrices();
//    cam.disableMouseInput();
    
    globe.init( 300 );
    
	ofEnableSmoothing();
    ofDisableArbTex();
    ofSetVerticalSync( true );
    ofSetFrameRate( 60.0 );
	ofBackground(0, 0, 0);
	
	numPoints = 10000;
	pointilist.init(numPoints, true);
    
    
    for ( int i = 0; i < NUM_TEXTURES; i++ ) {
        textures.push_back( ofTexture() );
    }
    
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
    
    // parse cities csv file
    NSURL *citiesPath = [NSURL URLWithString:@"../data/cities.txt" relativeToURL:[[NSBundle mainBundle] bundleURL]];
    NSError *error;
    NSString *citiesRaw = [NSString stringWithContentsOfURL:citiesPath encoding:NSUTF8StringEncoding error:&error];
    NSArray *cityLines = [citiesRaw componentsSeparatedByString:@"\n"];
    for ( NSString *city in cityLines ) {
        
        NSArray *cityInfo = [city componentsSeparatedByString:@","];
        string cityName = [[cityInfo objectAtIndex:0] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        ofVec2f latLon( [[cityInfo objectAtIndex:1] floatValue], [[cityInfo objectAtIndex:2] floatValue] );
        
        cities.push_back( cityName );
        cityLatLonHash[ cityName ] = latLon;
    }
    currentCityIndex = 8;
    updateCity();
    
    dbHelper = [[DBHelper alloc] init];
    [dbHelper setRequestInterval:5.0];
    [dbHelper requestRecentDances:100];
    isRequestingRecentDances = true;
}

void testApp::update(){
    
    updateNetworkIO();
    
	int currentMillis = ofGetElapsedTimeMillis();
	int deltaMillis = currentMillis - lastMillis;
	lastMillis = currentMillis;
    
    frustumHelp.calcNearAndFarClipCoordinates( cam );
    
    updateParticles( deltaMillis );
}

void testApp::updateParticles( int deltaMillis ) {
    
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
                                100.0, // size
//                                1.0, 1.0, 1.0, dp->alpha, // rgba
                                1.0, 1.0, 1.0, dp->alpha, // rgba
                                dp->texIndex, 0, dp->firstFrame + dp->currentFrame // texture unit, rotation (not used in this), cell number
                                );
        }
    }
}

void testApp::updateNetworkIO() {
    
    vector<DanceInfo> danceInfos = dbHelper.danceInfos;
    if ( danceInfos.size() > 0 ) {
        for ( int i=0; i<danceInfos.size(); i++ ) {
            
            DanceParticle *dp = new DanceParticle( danceInfos[i] );
            dpVector.push_back( dp );
            dpMap[dp->info.hash] = dp;
//            cout << dp->info.numFrames << " frames " << "dp count: " << dpVector.size() << endl;
        }
        [dbHelper clearDanceInfos];
//        cout << "local dance info count: " << danceInfos.size() << endl;
//        cout << "db helper dance info count: " << dbHelper.danceInfos.size() << endl;
    }
    
    if ( isRequestingRecentDances && ![dbHelper isRequestingRecentDanceInfos] && ![dbHelper isProcessingDanceInfosWithoutVideos] ) {
        cout << "initial request finished..." << endl;
        isRequestingRecentDances = false;
        [dbHelper requestDancesSince];
    }
    
    
}

void testApp::addFramesToTextures( DanceParticle * dp ) {
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

void testApp::draw(){  
    glEnable( GL_DEPTH_TEST );
    ofEnableAlphaBlending();  
    
    float camZ = cam.getPosition().z;
    if ( camZ > 0 )
        cam.setPosition( 0, 0, -fabs(camZ) );
    cam.lookAt( ofVec3f(), ofVec3f( 0, -1, 0 ) );
    
    cam.begin();
    
//    if ( !  paused ) globe.yRotation+=.5;
//    globe.draw();
//    ofVec3f sf = globe.getWorldCoordForLatLon( cityLatLonHash[cities[currentCityIndex]] );
//    ofSphere( sf.x, sf.y, sf.z, 10 );
    
    glPushMatrix();
	pointilist.draw();
    glPopMatrix();
    cam.end();
    
    
    ofSetupScreen();
    glDisable( GL_DEPTH_TEST );
    
	ofDrawBitmapString( "fps: "+ofToString(ofGetFrameRate(),2) + "\nnum particles: " + ofToString(dpVector.size(), 2), 10, 15 );
    ofDrawBitmapString( cities[currentCityIndex], 10, 45 );
    
    if ( drawTextures ) {
        for ( int i = 0; i < textures.size(); i++ ) {
            ofTexture &tex = textures[i];
            tex.draw( i * 200, ofGetHeight() - 200, 200, 200 );
        }
    }
}

void testApp::updateCity() {
    // in the future, this function would setup whatever animated transitions take place when
    // switching the city of focus in globe mode
    
    ofVec2f latLon = cityLatLonHash[ cities[ currentCityIndex ] ];
    globe.setLatLon( latLon );
    
}

void testApp::keyPressed(int key){
    
    if ( key == OF_KEY_DOWN ) {
        if ( ++currentCityIndex >= cities.size() )
            currentCityIndex = 0;
        updateCity();
    }
    else if ( key == OF_KEY_UP ) {
        if ( --currentCityIndex < 0 )
            currentCityIndex = cities.size() - 1;
        updateCity();
    }
    
    if ( key == ' ' ) {
        drawTextures = !drawTextures;
//        [dbHelper requestRecentDances:1000];
//        isRequestingRecentDances = true;
    }
    if ( key == 'f' ) {
        
        frustumHelp.calcNearAndFarClipCoordinates( cam );
        cout << "far clip tl " << frustumHelp.farClip[FrustumHelp::FTL] << endl;
        cout << "far clip tr " << frustumHelp.farClip[FrustumHelp::FTR] << endl;
        cout << "far clip bl " << frustumHelp.farClip[FrustumHelp::FBL] << endl;
        cout << "far clip br " << frustumHelp.farClip[FrustumHelp::FBR] << endl;
    }
    else if ( key == 'p' ) {
        paused = !paused;
    }
    else if ( key == 'b' ) {
        
        frustumHelp.calcNearAndFarClipCoordinates( cam );
        for ( int i=0; i<dpVector.size(); i++ ) {
            DanceParticle *dp = dpVector[i];
            dp->pos.set( frustumHelp.getRandomPointOnFarPlane() );
            cout << dp->pos << " new position" << endl;
        }
    }
}

void testApp::mousePressed(int x, int y, int button){
    cout << "YES" << endl;
    if ( dpVector.size() > 0 ) {
        frustumHelp.calcNearAndFarClipCoordinates( cam );
        DanceParticle *dp = dpVector[0];
        dp->pos.x = ofMap( x, 0, ofGetWidth(), frustumHelp.farClip[FrustumHelp::FTL].x, frustumHelp.farClip[FrustumHelp::FTR].x );
        dp->pos.y = ofMap( y, 0, ofGetHeight(), frustumHelp.farClip[FrustumHelp::FTL].y, frustumHelp.farClip[FrustumHelp::FBL].y );
        dp->pos.z = frustumHelp.farClip[FrustumHelp::FTL].z;
    }
    
}
