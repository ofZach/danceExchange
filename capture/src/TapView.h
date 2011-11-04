#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include "ofxTween.h"

class TapView {
    
public:
    
    ofEvent<double> startCaptureEvent;
    
    int lastMillis;
    int lastTap;
    int numTaps;
    int countdownTicks;
    double average;
    
    bool isCountingDown;
    double countdownMillis;
    bool finishedCountdown;
    
    ofxTween tapScaleTweens[8];
    float tapScales[8];
    ofxEasingBounce easingBounce;
    ofxEasingBack easingBack;
    ofxEasingQuad easingQuad;
    
    
    vector<int> tapDiffs;
    
    ofImage circle;
    
    TapView();
    ~TapView();
    void init();
    void update();
    void draw();
    void tap();
    void beginCountdown( double countDelay, bool immediate );
    void reset();
};