#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    upKey = downKey = leftKey = rightKey = false;
    
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
//    cam.disableMouseInput();
    
    globe.init( 200 );
    
	ofEnableSmoothing();
    ofDisableArbTex();
    ofSetVerticalSync( true );
    ofSetFrameRate( 60.0 );
	ofBackground(0, 0, 0);
	
    dpManager.init();
    dpManager.frustumHelp = frustumHelp;
    
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
    
//    if ( upKey ) globe.xRotation++;
//    if ( downKey ) globe.xRotation--;
//    if ( leftKey ) globe.yRotation++;
//    if ( rightKey ) globe.yRotation--;
    
    frustumHelp.calcNearAndFarClipCoordinates( cam );
    
    dpManager.frustumHelp.calcNearAndFarClipCoordinates( cam );
    dpManager.update( deltaMillis );
}

void testApp::updateNetworkIO() {
    
    vector<DanceInfo> danceInfos = dbHelper.danceInfos;
    if ( danceInfos.size() > 0 ) {
        for ( int i=0; i<danceInfos.size(); i++ ) {
            dpManager.createParticle( danceInfos[i] );
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

void testApp::draw(){  
    glEnable( GL_DEPTH_TEST );
    ofEnableAlphaBlending();  
    
    float camZ = cam.getPosition().z;
    if ( camZ > 0 )
        cam.setPosition( 0, 0, -fabs(camZ) );
    cam.lookAt( ofVec3f(), ofVec3f( 0, -1, 0 ) );
    
    cam.begin();
    
//    globe.draw();
    
    ofVec3f sf = globe.getWorldCoordForLatLon( cityLatLonHash[ "San Francisco" ] );
    ofVec3f sfScreen = cam.worldToScreen( sf );
    
    glPushMatrix();
    dpManager.draw();
//	pointilist.draw();
    glPopMatrix();
    cam.end();
    
    
    ofSetupScreen();
    glDisable( GL_DEPTH_TEST );
    
	ofDrawBitmapString( "fps: "+ofToString(ofGetFrameRate(),2) + "\nnum particles: " + ofToString(dpManager.dpVector.size(), 2), 10, 15 );
    ofDrawBitmapString( cities[currentCityIndex], 10, 45 );
    
    if ( drawTextures ) {
        for ( int i = 0; i < dpManager.textures.size(); i++ ) {
            ofTexture &tex = dpManager.textures[i];
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
    
    if ( key == OF_KEY_DOWN )
        downKey = true;
    if ( key == OF_KEY_UP )
        upKey = true;
    if ( key == OF_KEY_LEFT )
        leftKey = true;
    if ( key == OF_KEY_RIGHT )
        rightKey = true;
    
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
    if ( key == OF_KEY_DOWN )
        downKey = false;
    if ( key == OF_KEY_UP )
        upKey = false;
    if ( key == OF_KEY_LEFT )
        leftKey = false;
    if ( key == OF_KEY_RIGHT )
        rightKey = false;
}

void testApp::mousePressed(int x, int y, int button){
    
}
