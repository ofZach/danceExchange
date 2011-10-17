#include "testApp.h"

static int FRAMES_PER_BEAT = 6;
static int BEATS_PER_CAPTURE = 4;

void testApp::setup(){
    ofSetVerticalSync( TRUE );
    ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    
    helper = [[Helper alloc] init];
    [helper setApp:this];
    
    previewView = 0;
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
    
    if ( previewView ) {
        previewView->update();
    }
    
    if ( isCapturing ) {
        captureMillis += deltaMillis;
        if ( captureMillis > captureInterval ) {
            captureFrame();
            captureMillis -= captureInterval;
            cout << "frames captured: " << frames.size() << std::endl;
            
            if ( frames.size() == FRAMES_PER_BEAT * BEATS_PER_CAPTURE ) {
                isCapturing = false;
                startPreviewing();
            }
        }
    }
    
    if ( isUploading && ![helper isUploading] ) {
        // the upload finished
        isUploading = false;
        uploadMessage = [helper sup];
        cout << "upload message: " << uploadMessage << std::endl;
    }
}

void testApp::draw(){
    ofBackground( 0, 0, 0 );
    
    ofSetColor( 255, 255, 255 );
    grabber.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    
    if ( tapView ) {
        tapView->draw();
    }
    
    if ( previewView ) {
        previewView->draw();
        if ( uploadMessage != "" ) {
            ofRectangle stringRect = verdana.getStringBoundingBox( uploadMessage, 0, 0 );
            float stringX = ofGetWidth() / 2.0 - stringRect.width / 2.0;
            float stringY = ofGetHeight() / 2.0;// - stringRect.height / 2.0;
            ofSetColor( 255, 255, 255, 200 );
            verdana.drawString( uploadMessage, stringX, stringY );
        }
    }
    
    if ( isUploading ) {
        ofSetColor( 255, 255, 255, 100);
        ofRect(0, 0, ofGetWidth() * [helper uploadProgress], ofGetHeight() );
    }
}

void testApp::startCapturing(double & d){
    captureFrame();
    isCapturing = true;
    captureMillis = 0;
    captureInterval = d / (double)FRAMES_PER_BEAT;
    cout << "startCapturing with interval of " << captureInterval << std::endl;
}

void testApp::startPreviewing() {
    previewView = new PreviewView();
    previewView->init( frames, captureInterval );
}

void testApp::captureFrame() {
    
    ofImage *frame = new ofImage();
    ofPixels pixels = grabber.getPixelsRef();
    frame->setFromPixels( pixels );
//    frame->resize( 640, 360 );
    frames.push_back( frame );
}

void testApp::saveVideoFiles() {
    
    videoSaver.setup( frames[0]->width, frames[0]->height );
    videoSaver.startMovie();
    for ( int i=0; i<frames.size(); i++ ) {
        ofImage *img = frames[i];
        videoSaver.addFrame( img->getPixels(), captureInterval / 1000.0 );
    }
    videoSaver.finishMovie();
    string fileNameMinusExtension = videoSaver.finalFileSavingSetting();
    [helper startVideoUploadRequest:[NSString stringWithCString:fileNameMinusExtension.c_str() encoding:NSUTF8StringEncoding] withNumFrames:BEATS_PER_CAPTURE*FRAMES_PER_BEAT];
    
    isUploading = true;
}

void testApp::keyPressed(int key){
    // if the preview view is active, we handle things here differently
    if ( previewView && !isUploading ) {
        if ( key == 'x' ) {
            delete previewView;
            previewView = 0;
            for ( int i = 0; i < frames.size(); i++ ) {
                ofImage *img = frames[i];
                delete img;
            }
            frames.clear();
            uploadMessage = "";
            tapView->reset();
        }
//        else if ( key == 'u' ) {
//            cout << "start uploading..." << std::endl;
//            for ( int i=0; i<frames.size(); i++ ) {
//                ofImage *img = frames[i];
//                [helper addFrame:img->getPixels() withWidth:img->width andHeight:img->height];
//            }
//            [helper startGifRequest:captureInterval];
//            isUploading = true;
//        }
//        else if ( key == 's' ) {
//            gifEncoder.setup( frames[0]->width, frames[0]->height, captureInterval / 1000.0, 256 );
//            gifEncoder.setDitherMode( OFX_GIF_DITHER_BAYER16x16 );
//            for ( int i=0; i<frames.size(); i++ ) {
//                ofImage *img = frames[i];
//                gifEncoder.addFrame( *img );
//            }
//            gifStartTime = ofGetElapsedTimeMillis();
//            gifEncoder.save( "test.gif" );
//        }
        else if ( key == 'm' ) {
            saveVideoFiles();
        }
//        // if there is an upload message, then we finished an upload
//        // open the gif's url and clear out preview mode
//        else if ( key == ' ' && uploadMessage != "" ) {
//            ofLaunchBrowser( uploadMessage );
//            delete previewView;
//            previewView = 0;
//            for ( int i = 0; i < frames.size(); i++ ) {
//                ofImage *img = frames[i];
//                delete img;
//            }
//            frames.clear();
//            tapView->reset();
//            uploadMessage = "";
//        }
    }
    else {
        if ( key == 'p' ) {
            
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