#include "testApp.h"

static int FRAMES_PER_BEAT = 6;
static int BEATS_PER_CAPTURE = 4;
static int LOCKED_BPM = 110;
static int NUM_CAPTURES = 4;

void testApp::setup(){
    ofSetVerticalSync( TRUE );
    ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    
    helper = [[Helper alloc] init];
    [helper setApp:this];
    
    previewView = 0;
    emailView = 0;
    tapView = new TapView();
    ofAddListener(tapView->startCaptureEvent, this, &testApp::startCapturing);
    
    camWidth = 640;
    camHeight = 480;
    
    grabber.setVerbose( true );
    grabber.initGrabber( camWidth, camHeight );
    
    ofSetWindowShape( 1024 , 768 );
    
    showFrames = false;
    
    lastMillis = 0;
    isCapturing = false;
    captureMillis = 0;
    
    isUploading = false;
    uploadMessage = "";
    
    isEmailing = false;
    
    ofTrueTypeFont::setGlobalDpi(72);
    verdana.loadFont( "verdana.ttf", 24, true );
//    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &testApp::onGifSaved);
    
    string thePath = ofToDataPath( "test.gif", true );
    NSString *pathString = [NSString stringWithCString:thePath.c_str() encoding:NSUTF8StringEncoding];
    [helper gifDecode:pathString];
    
    cout << "the path: " << thePath << std::endl;
}

void testApp::update(){
    
    int currentMillis = ofGetElapsedTimeMillis();
    int deltaMillis = currentMillis - lastMillis;
    lastMillis = currentMillis;
    
    grabber.grabFrame();
    
    
    if ( tapView ) {
        tapView->update();
    }
    
    
    if ( previewViews.size() > 0 ) {
        for ( int i=0; i<previewViews.size(); i++ ) {
            previewViews[i]->update();
        }
    }
//    if ( previewView ) {
//        previewView->update();
//    }
    
    if ( isCapturing ) {
        captureMillis += deltaMillis;
        if ( captureMillis > captureInterval ) {
            captureFrame();
            captureMillis -= captureInterval;
            cout << "capture " << (currentCaptureIndex+1) << " frames captured: " << frames[currentCaptureIndex].size() << std::endl;
            
            if ( frames[currentCaptureIndex].size() == FRAMES_PER_BEAT * BEATS_PER_CAPTURE ) {
                if ( ++currentCaptureIndex == NUM_CAPTURES ) {
                    isCapturing = false;
                    startPreviewing();
                }
            }
        }
    }
    
    if ( isUploading && ![helper isUploading] ) {
        // the upload finished
        isUploading = false;
        uploadMessage = [helper sup];
        
        cout << "upload message: " << uploadMessage << std::endl;
        
        emailView = new EmailView();
        ofAddListener( emailView->emailAddressEnteredEvent, this, &testApp::emailAddressEntered );
        ofAddListener( emailView->emailAddressSkippedEvent, this, &testApp::emailAddressEntered );
    }
    
    if ( isEmailing && ![helper isEmailing] ) {
        
        isEmailing = false;
        destroyPreview();
    }
}

void testApp::draw(){
    ofBackground( 0, 0, 0 );
    
    ofSetColor( 255, 255, 255 );
    grabber.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    
    if ( tapView ) {
        tapView->draw();
    }
    
    if ( previewViews.size() > 0 ) {
        if ( previewViews.size() == 1 ) {
            previewViews[0]->draw();
        }
        else if ( previewViews.size() == 4 ) {
            int previewWidth = ofGetWidth() / 2;
            int previewHeight = ofGetHeight() / 2;
            
            previewViews[0]->draw( 0, 0, previewWidth, previewHeight );
            previewViews[1]->draw( previewWidth, 0, previewWidth, previewHeight );
            previewViews[2]->draw( 0, previewHeight, previewWidth, previewHeight );
            previewViews[3]->draw( previewWidth, previewHeight, previewWidth, previewHeight );                                
        }
        
        if ( uploadMessage != "" ) {
            ofRectangle stringRect = verdana.getStringBoundingBox( uploadMessage, 0, 0 );
            float stringX = ofGetWidth() / 2.0 - stringRect.width / 2.0;
            float stringY = ofGetHeight() / 2.0 - 100;// - stringRect.height / 2.0;
            ofSetColor( 255, 255, 255, 200 );
            verdana.drawString( uploadMessage, stringX, stringY );
        }
    }
    
//    if ( previewView ) {
//        previewView->draw();
//        if ( uploadMessage != "" ) {
//            ofRectangle stringRect = verdana.getStringBoundingBox( uploadMessage, 0, 0 );
//            float stringX = ofGetWidth() / 2.0 - stringRect.width / 2.0;
//            float stringY = ofGetHeight() / 2.0 - 100;// - stringRect.height / 2.0;
//            ofSetColor( 255, 255, 255, 200 );
//            verdana.drawString( uploadMessage, stringX, stringY );
//        }
//    }
    
    if ( emailView ) {
        emailView->draw();
    }
    
    if ( isUploading ) {
        ofSetColor( 255, 255, 255, 100);
        ofRect(0, 0, ofGetWidth() * [helper uploadProgress], ofGetHeight() );
    }
}

void testApp::emailAddressEntered( string & emailAddress ) {
    
    if ( emailAddress == "" ) {
        cout << "no email address was entered" << endl;
        destroyPreview();
    }
    else {
        cout << "this email address was entered: " << emailAddress << endl;
        
        NSString *email = [NSString stringWithCString:emailAddress.c_str() encoding:[NSString defaultCStringEncoding]];
        [helper sendEmailTo:email];
        isEmailing = true;
    }
    
}

void testApp::startCapturing(double & d){
    currentCaptureIndex = 0;
    chosenCaptureIndex = 0;
    captureFrame();
    isCapturing = true;
    captureMillis = 0;
    captureInterval = d / (double)FRAMES_PER_BEAT;
    cout << "startCapturing with interval of " << captureInterval << std::endl;
}

void testApp::startPreviewing() {
    
    for ( int i=0; i<NUM_CAPTURES; i++ ) {
        PreviewView *pv = new PreviewView();
        pv->init( frames[i], captureInterval );
        previewViews.push_back( pv );
    }
//    previewView = new PreviewView();
//    previewView->init( frames[0], captureInterval );
}

void testApp::captureFrame() {
    
    ofImage *frame = new ofImage();
    ofPixels pixels = grabber.getPixelsRef();
    frame->setFromPixels( pixels );
    frames[currentCaptureIndex].push_back( frame );
}

void testApp::saveVideoFiles() {
    
    if ( chosenCaptureIndex >= NUM_CAPTURES )
        chosenCaptureIndex = 0;
    
    videoSaver.setup( frames[chosenCaptureIndex][0]->width, frames[chosenCaptureIndex][0]->height );
    videoSaver.startMovie();
    for ( int i=0; i<frames[chosenCaptureIndex].size(); i++ ) {
        ofImage *img = frames[chosenCaptureIndex][i];
        videoSaver.addFrame( img->getPixels(), captureInterval / 1000.0 );
    }
    videoSaver.finishMovie();
    string fileNameMinusExtension = videoSaver.finalFileSavingSetting();
    [helper startVideoUploadRequest:[NSString stringWithCString:fileNameMinusExtension.c_str() encoding:NSUTF8StringEncoding] withNumFrames:BEATS_PER_CAPTURE*FRAMES_PER_BEAT];
    
    isUploading = true;
}

void testApp::destroyPreview() {
    
//    if ( previewView ) {
//        delete previewView;
//        previewView = 0;
//    }
    
    if ( previewViews.size() > 0 ) {
        for ( int i=0; i<previewViews.size(); i++ ) {
            PreviewView *pv = previewViews[i];
            delete pv;
        }
        previewViews.clear();
    }
    
    if ( emailView ) {
        ofRemoveListener( emailView->emailAddressEnteredEvent, this, &testApp::emailAddressEntered );
        ofRemoveListener( emailView->emailAddressSkippedEvent, this, &testApp::emailAddressEntered );
        delete emailView;
        emailView = 0;
    }
    
    for ( int i = 0; i < NUM_CAPTURES; i++ ) {
        for ( int j = 0; j < frames[i].size(); j++ ) {
            ofImage *img = frames[i][j];
            delete img;
        }
        frames[i].clear();
    }
    uploadMessage = "";
    tapView->reset();
    
}

void testApp::keyPressed(int key){
    if ( emailView ) return;
    
    // if the preview view is active, we handle things here differently
    if ( previewViews.size() > 0 && !isUploading ) {
        if ( key == 'x' ) {
            destroyPreview();
        }
        else if ( key == '1' ) {
            chosenCaptureIndex = 0;
            saveVideoFiles();
        }
        else if ( key == '2' ) {
            chosenCaptureIndex = 1;
            saveVideoFiles();
        }
        else if ( key == '3' ) {
            chosenCaptureIndex = 2;
            saveVideoFiles();
        }
        else if ( key == '4' ) {
            chosenCaptureIndex = 3;
            saveVideoFiles();
        }
//        else if ( key == 'm' ) {
//            saveVideoFiles();
//        }
    }
    else {
        if ( key == 'p' ) {
            
        }
        else if ( key == 'c' ) {
            cout << " beginning countdown" << endl;
            tapView->beginCountdown( ( 60.0 / (double)LOCKED_BPM ) * 1000.0, true );
        }
        else if ( key == ' ' ) {
            tapView->tap();
        }
    }
}

void testApp::onGifSaved( string & filename ) {
    cout << "the gif was saved with filename: " << filename << std::endl;
    double now = ofGetElapsedTimeMillis();
    double elapsedGifMillis = now - gifStartTime;
    cout << "gif encoded in " << elapsedGifMillis / 1000.0 << " seconds" << std::endl;
}

void testApp::windowResized(int w, int h){

}