#pragma once

#include "ofMain.h"

class WarningView {
public:
    
    ofTrueTypeFont font;
    string message;
    
    WarningView( ofTrueTypeFont & font, string warningMessage );
    ~WarningView();
    void init();
    void update();
    void draw();
};