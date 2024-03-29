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
	
    globe.init( 300 );
    globe.pos.set( 300, 0, 0 );
    ofAddListener( globe.latLonTweenEnded, this, &testApp::globeLatLonTweenEnded );
    
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
    
	
	
    // setup the networking
    NM.dvManager = &dvManager;
	NM.dpManager = &dpManager;
	NM.setup();
    NM.requestHandshake( VERSION_NUMBER );
	
	LM.setup();
	currentCityIndex = 8;
	globe.setLatLon( LM.latLonForCity(currentCityIndex)  );

	
}

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
    
    globe.update( deltaMillis );
}

void testApp::draw(){  
    glEnable( GL_DEPTH_TEST );
    ofEnableAlphaBlending();  
    
    float camZ = cam.getPosition().z;
    if ( camZ > 0 )
        cam.setPosition( 0, 0, -fabs(camZ) );
    cam.lookAt( ofVec3f(), ofVec3f( 0, -1, 0 ) );
    
    cam.begin();
    
//    cam.enableOrtho();
//    ofPushMatrix();
//    ofTranslate( ofGetWidth()/2, -ofGetHeight()/2 );
    globe.draw();
//    ofPopMatrix();
    
    ofVec3f sf = globe.getWorldCoordForLatLon( LM.cityLatLonHash[ "San Francisco" ] );
    ofVec3f sfScreen = cam.worldToScreen( sf );
    
    ofVec3f city = globe.getWorldCoordForLatLon( LM.cityLatLonHash[ LM.cities[currentCityIndex]] );
    ofVec3f cityScreen = cam.worldToScreen( city );
    
    glPushMatrix();
    dpManager.draw();
    glPopMatrix();
    cam.end();
    
    
    ofSetupScreen();
    glDisable( GL_DEPTH_TEST );
    
//    ofSetColor( 255, 0, 0 );
//    ofCircle( sfScreen.x, sfScreen.y, 16 );
//    ofSetColor( 0, 255, 0 );
//    ofCircle( cityScreen.x, cityScreen.y, 16 );
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
}

void testApp::danceVideoLoaded( danceVideo & dv ) {
    
    dpManager.createParticle( &dv, dv.isNew );
}

void testApp::globeLatLonTweenEnded( int & theId ) {
    
    cout << "globe lat lon tween ended" << endl;
    // tell the DPManager to do something with particles for the corresponding city
    if ( mode == GLOBE_MODE ) {
        dpManager.animateParticlesForCity( LM.cities[ currentCityIndex ], globe.getWorldCoordForLatLon( LM.latLonForCity( currentCityIndex ) ) );
        cityName = LM.cities[currentCityIndex];
        cityTextRect = tradeGothic.getStringBoundingBox( cityName, 0, 0 );
        cityTextTween.setParameters( easingQuad, ofxTween::easeInOut, -(cityTextRect.width+20), 0, 400, 0);
    }
    
}

void testApp::globeToRandomCity( int delay ) {
    if ( dpManager.dpVector.size() == 0 )
        return;
    
    int numDancesInCity = 0;
    while ( numDancesInCity == 0 ) {
        currentCityIndex = ofRandom( 0, LM.cities.size() );
        numDancesInCity = dpManager.getNumDancesInCity( LM.cities[ currentCityIndex ] );    
    }
    
    globe.setLatLon( LM.latLonForCity(currentCityIndex), 500, delay );
    cityTextRect = tradeGothic.getStringBoundingBox( cityName, 0, 0 );
    cityTextTween.setParameters( easingQuad, ofxTween::easeInOut, cityTextX, -(cityTextRect.width+20), 400, delay );
}

void testApp::switchMode( VizMode nextMode ) {
    if ( mode == nextMode )
        return;
    
    mode = nextMode;
    switch ( mode ) {
        case STARFIELD_MODE:
            globe.tweenGlobeToScale( 0, 500 );
            dpManager.transitionToStarfieldMode( 500, 500 );
            cityTextRect = tradeGothic.getStringBoundingBox( cityName, 0, 0 );
            cityTextTween.setParameters( easingQuad, ofxTween::easeInOut, cityTextX, -(cityTextRect.width+20), 400, 0 );
            break;
            
        case GLOBE_MODE:
            globe.tweenGlobeToScale( 1, 500, 500 );
            dpManager.transitionToGlobeMode( 500, 0 );
            globeToRandomCity( 1000 );
            break;
    }
    
}

void testApp::keyPressed(int key){
    
    if ( key == '1' ) {
        switchMode( STARFIELD_MODE );
    }
    else if ( key == '2' ) {
        switchMode( GLOBE_MODE );
    }
    
  
    if ( key == 'c' ) {
        // pick a random city for the globe to spin to if we are in globe mode
        if ( mode == GLOBE_MODE ) {
            globeToRandomCity(0);
        }
    }
    
//    if ( key == OF_KEY_DOWN ) {
//        if ( ++currentCityIndex >= cities.size() )
//            currentCityIndex = 0;
//        updateCity();
//    }
//    else if ( key == OF_KEY_UP ) {
//        if ( --currentCityIndex < 0 )
//            currentCityIndex = cities.size() - 1;
//        updateCity();
//    }
    
    else if ( key == OF_KEY_LEFT ) {
    }
    else if ( key == OF_KEY_RIGHT ) {
    }
    else if ( key == 'z' ) {
        DanceParticle *dp = dpManager.dpVector.at( (int)ofRandom( 0.0, dpManager.dpVector.size()-1.0 ) );
        dpManager.zoomParticle( dp );
        NM.requestLargeVideo( dp->DV );
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
