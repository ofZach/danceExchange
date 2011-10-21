#pragma once

#include "ofMain.h"
#import <Foundation/Foundation.h>
#import "Helper.h"
#include "PreviewView.h"
#include "TapView.h"
#include "EmailView.h"
#include "ofxQtVideoSaver.h"

class testApp : public ofBaseApp{

	public:
        Helper *helper;
        PreviewView *previewView;
        TapView *tapView;
        EmailView *emailView;
        ofxQtVideoSaver videoSaver;
    
        ofTrueTypeFont verdana;
    
        double lastMillis;
    
        int camWidth, camHeight;
        ofVideoGrabber grabber;
    
        bool isCapturing;
        double captureInterval;
        double captureMillis;
        void captureFrame();
    
        bool isUploading;
        string uploadMessage;
    
        bool isEmailing;
    
        vector<ofImage*> frames;
        bool showFrames;
    
		void setup();
		void update();
		void draw();
        void saveVideoFiles();
		void keyPressed  (int key);
		void windowResized(int w, int h);
        void destroyPreview();
		
        void emailAddressEntered( string & emailAddress );
        void startCapturing(double & d);
        void startPreviewing();
    
        double gifStartTime;
        void onGifSaved( string & filename );
};
