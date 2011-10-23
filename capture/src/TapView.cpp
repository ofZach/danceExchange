#include "TapView.h"


static int MAX_TAPS = 8;

TapView::TapView() {
    circle.loadImage( "circle.png" );
    reset();
    
}

void TapView::reset() {
    
    lastMillis = 0;
    numTaps = 0;
    lastTap = 0;
    average = 0;
    countdownTicks = 0;
    tapDiffs.clear();
    isCountingDown = false;
}

void TapView::update() {
    
    int currentMillis = ofGetElapsedTimeMillis();
    int deltaMillis = currentMillis - lastMillis;
    lastMillis = currentMillis;
    
    if ( !isCountingDown && numTaps > 0 && currentMillis - lastTap > 1000 ) {
        cout << "time out on taps" << std::endl;
        numTaps = 0;
        lastTap = 0;
    }
    
    if ( isCountingDown ) {
        countdownMillis += (double)deltaMillis;
        if ( countdownMillis > average ) {
            countdownMillis -= average;
            countdownTicks++;
            
            if ( countdownTicks == MAX_TAPS + 1 )
                ofNotifyEvent( startCaptureEvent, average, this );
        }
    }
}

void TapView::draw() {
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
        
        
        float theY = ofGetHeight() / 2.0;
        float theX = ofMap( i, 0, MAX_TAPS-1, 150, ofGetWidth()-150 );
        
        circle.draw( theX - circle.width / 2.0, theY - circle.height / 2.0 );
        
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