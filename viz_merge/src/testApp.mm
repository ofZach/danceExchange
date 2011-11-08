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
    
    ofEnableSmoothing();
    ofDisableArbTex();
    ofSetVerticalSync( true );
    ofSetFrameRate( 60.0 );
	ofBackground(0, 0, 0);
//    ofBackground( 255, 255, 255 );
	
    pointilist.init( 1000, true );
    
    dpManager.init( &pointilist );
    dpManager.frustumHelp = frustumHelp;
    
    
    bool isMacMini = false;
    dvManager.init( &pointilist, isMacMini );
    ofAddListener( dvManager.danceVideoLoadedEvent, this, &testApp::danceVideoLoaded );
    
	BM.DVM = &dvManager;
	BM.pointilist = &pointilist;

	MM.DVM = &dvManager;
	MM.pointilist = &pointilist;
	
	BRSTM.DVM = &dvManager;
	BRSTM.pointilist = &pointilist;
	
	
	
    // setup the networking
    NM.dvManager = &dvManager;
	NM.dpManager = &dpManager;
	NM.setup();
    NM.requestHandshake( VERSION_NUMBER );
	
	LM.setup();
	BM.setup();
	MM.setup();
	BRSTM.setup();
	
	scenes.push_back(sceneAndDuration(STARFIELD_MODE,12));
	scenes.push_back(sceneAndDuration(BRAND_MODE,12));
	scenes.push_back(sceneAndDuration(MAP_MODE,5));
	scenes.push_back(sceneAndDuration(BURST_MODE,5));
	for (int i = 0; i < scenes.size(); i++){
		totalTime += scenes[i].duration;
	}
	curSceneAndDur = scenes[0];
	
}


int lastMode = -1;
void testApp::update(){
    
	float t = ofGetElapsedTimef();
	while (t > totalTime){
		t -= totalTime;	
	}
	int whereAmI = 0;
	float adder = 0;
	for (int i = 0; i < scenes.size(); i++){
		if (t >= adder && t < (adder + scenes[i].duration)){
			whereAmI = i;
		}
		adder += scenes[i].duration;
	}
	
	if (curSceneAndDur.mode !=  scenes[whereAmI].mode){
			
		switchMode((VizMode)scenes[whereAmI].mode);
	}
	curSceneAndDur = scenes[whereAmI];
	
	
	
	if (bAmChangingScenes == true){
		if (ofGetElapsedTimef() - sceneChangeStartTime > sceneChangeTime){
			bAmChangingScenes = false;	
		}
		if ((ofGetElapsedTimef() - sceneChangeStartTime) > (sceneChangeTime/2)){
			mode = nextScene;
		}
	}
	
	
	// update the networking. 
    NM.update();
    
	int currentMillis = ofGetElapsedTimeMillis();
	int deltaMillis = currentMillis - lastMillis;
	lastMillis = currentMillis;
    
    
    frustumHelp.calcNearAndFarClipCoordinates( cam );
    
    dvManager.update( deltaMillis );
    
    dpManager.frustumHelp.calcNearAndFarClipCoordinates( cam );
    dpManager.update( deltaMillis );
    
	
	int whichLast =lastMode;
	if (whichLast != BRAND_MODE && mode == BRAND_MODE){
		BM.start();	
	}
	if (whichLast != MAP_MODE && mode == MAP_MODE){
		MM.start();	
	}
	if (whichLast != BURST_MODE && mode == BURST_MODE){
		BRSTM.start();
	}
	lastMode = mode;
	
	if (mode == BRAND_MODE){
		BM.update();
	}
	if (mode == MAP_MODE){
		MM.update();
	}
	if (mode == BURST_MODE){
		BRSTM.update();
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
	if (mode == BURST_MODE){
		BRSTM.draw();
	}
	
	
	if (bAmChangingScenes == true){
		ofEnableAlphaBlending();
		float pct = (ofGetElapsedTimef() - sceneChangeStartTime) / sceneChangeTime;
	
		float alpha = sin(pct * PI);
		ofSetColor(0,0,0,255*alpha);
		ofFill();
		ofRect(0,0,ofGetWidth(), ofGetHeight());
		
	}
	
	
	
}

void testApp::danceVideoLoaded( danceVideo & dv ) {
    
//	printf("video loaded ! \n");
    dpManager.createParticle( &dv, dv.isNew );
}



void testApp::switchMode( VizMode nextMode ) {
    if ( mode == nextMode )
        return;
    
	for (int i = 0; i < dvManager.danceVideos.size(); i++){
		dvManager.danceVideos[i]->isInUse = false;
	}

    
    //mode = nextMode;
    switch ( nextMode ) {
        case STARFIELD_MODE:
            dpManager.transitionToStarfieldMode( 500, 500 );
            break;
			
		case BRAND_MODE:
			
			dpManager.transitionToGlobeMode( 500, 0 );
			// do something with dpManager ?
			
			break;
		case MAP_MODE:
			dpManager.transitionToGlobeMode( 500, 0 );
			// do something with dpManager ?
			break;
		case BURST_MODE:
			dpManager.transitionToGlobeMode( 500, 0 );
			//globe.tweenGlobeToScale( 0, 500 );
			// do something with dpManager ?
			break;
    }
	
	sceneChangeTime = 0.8;
	sceneChangeStartTime = ofGetElapsedTimef();
	bAmChangingScenes = true;
	nextScene = nextMode;
	
}

void testApp::keyPressed(int key){
    
    if ( key == '1' ) {
        switchMode( STARFIELD_MODE );
    }  else if (key == '3'){
		switchMode( BRAND_MODE );
	} else if (key == '4'){
		switchMode( MAP_MODE );
	} else if (key == '5'){
		switchMode( BURST_MODE );
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
//        NM.requestRandomDances( dvManager.numRandomVideos );
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

void testApp::windowResized( int w, int h ) {
    frustumHelp.setup( nearClip, farClip, fov, ofGetWidth()/(float)ofGetHeight() );
    dpManager.frustumHelp = frustumHelp;
    
}