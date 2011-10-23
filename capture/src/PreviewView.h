#pragma once

#include "ofMain.h"

class PreviewView {
public:
    
    vector<ofImage*> frames;
    double interval;
    double lastMillis;
    double intervalMillis;
    int frameIndex;
    
    void init( vector<ofImage*> &frames, double interval ); 
    void update();
    void draw( int theX = 0, int theY = 0, int theWidth = ofGetWidth(), int theHeight = ofGetHeight() );
};