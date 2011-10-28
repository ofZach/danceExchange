#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
       
    ofEnableSmoothing();
    //ofDisableArbTex();
    ofSetVerticalSync( true );
    ofSetFrameRate( 60.0 );
	ofBackground(0, 0, 0);

	dvManager.init( );
    ofAddListener( dvManager.danceVideoLoadedEvent, this, &testApp::danceVideoLoaded );
    
	
	
    // setup the networking
    NM.dvManager = &dvManager;
	NM.setup();
	
	
	
}

static int lastMillis = ofGetElapsedTimeMillis();
void testApp::update(){
    // update the networking. 
    NM.update();
    
	int currentMillis = ofGetElapsedTimeMillis();
	int deltaMillis = currentMillis - lastMillis;
	lastMillis = currentMillis;
    
    
   dvManager.update( deltaMillis );
    
 
}

void testApp::draw(){  
    //glEnable( GL_DEPTH_TEST );
    ofEnableAlphaBlending();  
	
	for (int i = 0; i < dvManager.danceVideos.size(); i++){
		dvManager.danceVideos[i]->draw(ofPoint((i%20)*50,(i/20)*38),  50,76/2);
	}
	
	ofSetColor(255,255,255);
	ofDrawBitmapString("frame rate " + ofToString(ofGetFrameRate(), 3), ofPoint(10,10));
    
}

void testApp::danceVideoLoaded( danceVideo & dv ) {
    
   // dpManager.createParticle( &dv, dv.isNew );
}


void testApp::keyPressed(int key){

}

void testApp::keyReleased(int key) {
}

void testApp::mousePressed(int x, int y, int button){
   
    
}
