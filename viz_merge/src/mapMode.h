#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "Pointilist.h"
#include "DVManager.h"

class tri {
	
public: 
	
	ofPoint pos;
	bool bIsCity;
	bool bIsSelectedCity;
	
	float size;
	float newSize;
	
	float energy;
	//ofPoint cityPt;
	float dist;
	float angle;
	

	
	void calculateForNewCity(ofPoint cityPt){
		ofPoint diff = (pos - cityPt);
		dist = (pos - cityPt).length();
		angle = atan2(diff.y, diff.x);
		//cout << dist << " " << angle << endl;
	}
	
	void draw(){
		float s2 = size / 2.0;
		
		float sizeOfTriangle = size;
		if (!bIsSelectedCity) sizeOfTriangle = (1-energy) * size + (energy) * (size*0.6);
		float ang = 0;
		if (!bIsSelectedCity) ang = energy * angle + (1-energy) * 0;
		
		
		//207,6,34
		
		float i = pos.x;
		float j = pos.y;
		if (bIsCity && !bIsSelectedCity) ofSetColor(207,6,34, 255 - 100*energy);
		else if (bIsCity && bIsSelectedCity) ofSetColor(207,6,34);
		else ofSetColor(80,80,80);
		ofFill();
		
		
		
		if (dist < 100){
			float pct = (dist/100.0);
			if (!bIsCity) ofSetColor(80,80,80, 255 * (1-energy) + energy * 255*pct);
		}
		
		ofPushMatrix();
		
		if (ang > PI) ang -= TWO_PI;
		if (ang < -PI) ang += TWO_PI;
		
		glTranslated(i, j, 0);
		glRotatef(ang*RAD_TO_DEG,0,0,1);
		//glTranslated(-sizeOfTriangle, -sizeOfTriangle, 0);
		
		ofTriangle(ofPoint(-s2,-s2), ofPoint(-s2,-s2+sizeOfTriangle), ofPoint(-s2+sizeOfTriangle*(sqrt(3)/2.0), -s2+sizeOfTriangle/2.0)); 
		ofPopMatrix();
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
	int whichCity;
	ofTrueTypeFont tradeGothic;
    
	
	
};