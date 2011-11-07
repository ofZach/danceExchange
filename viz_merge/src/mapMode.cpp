

#include "mapMode.h"

// 

void mapMode::setup(){
	whichCity = 0;
	mapImg.loadImage("images/map.png");
	mapImg.setImageType(OF_IMAGE_GRAYSCALE);
	
	tradeGothic.loadFont( "TradeGothicLTStd-BdCn20.otf",  84 );
    
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


void mapMode::start(){
	
	int which = (int)ofRandom(0,cityPts.size()+1) % cityPts.size();
	scalePt = cityPts[which];
	whichCity = which;
	
	
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
	
	
	offsetTarget =  scalePt - ofPoint(100,100);
	offsetPt.set(0,0,0);
	
	
	
	
	scale = 0;
	
	
}
void mapMode::end(){
			
}


void mapMode::update(){
	
	if (ofGetKeyPressed('k')){
		scale = scale * 0.95 + 1.0 * 0.05;
		offsetPt =offsetPt * 0.95 + offsetTarget * 0.05;
	} else {
		scale = scale * 0.95 + 0.0 * 0.05;		
		offsetPt = offsetPt * 0.95 + ofPoint(0,0,0) * 0.05;
	}
	
	for (int i = 0; i < triangles.size(); i++){
		triangles[i].energy = scale;
	}
	
}

void mapMode::draw(){
	
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
	
}

