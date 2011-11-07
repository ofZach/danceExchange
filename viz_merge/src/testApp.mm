#include "testApp.h"

#define VERSION_NUMBER 1

//--------------------------------------------------------------
void testApp::setup() {
       
    mode = STARFIELD_MODE;
    
    drawTextures = false;
    paused = false;
    
    fov = 60.0;
    nearClip = 1.0;
    farClip = 5000.0;
    
    cam.setFov( fov );
    cam.setNearClip( nearClip );
    cam.setFarClip( farClip );
    frustumHelp.setup( nearClip, farClip, fov, ofGetWidth()/(float)ofGetHeight() );
    
    cam.cacheMatrices();
    
    tradeGothic.loadFont( "TradeGothicLTStd-BdCn20.otf",  64 );
    cityTextX = 0;
    cityName = "";
    
	ofEnableSmoothing();
    ofDisableArbTex();
    ofSetVerticalSync( true );
    ofSetFrameRate( 60.0 );
	ofBackground(0, 0, 0);
//    ofBackground( 255, 255, 255 );
	
    pointilist.init( 1000, true );
    
    dpManager.init( &pointilist );
    dpManager.frustumHelp = frustumHelp;
    
    dvManager.init( &pointilist );
    ofAddListener( dvManager.danceVideoLoadedEvent, this, &testApp::danceVideoLoaded );
    
	BM.DVM = &dvManager;
	BM.pointilist = &pointilist;

	MM.DVM = &dvManager;
	MM.pointilist = &pointilist;
	
	
    // setup the networking
    NM.dvManager = &dvManager;
	NM.dpManager = &dpManager;
	NM.setup();
    NM.requestHandshake( VERSION_NUMBER );
	
	LM.setup();
	currentCityIndex = 8;
//	globe.setLatLon( LM.latLonForCity(currentCityIndex)  );
	
	
	//bg.loadImage("images/bg.png");
	BM.setup();
	MM.setup();
	
}


int lastMode = -1;
void testApp::update(){
    // update the networking. 
    NM.update();
    
	int currentMillis = ofGetElapsedTimeMillis();
	int deltaMillis = currentMillis - lastMillis;
	lastMillis = currentMillis;
    
    
    frustumHelp.calcNearAndFarClipCoordinates( cam );
    
    dvManager.update( deltaMillis );
    
    dpManager.frustumHelp.calcNearAndFarClipCoordinates( cam );
    dpManager.update( deltaMillis );
    
//    globe.update( deltaMillis );
	
	int whichLast =lastMode;
	if (whichLast != BRAND_MODE && mode == BRAND_MODE){
		BM.start();	
	}
	if (whichLast != MAP_MODE && mode == MAP_MODE){
		MM.start();	
	}
	lastMode = mode;
	
	if (mode == BRAND_MODE){
		BM.update();
	}
	if (mode == MAP_MODE){
		MM.update();
	}
	
	
}

void testApp::draw(){  
	
	ofEnableAlphaBlending();
	ofSetColor(255,255,255, 130);
	glDisable(GL_DEPTH_TEST);
	//bg.draw(0,0);
	
    glEnable( GL_DEPTH_TEST );
    ofEnableAlphaBlending();  
    
    float camZ = cam.getPosition().z;
    if ( camZ > 0 )
        cam.setPosition( 0, 0, -fabs(camZ) );
    cam.lookAt( ofVec3f(), ofVec3f( 0, -1, 0 ) );
    
    cam.begin();
    
    glPushMatrix();
    dpManager.draw();
    glPopMatrix();
    cam.end();
    
    
    ofSetupScreen();
    glDisable( GL_DEPTH_TEST );
    ofSetColor( 255, 255, 255 );
    
	ofDrawBitmapString( "fps: "+ofToString(ofGetFrameRate(),2) + "\nnum particles: " + ofToString(dpManager.dpVector.size(), 2), 10, ofGetHeight() - 40 );
    if ( cityTextTween.isRunning() ) cityTextX = cityTextTween.update();
    tradeGothic.drawString( cityName, cityTextX, 80 );
    
    if ( drawTextures ) {
        for ( int i = 0; i < dvManager.textures.size(); i++ ) {
            ofTexture &tex = dvManager.textures[i];
            tex.draw( i * 200, ofGetHeight() - 200, 200, 200 );
        }
    }
	
	if (mode == BRAND_MODE){
		BM.draw();
	}
	if (mode == MAP_MODE){
		MM.draw();	
	}
}

void testApp::danceVideoLoaded( danceVideo & dv ) {
    
//	printf("video loaded ! \n");
    dpManager.createParticle( &dv, dv.isNew );
}

void testApp::switchMode( VizMode nextMode ) {
    if ( mode == nextMode )
        return;
    
    if ( nextMode == GLOBE_MODE )
        return;
    
    mode = nextMode;
    switch ( mode ) {
        case STARFIELD_MODE:
            dpManager.transitionToStarfieldMode( 500, 500 );
            cityTextRect = tradeGothic.getStringBoundingBox( cityName, 0, 0 );
            cityTextTween.setParameters( easingQuad, ofxTween::easeInOut, cityTextX, -(cityTextRect.width+20), 400, 0 );
            break;
			
		case BRAND_MODE:
			
			dpManager.transitionToGlobeMode( 500, 0 );
			cityTextRect = tradeGothic.getStringBoundingBox( cityName, 0, 0 );
            cityTextTween.setParameters( easingQuad, ofxTween::easeInOut, cityTextX, -(cityTextRect.width+20), 400, 0 );
			// do something with dpManager ?
			
			break;
		case MAP_MODE:
			dpManager.transitionToGlobeMode( 500, 0 );
			cityTextRect = tradeGothic.getStringBoundingBox( cityName, 0, 0 );
            cityTextTween.setParameters( easingQuad, ofxTween::easeInOut, cityTextX, -(cityTextRect.width+20), 400, 0 );
			// do something with dpManager ?
			break;
    }
    
}

void testApp::keyPressed(int key){
    
    if ( key == '1' ) {
        switchMode( STARFIELD_MODE );
    } else if ( key == '2' ) {
        switchMode( GLOBE_MODE );
    } else if (key == '3'){
		switchMode( BRAND_MODE );
	} else if (key == '4'){
		switchMode( MAP_MODE );
	}
    else if ( key == OF_KEY_LEFT ) {
    }
    else if ( key == OF_KEY_RIGHT ) {
    }
    else if ( key == 'z' ) {
        DanceParticle *dp = dpManager.dpVector.at( (int)ofRandom( 0.0, dpManager.dpVector.size()-1.0 ) );
        dpManager.zoomParticle( dp );
        NM.requestLargeVideo( dp->DV );
    }
    else if ( key == 'r' ) {
        NM.requestRandomDances( NUM_RANDOM_VIDEOS );
    }
    if ( key == ' ' ) {
        drawTextures = !drawTextures;
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
}

void testApp::keyReleased(int key) {
}

void testApp::mousePressed(int x, int y, int button){
    cam.disableMouseInput();
    
}
