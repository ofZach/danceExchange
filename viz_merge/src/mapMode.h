#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "Pointilist.h"
#include "DVManager.h"

class tri {
	
public: 
	
	ofPoint pos;
	bool bIsCity;
	float size;
	
	void draw(){
		float s2 = size / 2.0;
		float sizeOfTriangle = size;
		float i = pos.x;
		float j = pos.y;
		if (bIsCity) ofSetColor(255,0,255);
		else ofSetColor(80,80,80);
		ofFill();
		ofTriangle(ofPoint(i-s2,j-s2), ofPoint(i-s2,j-s2+sizeOfTriangle), ofPoint(i-s2+sizeOfTriangle*(sqrt(3)/2.0), j-s2+sizeOfTriangle/2.0)); 
		//ofEnableSmoothing();
		//ofNoFill();
		//ofTriangle(ofPoint(i-s2,j-s2), ofPoint(i-s2,j-s2+sizeOfTriangle), ofPoint(i-s2+sizeOfTriangle*(sqrt(3)/2.0), j-s2+sizeOfTriangle/2.0)); 
		
	}
	
};



class mapMode {

public: 
	
	void setup();
	void update();
	void draw();
	
	void start();
	void end();
	
	Pointilist                              *pointilist;
	DVManager								*DVM;
	
	ofPoint									offsetTarget;
	ofPoint									offsetPt;	// get the city to top left
	float									scale;
	ofPoint									scalePt;
	
	ofImage mapImg;
	vector < ofPoint > cityPts;
	vector < string > cityNames;
	vector  < tri > triangles;
	
	
};