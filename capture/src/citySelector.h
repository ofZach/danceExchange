
#pragma once

#include "ofxXmlSettings.h"

class citySelector {

public: 
	
	void saveCityXml();
	void loadCityFromXML();
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	
	
	bool bCitySelected;
	
	ofTrueTypeFont font;
	
	string getCity();
	
	
	vector < string > cityList;
	int selectedCity;
	
	ofxXmlSettings XML;
	
	bool bDone;
	
};
