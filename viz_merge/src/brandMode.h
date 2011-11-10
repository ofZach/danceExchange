

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "Pointilist.h"
#include "DVManager.h"


class brandParticle {
	
public: 

	void setup(){
		
		origPt.set(ofRandom(0,ofGetWidth()), ofRandom(0,ofGetHeight()));
		origSize = 1;
		lockTarget = 0;
		speed = ofRandom(0.8, 1.3);
		diffT = ofRandom(-0.5, 0.5);
		timeAdder = 0;
		bLocking = false;
	}
	
	void calc(danceVideo * DV, ofRectangle rect){
		
		//const int FRAME_WIDTH = 100;
		//const int FRAME_HEIGHT = 76;
		dvPtr = DV;
		lock=rect;
		float w = 100*2;
		origWH.set(100*2, 76*2);
		
		origSize = ofRandom(0.1, 0.65);
		
		lockSize =  (float)lock.width / (float)w;
		//cout << lockSize <<endl;
		
		update();
	}
	
	void update(){
		
		
		origPt.x += speed;
		
		origPt.y += ofSignedNoise(origPt.x/1000.0, timeAdder + diffT);
		timeAdder += 0.01 * 1.0/60.0;
		
		if (origPt.x > ofGetWidth() + origWH.x/2){
			origPt.x = 0 - origWH.x/2;
		}
		
		if (origPt.y > ofGetHeight() + origWH.y/2){
			origPt.y = 0 - origWH.y/2;
		}
		
		if (origPt.y < 0 - origWH.y/2){
			origPt.y = ofGetHeight() + origWH.y/2;
		}
		
		
		
		
		if (bLocking){
			lockTarget = 1;
		} else {
			lockTarget = 0;
		}
		
		lockAmount = 0.94* lockAmount + 0.06 * lockTarget;
		
		lockPt.x = lock.x + lock.width/2;
		lockPt.y = lock.y + lock.height/2;
		
		mixPt = (1 - lockAmount) * origPt + (lockAmount) * lockPt;
		mixSize = (1 - lockAmount) * origSize + (lockAmount) * lockSize;
		
		///cout << origSize << " " << mixSize << endl;
	}
	
	void draw(){
		ofSetRectMode(OF_RECTMODE_CENTER);
		//dvPtr->draw(mixPt, origWH.x*mixSize, origWH.y*mixSize);
		ofSetRectMode(OF_RECTMODE_CORNER);
	}
	
	float diffT;
	bool bLocking;
	
	float lockAmount;
	float lockTarget;
	ofRectangle lock;
	ofPoint origWH;
	ofPoint origPt;
	float origSize;
	ofPoint lockPt;
	float lockSize;
	ofPoint mixPt;
	float mixSize;
	danceVideo * dvPtr;
	float speed;
	float timeAdder;
};



class brandMode {

public: 
	
	
	
	
	void setup();
	void loadSquareSettings();
	void update();
	void draw();
	
	void start();
	void end();
	
	DVManager * DVM;
	
	
	vector < ofRectangle > rectanglesPre;
	vector < ofRectangle > rectangles;
	vector < ofRectangle > rectanglesWithRandomness;
	
	vector < int > associations;
	vector < bool > bIsParticle;
	
	vector < brandParticle > particles;
	
	ofImage eagle;
	
	Pointilist                              *pointilist;
	
	ofRectangle								videoRect;
	ofRectangle								eagleRect;
	
	ofVideoPlayer							video;
	float energy;
	
	bool bHitPause;
	float pauseTime;
	bool bHadVideosAtStart;
};