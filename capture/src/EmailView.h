#pragma once

#include "ofMain.h"

class EmailView {
public:
    
    ofTrueTypeFont verdana;
    string emailAddress;
    
    ofEvent<string> emailAddressEnteredEvent;
    ofEvent<string> emailAddressSkippedEvent;
    
    EmailView();
    ~EmailView();
    
    void init();
    void update();
    void draw();
    void keyPressed( ofKeyEventArgs & args );
    
};