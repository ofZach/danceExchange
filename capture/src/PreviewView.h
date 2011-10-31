#pragma once

#include "ofMain.h"
#include "ofxTween.h"

class PreviewView {
public:
    
    vector<ofImage*> frames;
    double interval;
    double lastMillis;
    double intervalMillis;
    int frameIndex;
    
    float centerX, centerY;
    float centerXStart, centerXEnd, centerYStart, centerYEnd;
    
    float width, height;
    float widthStart, widthEnd, heightStart, heightEnd;
    
    ofxTween sizeTween;
    ofxTween centerTween;
    ofxTween fadeOutTween;
    ofxEasingQuad easingQuad;
    
    float alpha;
    
    ~PreviewView();
    void init( vector<ofImage*> &frames, double interval ); 
    void update();
    void setCenter( float cX, float cY );
    void startFadeOut();
    void startSizeTween( float wStart, float wEnd, float hStart, float hEnd, int duration, int delay );
    void startCenterTween( float cxStart, float cxEnd, float cyStart, float cyEnd, int duration, int delay );
    void draw();
    void draw( float x, float y, float w, float h );
};