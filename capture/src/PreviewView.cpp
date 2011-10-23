#include "PreviewView.h"

void PreviewView::init( vector<ofImage*> &frames, double interval ) {
    this->frames = frames;
    lastMillis = ofGetElapsedTimeMillis();
    frameIndex = 0;
    intervalMillis = 0;
    this->interval = interval;
}

void PreviewView::update() {
    double currentMillis = ofGetElapsedTimeMillis();
    double deltaMillis = currentMillis - lastMillis;
    lastMillis = currentMillis;
    
    intervalMillis += deltaMillis;
    if ( intervalMillis > interval ) {
        intervalMillis -= interval;
        if ( ++frameIndex == frames.size() )
            frameIndex = 0;
    }
}

void PreviewView::draw( int theX, int theY, int theWidth, int theHeight ) {
    ofImage *img = frames[frameIndex];
    ofSetColor( 255, 255, 255 );
    img->draw( theX, theY, theWidth, theHeight );
}