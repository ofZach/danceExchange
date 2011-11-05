#include "TapView.h"


static int MAX_TAPS = 8;

static float minScale = 0.5;
static float maxScale = 1.0;

TapView::TapView() {
    circle.loadImage( "images/circle.png" );
    reset();
    
}

void TapView::reset() {
    
    lastMillis = 0;
    numTaps = 0;
    lastTap = 0;
    average = 0;
    countdownTicks = 0;
    finishedCountdown = false;
    tapDiffs.clear();
    isCountingDown = false;
}

void TapView::update() {
    
    int currentMillis = ofGetElapsedTimeMillis();
    int deltaMillis = currentMillis - lastMillis;
    lastMillis = currentMillis;
    
    if ( !isCountingDown && numTaps > 0 && currentMillis - lastTap > 1000 ) {
        cout << "time out on taps" << std::endl;
        for ( int i=0; i<MAX_TAPS; i++ ) {
            if ( tapScaleTweens[i].isRunning() || tapScaleTweens[i].getTarget(0) > 0 ) {
                tapScaleTweens[i].setParameters(easingQuad, ofxTween::easeInOut, tapScaleTweens[i].getTarget(0), 0.0, 300, i*30 );
            }
        }
        
        numTaps = 0;
        lastTap = 0;
    }
    
    for ( int i=0; i<MAX_TAPS; i++ ) {
        tapScaleTweens[i].update();
    }
    
    if ( isCountingDown ) {
        countdownMillis += (double)deltaMillis;
        if ( countdownMillis > average ) {
            countdownMillis -= average;
            countdownTicks++;
            
            if ( countdownTicks == MAX_TAPS + 1 ) {
                ofNotifyEvent( startCaptureEvent, average, this );
                isCountingDown = false;
                finishedCountdown = true;
            }
        }
    }
}

void TapView::draw() {
    
    if ( countdownTicks == MAX_TAPS + 1 )
        return;
    
    if ( isCountingDown && countdownTicks > 0 )
        return;
    
    for ( int i=0; i<MAX_TAPS; i++ ) {
        if ( numTaps == MAX_TAPS ) {
            if ( i < countdownTicks )
                ofSetColor( 255, 0, 0, 200 );
            else
                ofSetColor( 255, 255, 255, 200 );
        }
        else {
            if ( i < numTaps )
                ofSetColor( 255, 255, 255, 200 );
            else
                ofSetColor( 255, 255, 255, 100 );
        }
        
        
        float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
        float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
        
        
        float theY = ofGetHeight() / 2.0;
        float theX = ofMap( i, 0, MAX_TAPS-1, 150, aspectWidth-150 );
        
        ofPushMatrix();
        ofTranslate( theX + xOffset, theY );
        float theScale = ofMap(tapScaleTweens[i].getTarget(0), 0, 1, minScale, maxScale);
        ofScale( theScale, theScale );
//        circle.draw( theX - circle.width / 2.0 + xOffset, theY - circle.height / 2.0 );
        circle.draw( -circle.width / 2.0, -circle.height / 2.0 );
        ofPopMatrix();
        
    }
}

void TapView::beginCountdown( double countDelay, bool immediate ) {
    
    average = countDelay;
    countdownTicks = ( immediate ? 1 : 0 );
    countdownMillis = 0;
    isCountingDown = true;
    numTaps = MAX_TAPS;
}

void TapView::tap() {
    if ( isCountingDown )
        return;
    
    int now = ofGetElapsedTimeMillis();
    int diff = now - lastTap;
    if ( lastTap != 0 )
        tapDiffs.push_back( diff );
    
    cout << "numTaps: " << numTaps << endl;
    tapScaleTweens[numTaps].setParameters( easingBounce, ofxTween::easeOut, 0.0, 1.0, 400, 0 );
    numTaps++;
    if ( numTaps == MAX_TAPS ) {
        // start the countdown
        lastTap = 0;
        double total = 0;
        for ( int i=0; i<tapDiffs.size(); i++ ) {
            total += tapDiffs[i];
        }
        beginCountdown( total / (double)tapDiffs.size(), false );
    }
    else {
        lastTap = now;
    }
}

TapView::~TapView() {
    
}