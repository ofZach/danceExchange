#pragma once

#include "ofMain.h"

#include "typer.h"



class EmailView {
public:
    
    ofTrueTypeFont verdana;
    ofTrueTypeFont tradeGothic;
    string emailAddress;
    
    ofImage warningImage;
    
    ofEvent<string> emailAddressEnteredEvent;
    ofEvent<string> emailAddressSkippedEvent;
    
    EmailView();
    ~EmailView();
    
    void init();
    void update();
    void draw();
    void keyPressed( ofKeyEventArgs & args );
    
	typer utf8_helper;
	bool bDisregardKeys;
	
};