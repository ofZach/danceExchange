#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "Pointilist.h"
#include "DVManager.h"


class burstMode {

public: 
	
	void setup();
	void update();
	void draw();
	
	void start();
	void end();
	
	Pointilist                              *pointilist;
	DVManager								*DVM;
	
	int										nFrames;
	int										whichVideo;
	int										whichFrame;
	int										frameCount;
	float									speed;
	
	bool									bFirstFrame;
	float									lastTimeF;
	float									timeTotal;
	
};