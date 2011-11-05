#include "testApp.h"

static int FRAMES_PER_BEAT = 6;
static int BEATS_PER_CAPTURE = 4;
static int LOCKED_BPM = 110;
static int NUM_CAPTURES = 4;
static int WHITE_FLASH = 200;
static int NUM_OLD_PREVIEWS = 5;

#define VERSION_NUMBER 1

void testApp::setup(){
    ofSetVerticalSync( TRUE );
    ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    
    helper = [[Helper alloc] init];
    [helper setApp:this];
    [helper setHeroku:YES];
    
    previewView = 0;
    emailView = 0;
    chosenPreviewView = 0;
    tapView = new TapView();
    ofAddListener(tapView->startCaptureEvent, this, &testApp::startCapturing);
    
    tradeGothic.loadFont( "fonts/TradeGothicLTStd-BdCn20.otf", 200, true, false, true );
    tradeGothicSmall.loadFont( "fonts/TradeGothicLTStd-BdCn20.otf", 50 );
//    tradeGothicSmall.setLetterSpacing(.8);
    
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
    verdana.loadFont( "fonts/verdana.ttf", 24, true );
//    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &testApp::onGifSaved);
    
    string thePath = ofToDataPath( "test.gif", true );
    NSString *pathString = [NSString stringWithCString:thePath.c_str() encoding:NSUTF8StringEncoding];
    [helper gifDecode:pathString];
    
    cout << "the path: " << thePath << std::endl;
    
    [helper requestHandshake:VERSION_NUMBER];
    isRequestingHandshake = true;
    
    ofSetFullscreen( true );
	ofHideCursor();

	
	CS.setup();
}

void testApp::update(){
    
	
	
	if (CS.bDone == false){
		CS.update();
		return;
	}
	
	
	
    int currentMillis = ofGetElapsedTimeMillis();
    int deltaMillis = currentMillis - lastMillis;
    lastMillis = currentMillis;
    
    if ( isRequestingHandshake && ![helper isRequestingHandshake] ) {
        isRequestingHandshake = false;
        string updateUrl = [helper appUpdateUrl];
        if ( updateUrl != "" ) {
            ofLaunchBrowser( updateUrl );
            ofExit();
        }
    }
    
    grabber.grabFrame();
    
    
    if ( tapView ) {
        tapView->update();
    }
    
    
    if ( previewViews.size() > 0 ) {
        for ( int i=0; i<previewViews.size(); i++ ) {
            previewViews[i]->update();
        }
    }
    
    for ( int i=0; i<oldPreviews.size(); i++ ) {
        oldPreviews[i]->update();
    }
    
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
    
    if ( whiteFlashTween.isRunning() ) {
        whiteFlashTween.update();
    }
    
    // this is where the upload finishes
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
        chosenPreviewView->startFadeOut();
//        destroyPreview();
    }
}

void testApp::draw(){
   
	
	if (CS.bDone == false){
		CS.draw();
		return;
	} else {
		 ofBackground( 0, 0, 0 );
	}
    
    ofSetColor( 255, 255, 255 );
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
//    grabber.draw( xOffset, 0, aspectWidth, ofGetHeight() );
    grabber.draw( xOffset + aspectWidth, 0, -aspectWidth, ofGetHeight() );
    
    if ( tapView ) {
        tapView->draw();
        if ( !tapView->isCountingDown && !tapView->finishedCountdown ) {
            drawInstructions( "TAP SPACE BAR TO THE BEAT OR C TO JUST START" );
        }
        if ( tapView->isCountingDown ) {
            int countdownNum = 9 - tapView->countdownTicks;
            if ( countdownNum <= 8 ) {
                drawCountdownNumber( countdownNum );
                drawDanceMessage( "GET READY TO DANCE!" );
            }
        }
    }
    
    if ( previewViews.size() > 0 ) {
        
        if ( !chosenPreviewView || ( chosenPreviewView && chosenPreviewView->alpha == 1.0 ) ) {
        
            if ( previewViews.size() == 1 ) {
                previewViews[0]->draw();
            }
            else if ( previewViews.size() == 4 ) {
                int previewWidth = ofGetWidth() / 2;
                int previewHeight = ofGetHeight() / 2;
                
                // draw all the previews
                
                for ( int i=0; i<previewViews.size(); i++ ) {
                    previewViews[i]->draw();
                    drawNumberForPreviewView( i+1, previewViews[i] );
                }
                
                // draw instructions for the operator
                drawInstructions( "PRESS 1, 2, 3 OR 4 TO CHOOSE OR X TO CANCEL" );
                
            }
        }
        
        if ( chosenPreviewView )
            chosenPreviewView->draw();
        
        if ( uploadMessage != "" ) {
            drawUploadMessage( uploadMessage );
        }
    }

    
    if ( whiteFlashTween.isRunning() ) {
        ofSetColor( 255, 255, 255, (int)(255.0 * whiteFlashTween.getTarget( 0 )) );
        ofRect( 0, 0, ofGetWidth(), ofGetHeight() );
    }
    
    if ( emailView ) {
        emailView->draw();
        drawInstructions( "ENTER YOUR EMAIL OR LEAVE BLANK AND HIT ENTER" );
    }
    
    drawOldPreviews();
    
    if ( isUploading ) {
//        ofSetColor( 255, 255, 255, 100);
//        ofRect(0, 0, ofGetWidth() * [helper uploadProgress], ofGetHeight() );
        
        drawProgressBar();
    }
    
}

void testApp::drawNumberForPreviewView( int num, PreviewView *pv ) {
    float scaleAmt = pv->width / pv->widthEnd;
    float xOffset, yOffset;
    if ( num == 1 ) {
        xOffset = 1.0;
        yOffset = 1.0;
    }
    else if ( num == 2 ) {
        xOffset = -1.0;
        yOffset = 1.0;
    }
    else if ( num == 3 ) {
        xOffset = 1.0;
        yOffset = -1.0;
    }
    else if ( num == 4 ) {
        xOffset = -1.0;
        yOffset = -1.0;
    }
    
    float xSafe = 20.0;
    float ySafe = 20.0;
//    ofRectangle numRect = tradeGothic.getStringBoundingBox( ofToString( (num==1?3:num) ), 0, 0 );
    ofRectangle numRect = tradeGothic.getStringBoundingBox( ofToString( 4 ), 0, 0 );
    
    float xTranslate = pv->centerX + ( pv->width / ( 2.0 * xOffset ) ) - ( xOffset > 0 ? numRect.width + numRect.x + xSafe : -xSafe );
    float yTranslate = pv->centerY + ( pv->height / ( 2.0 * yOffset ) ) - ( yOffset > 0 ? numRect.height + numRect.y + ySafe : -(ySafe - numRect.y) );
    
    // first draw black shadow knockout
    ofSetColor( 0, 0, 0 );
    ofPushMatrix();
    ofTranslate( xTranslate + 5, yTranslate + 5 );
    ofScale( scaleAmt, scaleAmt );
    tradeGothic.drawString( ofToString( num ), 0, 0 );
    ofPopMatrix();
    
    // then draw the white text
    ofSetColor( 255, 255, 255 );
    ofPushMatrix();
    ofTranslate( xTranslate, yTranslate );
    ofScale( scaleAmt, scaleAmt );
    tradeGothic.drawString( ofToString( num ), 0, 0 );
    ofPopMatrix();
}

void testApp::drawOldPreviews() {
    
    if ( oldPreviews.size() == 0 ) return;
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float laneWidth = ( ofGetWidth() - aspectWidth ) / 2.0;
    float xOffset = laneWidth > 100 ? (laneWidth - 100.0) / 2.0 : 10;
    float rightLaneOffset = laneWidth + aspectWidth + xOffset;
    
    // left lane
    int count = 0;
    float yPos = 0;
    do {
        yPos = count * 100 + xOffset;
        oldPreviews[count % oldPreviews.size()]->draw( xOffset, yPos, 100, 76 );
        count++;
    } while ( yPos < ofGetHeight() );
    
    // right lane
    count = 0;
    yPos = 0;
    do {
        yPos = count * 100 + xOffset;
        oldPreviews[ (count+oldPreviews.size()/2) % oldPreviews.size()]->draw( rightLaneOffset, yPos, 100, 76 );
        count++;
    } while ( yPos < ofGetHeight() );
}

void testApp::drawInstructions( string instructions ) {
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    
    ofRectangle instructionsRect = tradeGothicSmall.getStringBoundingBox( instructions, 0, 0 );
    float scaleFactor = (aspectWidth * .75) / (float)instructionsRect.width;
    instructionsRect.x *= scaleFactor;  instructionsRect.y *= scaleFactor;
    instructionsRect.width *= scaleFactor;  instructionsRect.height *= scaleFactor;
    
    ofSetColor( 0, 0, 0 );
    ofPushMatrix();
    ofTranslate( ( ofGetWidth() - xOffset - instructionsRect.width ) - instructionsRect.x, ofGetHeight() - instructionsRect.height - instructionsRect.y );
    ofScale( scaleFactor, scaleFactor );
    tradeGothicSmall.drawString( instructions, 0, 0 );
    ofPopMatrix();
    
    ofSetColor( 255, 255, 255 );
    ofPushMatrix();
    ofTranslate( ( ofGetWidth() - xOffset - instructionsRect.width ) - instructionsRect.x - 5, ofGetHeight() - instructionsRect.height - instructionsRect.y - 5 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothicSmall.drawString( instructions, 0, 0 );
    ofPopMatrix();
}

void testApp::drawUploadMessage( string message ) {
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    float targetWidth = aspectWidth * .7;
    
    
    ofRectangle rect = tradeGothic.getStringBoundingBox( message, 0, 0 );
    float scaleFactor = targetWidth / (float)rect.width;
    rect.x *= scaleFactor;  rect.y *= scaleFactor;
    rect.width *= scaleFactor;  rect.height *= scaleFactor;
    
    float bgWidth = aspectWidth * .75;
//    float bgHeight = ( bgWidth / (float)rect.width ) * (float)rect.height;
    float bgHeight = rect.height * 1.5;
    ofSetColor( 226, 31, 42 );
    ofRect( xOffset + (aspectWidth-bgWidth)*.5, ofGetHeight() * .2 - (bgHeight-rect.height)*.5, bgWidth, bgHeight );
    
    
    ofSetColor( 0, 0, 0 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x, -rect.y + ofGetHeight() * .2 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawString( message, 0, 0 );
    ofPopMatrix();
    
    ofSetColor( 255, 255, 255 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x - 5, -rect.y - 5 + ofGetHeight() * .2 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawString( message, 0, 0 );
    ofPopMatrix();
}

void testApp::drawDanceMessage( string message ) {
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    float targetWidth = aspectWidth * .75;
    
    
    ofRectangle rect = tradeGothic.getStringBoundingBox( message, 0, 0 );
    float scaleFactor = targetWidth / (float)rect.width;
    rect.x *= scaleFactor;  rect.y *= scaleFactor;
    rect.width *= scaleFactor;  rect.height *= scaleFactor;
    
    ofSetColor( 0, 0, 0 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x, -rect.y + ofGetHeight() * .05 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawString( message, 0, 0 );
    ofPopMatrix();
    
    ofSetColor( 255, 255, 255 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x - 5, -rect.y - 5 + ofGetHeight() * .05 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawString( message, 0, 0 );
    ofPopMatrix();
}

void testApp::drawCountdownNumber( int num ) {
    cout << "countdown: " << num << endl;
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    
    ofRectangle rect = tradeGothic.getStringBoundingBox( "4", 0, 0 );
    float scaleFactor = ofGetHeight() * .6 / (float)rect.height;
    rect.x *= scaleFactor;  rect.y *= scaleFactor;
    rect.width *= scaleFactor;  rect.height *= scaleFactor;
    
    ofSetColor( 0, 0, 0 );
    ofPushMatrix();
    ofTranslate( ofGetWidth()/2 - rect.x - rect.width/2.0, -rect.y + (ofGetHeight()-rect.height) * .5 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawStringAsShapes( ofToString( num ), 2.5, 2.5 );
    ofPopMatrix();
    
    ofSetColor( 255, 255, 255 );
    ofPushMatrix();
    ofTranslate( ofGetWidth()/2 - rect.x - rect.width/2.0, -rect.y + (ofGetHeight()-rect.height) * .5 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawStringAsShapes( ofToString( num ), -2.5, -2.5 );
    ofPopMatrix();
}

void testApp::drawProgressBar() {
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    float targetWidth = aspectWidth * .75;
    float inset = 4.0;
    
    string message = "UPLOADING...";
    ofRectangle rect = tradeGothic.getStringBoundingBox( message, 0, 0 );
    float scaleFactor = targetWidth / (float)rect.width;
    rect.x *= scaleFactor;  rect.y *= scaleFactor;
    rect.width *= scaleFactor;  rect.height *= scaleFactor;
    
    float textY = -rect.y + ofGetHeight() * .2;
    
    ofSetColor( 0, 0, 0 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x, textY );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawString( message, 0, 0 );
    ofPopMatrix();
    
    ofSetColor( 255, 255, 255 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x - 5, textY - 5 );
    ofScale( scaleFactor, scaleFactor );
    tradeGothic.drawString( message, 0, 0 );
    ofPopMatrix();
    
    
    ofSetColor( 0, 0, 0 );
    ofRect( xOffset + (aspectWidth-targetWidth)*.5, textY + rect.height * .1, targetWidth, ofGetHeight() * .2 );
    ofSetColor( 255, 255, 255 );
    ofRect( xOffset + (aspectWidth-targetWidth)*.5 + 4, textY + rect.height * .1 + 4, (targetWidth - 8) * [helper uploadProgress], ofGetHeight() * .2 - 8 );
}

void testApp::emailAddressEntered( string & emailAddress ) {
    
    if ( emailAddress == "" ) {
        cout << "no email address was entered" << endl;
        chosenPreviewView->startFadeOut();
//        destroyPreview();
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
    
    if ( previewViews.size() == 1 ) {
        previewViews[0]->startSizeTween( 0, ofGetWidth()/2, 0, ofGetHeight()/2, 400, 0 );
        previewViews[0]->setCenter( ofGetWidth()/2, ofGetHeight()/2 );
    }
    else if ( previewViews.size() == 4 ) {
        
        float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
        float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
        
        int previewWidth = aspectWidth / 2;
        int previewHeight = ofGetHeight() / 2;
        
        previewViews[0]->startSizeTween( 0, previewWidth, 0, previewHeight, 400, 0 );
        previewViews[0]->setCenter( previewWidth/2.0 + xOffset, previewHeight/2.0 );
        
        previewViews[1]->startSizeTween( 0, previewWidth, 0, previewHeight, 400, 100 );
        previewViews[1]->setCenter( previewWidth + previewWidth/2.0 + xOffset, previewHeight/2.0 );
        
        previewViews[2]->startSizeTween( 0, previewWidth, 0, previewHeight, 400, 200 );
        previewViews[2]->setCenter( previewWidth/2.0 + xOffset, previewHeight + previewHeight/2.0 );
        
        previewViews[3]->startSizeTween( 0, previewWidth, 0, previewHeight, 400, 300 );
        previewViews[3]->setCenter( previewWidth + previewWidth/2.0 + xOffset, previewHeight + previewHeight/2.0 );
        
    }
//    previewView = new PreviewView();
//    previewView->init( frames[0], captureInterval );
}

void testApp::captureFrame() {
    
    if ( frames[currentCaptureIndex].size() % FRAMES_PER_BEAT == 0 )
        whiteFlashTween.setParameters( easingQuad, ofxTween::easeOut, 1, 0, WHITE_FLASH, 0 );
    
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
	
    [helper startVideoUploadRequest:[NSString stringWithCString:fileNameMinusExtension.c_str() encoding:NSUTF8StringEncoding] withNumFrames:BEATS_PER_CAPTURE*FRAMES_PER_BEAT fromCity:[NSString stringWithCString:CS.getCity().c_str() encoding:NSUTF8StringEncoding]];
    
    
	string removeCommand = "rm -r " + fileNameMinusExtension + "mov";
    system(removeCommand.c_str());
    
	string removeCommandSmall = "rm -r " + fileNameMinusExtension + "s.mov";
    system(removeCommandSmall.c_str());
    
    isUploading = true;
    
    chosenPreviewView = previewViews[ chosenCaptureIndex ];
    ofAddListener( chosenPreviewView->fadeOutTween.end_E, this, &testApp::chosenPreviewFadedOut );
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    
    chosenPreviewView->startSizeTween( chosenPreviewView->widthEnd, aspectWidth, chosenPreviewView->height, ofGetHeight(), 500, 0 );
    chosenPreviewView->startCenterTween( chosenPreviewView->centerX, aspectWidth/2 + xOffset, chosenPreviewView->centerY, ofGetHeight()/2, 500, 0 ); 
}

void testApp::chosenPreviewFadedOut( int & theId ) {
    destroyPreview();
}

void testApp::destroyPreview() {
    
    bool saveChosenPreview = false;
    // if we're saving old previews, do it
    if ( NUM_OLD_PREVIEWS > 0  && chosenPreviewView ) {
        // if we're at the limit, erase an old preview
        if ( oldPreviews.size() == NUM_OLD_PREVIEWS ) {
            PreviewView *pv = *(oldPreviews.begin() );
            oldPreviews.erase( oldPreviews.begin() );
            delete pv;
        }
        
        oldPreviews.push_back( chosenPreviewView );
        saveChosenPreview = true;
    }
    
    
    if ( previewViews.size() > 0 ) {
        for ( int i=0; i<previewViews.size(); i++ ) {
            PreviewView *pv = previewViews[i];
            if ( saveChosenPreview && pv == chosenPreviewView ) {
                
            }
            else
                delete pv;
        }
        previewViews.clear();
    }
    
    if ( chosenPreviewView )
        chosenPreviewView = 0;
    
    if ( emailView ) {
        ofRemoveListener( emailView->emailAddressEnteredEvent, this, &testApp::emailAddressEntered );
        ofRemoveListener( emailView->emailAddressSkippedEvent, this, &testApp::emailAddressEntered );
        delete emailView;
        emailView = 0;
    }
    
    for ( int i = 0; i < NUM_CAPTURES; i++ ) {
        for ( int j = 0; j < frames[i].size(); j++ ) {
            ofImage *img = frames[i][j];
//            delete img;
        }
        frames[i].clear();
    }
    uploadMessage = "";
    tapView->reset();
    
}

void testApp::keyPressed(int key){
    
	if (CS.bDone == false){
		CS.keyPressed(key);
		return;
	}
	
    if ( emailView ) return;
    
    if ( key == 'f' || key == 'F' ) {
        ofToggleFullscreen();
    }
    
    // if the preview view is active, we handle things here differently
    if ( previewViews.size() > 0 && !isUploading ) {
        if ( key == 'x' || key == 'X' ) {
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
        else if ( key == 'c' || key == 'C' ) {
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