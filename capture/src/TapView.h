#pragma once

#include "ofMain.h"
#include "ofEvents.h"

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
    
    
    vector<int> tapDiffs;
    
    ofImage circle;
    
    TapView();
    ~TapView();
    void init();
    void update();
    void draw();
    void tap();
    void reset();
};