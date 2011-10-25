#include "testApp.h"

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
	
    globe.init( 200 );
    ofAddListener( globe.latLonTweenEnded, this, &testApp::globeLatLonTweenEnded );
    
	ofEnableSmoothing();
    ofDisableArbTex();
    ofSetVerticalSync( true );
    ofSetFrameRate( 60.0 );
	ofBackground(0, 0, 0);
	
    pointilist.init( 1000, true );
    
    dpManager.init( &pointilist );
    dpManager.frustumHelp = frustumHelp;
    
    dvManager.init( &pointilist );
    ofAddListener( dvManager.danceVideoLoadedEvent, this, &testApp::danceVideoLoaded );
    
	
	
    // setup the networking
    NM.dvManager = &dvManager;
	NM.dpManager = &dpManager;
	NM.setup();
	
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
    
    globe.draw();
    
    ofVec3f sf = globe.getWorldCoordForLatLon( LM.cityLatLonHash[ "San Francisco" ] );
    ofVec3f sfScreen = cam.worldToScreen( sf );
    
    glPushMatrix();
    dpManager.draw();
    glPopMatrix();
    cam.end();
    
    
    ofSetupScreen();
    glDisable( GL_DEPTH_TEST );
    
	ofDrawBitmapString( "fps: "+ofToString(ofGetFrameRate(),2) + "\nnum particles: " + ofToString(dpManager.dpVector.size(), 2), 10, 15 );
    ofDrawBitmapString( LM.cities[currentCityIndex], 10, 45 );
    
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
    }
    
}

void testApp::switchMode( VizMode nextMode ) {
    if ( mode == nextMode )
        return;
    
    mode = nextMode;
    switch ( mode ) {
        case STARFIELD_MODE:
            globe.tweenGlobeToScale( 0, 500 );
            dpManager.transitionToStarfieldMode( 500, 500 );
            break;
            
        case GLOBE_MODE:
            globe.tweenGlobeToScale( 1, 500, 500 );
            dpManager.transitionToGlobeMode( 500, 0 );
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
            currentCityIndex = ofRandom( 0, LM.cities.size() );
            globe.setLatLon( LM.latLonForCity(currentCityIndex)  );
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
