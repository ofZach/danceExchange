

#include "mapMode.h"

// 

void mapMode::setup(){
	whichCity = 0;
	bWasZoomingThisFrame = false;
	mapImg.loadImage("images/map.png");
	mapImg.setImageType(OF_IMAGE_GRAYSCALE);
	
	tradeGothic.loadFont( "TradeGothicLTStd-BdCn20.otf",  84 );
    
	
	textEnergy = 0;
	
	ofFile file;
	file.open("info/cityList.txt");
	ofBuffer temp = file.readToBuffer();
	int line = 0;
	while( temp.isLastLine() == false){
		line++;
		temp.getNextLine();
	}
	temp.resetLineReader();
	
	for (int i = 0; i < line; i++){
		
		string myLine = temp.getNextLine();
		vector < string > split = ofSplitString(myLine, ",", true, true);
		cityNames.push_back(split[0]);
		cityPts.push_back( ofPoint( ofToInt(split[1]), ofToInt(split[2])) );
		//cityNames.push_back(split[0]);
		
	}
	
	
	vector < ofPoint > pts;
	pts.push_back(ofPoint(256,343.5));
	pts.push_back(ofPoint(602.5,114.5));
	pts.push_back(ofPoint(546.5,125.5));
	pts.push_back(ofPoint(642.5,128.5));
	pts.push_back(ofPoint(591.5,129.5));
	pts.push_back(ofPoint(581.5,135.5));
	pts.push_back(ofPoint(574.5,144.5));
	pts.push_back(ofPoint(664.5,162.5));
	pts.push_back(ofPoint(613.5,174.5));
	pts.push_back(ofPoint(651.5,181.5));
	pts.push_back(ofPoint(1066.5,198.5));
	pts.push_back(ofPoint(1111.5,201.5));
	pts.push_back(ofPoint(694.5,207.5));
	pts.push_back(ofPoint(699.5,226.5));
	pts.push_back(ofPoint(787.5,252.5));
	pts.push_back(ofPoint(998.5,271.5));
	pts.push_back(ofPoint(270.5,277.5));
	pts.push_back(ofPoint(977.5,299.5));
	pts.push_back(ofPoint(307.5,317.5));
	pts.push_back(ofPoint(595.5,322.5));
	pts.push_back(ofPoint(293.5,323.5));
	pts.push_back(ofPoint(234.5,324.5));
	pts.push_back(ofPoint(233.5,361.5));
	pts.push_back(ofPoint(717.5,363.5));
	pts.push_back(ofPoint(368.5,471.5));
	pts.push_back(ofPoint(328.5,474.5));
	pts.push_back(ofPoint(668.5,474.5));
	pts.push_back(ofPoint(638.5,502.5));
	pts.push_back(ofPoint(1130.5,534.5));
	pts.push_back(ofPoint(1192.5,573.5));
	
	
	int sizeOfTriangle = 9;
	
	triangles.clear();
	
	ofPixels pix = mapImg.getPixelsRef();
	for (int i = 0; i < pix.getWidth(); i+= sizeOfTriangle){
		for (int j = 0; j < pix.getHeight(); j+= sizeOfTriangle){
			
			int colorAvg = 0;
			for (int k = 0; k < sizeOfTriangle; k++){
				for (int l = 0; l < sizeOfTriangle; l++){
					colorAvg += pix.getColor(i+k, j+l).r;
				}
			}
			colorAvg /= (float)(sizeOfTriangle*sizeOfTriangle);
			
			if (colorAvg > 38){
				ofSetColor(127,127,127);
				//cout << mouseX <<endl;
				//
				tri triTemp;
				triTemp.pos.set(i+sizeOfTriangle/2.0, j+sizeOfTriangle/2.0);
				triTemp.size = sizeOfTriangle;
				triTemp.bIsCity = false;
				triangles.push_back(triTemp);
				
				//ofTriangle(ofPoint(i,j), ofPoint(i,j+sizeOfTriangle), ofPoint(i+sizeOfTriangle*(sqrt(3)/2.0), j+sizeOfTriangle/2.0)); 
				//ofRect(i,j,sizeOfTriangle,sizeOfTriangle);
			}
			
		}
	}
	
	for (int  i = 0; i < pts.size(); i++){
		tri triTemp;
		float ii = pts[i].x;
		float jj = pts[i].y;
		triTemp.pos.set(ii+sizeOfTriangle/2.0, jj+sizeOfTriangle/2.0);
		triTemp.size = sizeOfTriangle;
		triTemp.bIsCity = true;
		triangles.push_back(triTemp);
	}
	
	for (int i = 0; i < cityPts.size(); i++){
		tri triTemp;
		float ii = cityPts[i].x;
		float jj = cityPts[i].y;
		
		triTemp.pos.set(ii+sizeOfTriangle/2.0, jj+sizeOfTriangle/2.0);
		triTemp.size = sizeOfTriangle;
		triTemp.bIsCity = true;
		triangles.push_back(triTemp);
	}
}


int mapMode::pickAGoodRandom(){

	//vector < int > randoms;
	int whichVid;
	
	if (DVM->danceVideos.size() > 20){
	
		bool bTryPicking = true;
		while (bTryPicking){
			whichVid = (int)ofRandom(0,DVM->danceVideos.size()+1) % DVM->danceVideos.size();
			bool bAllGood = true;
			for (int i = 0; i < randoms.size(); i++){
				if (randoms[i] == whichVid){
					bAllGood = false;
				}
			}
			if (bAllGood == true){
				bTryPicking = false;
			}
		}
		randoms.push_back(whichVid);
		if (randoms.size() > 10){
			randoms.erase(randoms.begin());	
		}
		
	} else {
		
		whichVid = (int)ofRandom(0,DVM->danceVideos.size()+1) % DVM->danceVideos.size();
	}
	
	
	
	
	return whichVid;
	
}

void mapMode::pickRandom(){
	// try to pick a random video, or city:
	
	for (int i = 0; i < DVM->danceVideos.size(); i++){
		DVM->danceVideos[i]->isInUse = false;
	}
	
	
	
	dvForZoom = NULL;
	int which  = 0;
	
	if (DVM->danceVideos.size() > 0){
		int whichVid = pickAGoodRandom();
		string city = DVM->danceVideos[whichVid]->city;
		
		DVM->danceVideos[whichVid]->isInUse = true;
		
		//cout << "selected video " << whichVid << " w/ city " << DVM->danceVideos[whichVid]->city << endl;
		for (int i = 0; i < cityNames.size(); i++){
			
			bool bAnySubStringMatches = false;
			if (ofIsStringInString(cityNames[i], city) ||
				ofIsStringInString(city, cityNames[i])){
				bAnySubStringMatches = true;
			}
			
			string citya = cityNames[i];
			string cityb = city;
			for (int j=0; j<citya.length(); ++j){
				citya[j]=tolower(citya[j]);
			} 
			for (int j=0; j<cityb.length(); ++j){
				cityb[j]=tolower(cityb[j]);
			}
			
			if (ofIsStringInString(citya, cityb) ||
				ofIsStringInString(cityb, citya) || 
				(citya.compare( cityb )) == 0){
				bAnySubStringMatches = true;
			}
			
			
			if ((cityNames[i].compare( city )) == 0 || bAnySubStringMatches){
				cout << cityNames[i] << " -- " << city << endl;
				which = i;
				whichCity = which;
				scalePt = cityPts[which];
				
				dvForZoom =  DVM->danceVideos[whichVid];
			}
			//cout <<  res << endl;
		}
		
		//which = 0;
	} else {
		which = (int)ofRandom(0,cityPts.size()+1) % cityPts.size();
		scalePt = cityPts[which];
		whichCity = which;
	}
	
	cout << which << endl;
	for (int i = 0; i < triangles.size(); i++){
		triangles[i].calculateForNewCity(scalePt);
		triangles[i].bIsSelectedCity = false;
		if (i >= (triangles.size()-cityPts.size())){
			
			int who = i - ((triangles.size()-cityPts.size()));
			if (who == which){
				triangles[i].bIsSelectedCity = true;
				
			}
		}
	}
	
	// 880x660
	offsetTarget =  scalePt - ofPoint(100,-mapRect.y);
	offsetPt.set(0,0,0);
	
	//cout << mapRect.x << " " << mapRect.y <<endl;
	
	//for (int i = 0; i < DVM->danceVideos.size(); i++){
	//		cout <<  DVM->danceVideos[i]->city << endl;
	//	}
	
	
	scale = 0;
}



void mapMode::start(){
	
	randoms.clear();
	
	textEnergy = 1;
	
	float ratioV = 12.0 / 6.2;  // 1200x618
	float ratioS = (float)ofGetWidth() / (float)ofGetHeight();
	if (ratioS > ratioV){
		// wide screen	
		mapRect.y = 0;
		mapRect.height = ofGetHeight();
		mapRect.width = ofGetHeight() * (ratioV);
		mapRect.x = (ofGetWidth() - mapRect.width)/2;
	} else {
		// tall screen
		mapRect.x = 0;
		mapRect.width = ofGetWidth();
		mapRect.height = ofGetWidth() * (1/ratioV);
		mapRect.y = (ofGetHeight() - mapRect.height)/2;
	}
	
	pickRandom();
	
	startTime = ofGetElapsedTimef();
	
	
}
void mapMode::end(){
			
}


void mapMode::update(){
	
	if (DVM->danceVideos.size() == 0){
		return;
	}
	
	bool bPressed = false;
	
	float fmod;
	fmod = ofGetElapsedTimef() - startTime;
	while (fmod > 6) {	
		fmod -= 6;
	}
	
	if (fmod > 2){
		bPressed = true;
		scale = scale * 0.95 + 1.0 * 0.05;
		offsetPt =offsetPt * 0.95 + offsetTarget * 0.05;
	} else {
		scale = scale * 0.95 + 0.0 * 0.05;		
		offsetPt = offsetPt * 0.95 + ofPoint(0,0,0) * 0.05;
	}
	
	for (int i = 0; i < triangles.size(); i++){
		triangles[i].energy = scale;
	}
	if (bPressed == true && bWasZoomingThisFrame == false){
		pickRandom();
	}
	
	bWasZoomingThisFrame = bPressed;
	
	if (bPressed == true) textEnergy *= 0.90f;
}

void mapMode::draw(){
	
	if (DVM->danceVideos.size() == 0){
		return;
	}
	
	//ofRect(mapRect.x, mapRect.y, mapRect.width, mapRect.height);
	glPushMatrix();
	glTranslated(mapRect.x, mapRect.y, 0); 
	float scalemap = mapRect.width / 1200;
	glScalef(scalemap, scalemap, 1);
	
	
	glPushMatrix();
	glTranslated(scalePt.x - offsetPt.x, scalePt.y - offsetPt.y, 0);
	glScalef(1 + scale*10, 1 + scale*10, 1);
	glTranslated(-scalePt.x, -scalePt.y, 0);
	
	for (int i = 0; i < triangles.size(); i++){
		triangles[i].draw();
	}
	
	//tradeGothic.drawString( cityNames[whichCity], scalePt.x, scalePt.y );
    
	
	glPopMatrix();
	
	
	
	//207, 6, 34
	
	glPushMatrix();
	ofSetColor(255,255,255, powf(scale,3)*255);
	glTranslated(scalePt.x - offsetPt.x, scalePt.y - offsetPt.y, 0);
	//glScalef(1 + scale*10, 1 + scale*10, 1);
	glTranslated(-scalePt.x, -scalePt.y, 0);
	
	tradeGothic.drawString( cityNames[whichCity], scalePt.x + 160 * scale, scalePt.y + 80 * scale);
    
	
	glPopMatrix();
	
	
	glPushMatrix();
	glTranslated(scalePt.x - offsetPt.x, scalePt.y - offsetPt.y, 0);
	glScalef(1 + scale*10, 1 + scale*10, 1);
	glTranslated(-scalePt.x, -scalePt.y, 0);
	
	if (dvForZoom != NULL){
		//dvForZoom
		
		pointilist->addPoint( scalePt.x + (10 + 70 * scale)/2, scalePt.y + (10 + 70 * scale)/2, 0,
							 10 + 60 * scale,
							 1,1,1,scale*0.95,
							 dvForZoom->texIndex, 0,  dvForZoom->firstFrame +  dvForZoom->currentFrame
							 );
		pointilist->draw();
	}
	
	//tradeGothic.drawString( cityNames[whichCity], scalePt.x, scalePt.y );
    
	
	glPopMatrix();
	
	ofEnableAlphaBlending();
	
	
	//ofNoFill();
	//ofSetColor(255,0,255);
	
	
	glPopMatrix();
	
	
	if (textEnergy > 0.01){
	ofSetColor(255,255,255,255*textEnergy);
	tradeGothic.drawString("50 COUNTRIES\nEXCHANGING NIGHTLIFE", 50, 300);
	}
}


/*
 video is 4/3
 
 880x660
 
 
 11 * 80
 glTranslated(scalePt.x - offsetPt.x, scalePt.y - offsetPt.y, 0);
 glScalef(1 + scale*10, 1 + scale*10, 1);
 glTranslated(-scalePt.x, -scalePt.y, 0);
 
 if (dvForZoom != NULL){
 //dvForZoom
 
 pointilist->addPoint( scalePt.x + (10 + 70 * scale)/2, scalePt.y + (10 + 70 * scale)/2, 0,
 10 + 70 * scale,
*/