#include "PreviewView.h"

void PreviewView::init( vector<ofImage*> &frames, double interval ) {
    this->frames = frames;
    lastMillis = ofGetElapsedTimeMillis();
    frameIndex = 0;
    intervalMillis = 0;
    this->interval = interval;
    alpha = 1;
}

void PreviewView::update() {
    
    if ( sizeTween.isRunning() ) {
        sizeTween.update();
        width = ofMap( sizeTween.getTarget( 0 ), 0, 1, widthStart, widthEnd );
        height = ofMap( sizeTween.getTarget( 0 ), 0, 1, heightStart, heightEnd );
    }
    
    if ( centerTween.isRunning() ) {
        centerTween.update();
        centerX = ofMap( centerTween.getTarget( 0 ), 0, 1, centerXStart, centerXEnd );
        centerY = ofMap( centerTween.getTarget( 0 ), 0, 1, centerYStart, centerYEnd );
    }
    
    if ( fadeOutTween.isRunning() ) {
        alpha = fadeOutTween.update();
    }
    
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

void PreviewView::setCenter( float cX, float cY ) {
    centerX = cX;
    centerY = cY;
}

void PreviewView::draw() {
    ofImage *img = frames[frameIndex];
    ofSetColor( 255, 255, 255, (int)(255*alpha) );
//    img->draw( theX, theY, theWidth, theHeight );
//    img->draw( centerX, centerY, width, height );
    img->draw( centerX - width/2.0, centerY - height/2.0, width, height );
}

void PreviewView::draw( float x, float y, float w, float h ) {
    ofImage *img = frames[frameIndex];
    ofSetColor( 255, 255, 255 );
    img->draw( x, y, w, h );
}

void PreviewView::startFadeOut() {
    
    fadeOutTween.setParameters( easingQuad, ofxTween::easeInOut, 1, 0, 500, 0 );
    
}

void PreviewView::startSizeTween( float wStart, float wEnd, float hStart, float hEnd, int duration, int delay ) {
    
    widthStart  = wStart;
    widthEnd    = wEnd;
    heightStart = hStart;
    heightEnd   = hEnd;
    
    sizeTween.setParameters( easingQuad, ofxTween::easeInOut, 0, 1, duration, delay );
}

void PreviewView::startCenterTween( float cxStart, float cxEnd, float cyStart, float cyEnd, int duration, int delay ) {
    centerXStart    = cxStart;
    centerXEnd      = cxEnd;
    centerYStart    = cyStart;
    centerYEnd      = cyEnd;
    
    centerTween.setParameters( easingQuad, ofxTween::easeInOut, 0, 1, duration, 0 );
}

PreviewView::~PreviewView() {
    // destructor
    cout << "~PreviewView()" << endl;
    for ( int i=0; i<frames.size(); i++ ) {
        ofImage *img = frames[i];
        delete img;
    }
}