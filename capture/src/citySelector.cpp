/*
 *  citySelector.cpp
 *  emptyExample
 *
 *  Created by zachary lieberman on 11/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "citySelector.h"


static const ofColor cyanPrint = ofColor::fromHex(0x00abec);
static const ofColor magentaPrint = ofColor::fromHex(0xec008c);
static const ofColor yellowPrint = ofColor::fromHex(0xffee00);

void drawHighlightString(string text, ofPoint position, ofColor background = ofColor::black, ofColor foreground = ofColor::white);
void drawHighlightString(string text, int x, int y, ofColor background = ofColor::black, ofColor foreground = ofColor::white);

void drawHighlightString(string text, ofPoint position, ofColor background, ofColor foreground) {
	drawHighlightString(text, position.x, position.y, background, foreground);
}

void drawHighlightString(string text, int x, int y, ofColor background, ofColor foreground) {
	ofPushStyle();
	int textWidth =  10 + text.length() * 8;
	ofSetColor(background);
	ofFill();
	ofRect(x - 5, y - 12, textWidth, 20);
	ofSetColor(foreground);
	ofDrawBitmapString(text, x, y);
	ofPopStyle();
}




void citySelector::saveCityXml(){

	XML.setValue("selectedCity", selectedCity);
	XML.saveFile("settings/citySettings.xml");
}

void citySelector::loadCityFromXML(){

	XML.loadFile("settings/citySettings.xml");
	
	int nCities = XML.getNumTags("city");
	for (int i = 0; i < nCities; i++){
		cityList.push_back(XML.getValue("city", "", i));
	}
	selectedCity = XML.getValue("selectedCity", -1);
						   
}

void citySelector::setup(){
	loadCityFromXML();
	//selectedCity = 2;
	//saveCityXml();
	bDone = false;
}

void citySelector::update(){
	ofBackground(127,127,127); // gray bg;
}

string citySelector::getCity(){
	if (selectedCity == -1){
			// how can this be?
		return "unkown";
	} else {
		return cityList[selectedCity];
	}
}




void citySelector::keyPressed(int key){

	if (key == OF_KEY_LEFT || key == OF_KEY_UP){
		selectedCity--;
		if (selectedCity < 0){
			selectedCity = 	cityList.size() - 1;
		}
	}
	
	if (key == OF_KEY_RIGHT || key == OF_KEY_DOWN){
		selectedCity++;
		selectedCity %= cityList.size();
	}
	
	if (key == OF_KEY_RETURN ){
		if (selectedCity != -1){
			saveCityXml();
			bDone = true;	
		}
	}
	
}

void citySelector::draw(){
	
	
	for (int i = 0; i < cityList.size(); i++){
		ofColor col = magentaPrint;
		
		if (i == selectedCity) drawHighlightString(cityList[i], 300, 100 + 20*i, col);
		else drawHighlightString(cityList[i], 300, 100 + 20*i);
	}
	
	drawHighlightString("please use arrow keys to choose your city",300,580, cyanPrint); 
	drawHighlightString("and hit enter / return when it's selected", 300,600, cyanPrint);
	
}



