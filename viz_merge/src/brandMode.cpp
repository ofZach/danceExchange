

#include "brandMode.h"


void brandMode::setup(){
	loadSquareSettings();
	eagle.loadImage("images/eagle.png");
}


void brandMode::loadSquareSettings(){

	ofxXmlSettings XML;
	XML.loadFile("xml/squares.xml");
	
	int nTags = XML.getNumTags("square");
	for (int i = 0; i< nTags; i++){
		XML.pushTag("square", i);
	
		ofRectangle rect;
		rect.x = XML.getValue("x", 0.0) * 2;
		rect.y = XML.getValue("y", 0.0) * 2;
		rect.width = XML.getValue("w", 0.0) * 2;
		rect.height = XML.getValue("h",0.0) * 2;
		
		rectangles.push_back(rect);
		
		XML.popTag();
	}
	
}


void brandMode::start(){
	
	ofSeedRandom(0);
	
	int nVideos = DVM->danceVideos.size();
	
	associations.clear();
	bIsParticle.clear();
	particles.clear();
	
	// let's make particles, then associations. 
	
	for (int i = 0; i < nVideos; i++){
		
		brandParticle p;
		p.setup();
		p.calc(DVM->danceVideos[i], rectangles[(nVideos-1)-i]);
		particles.push_back(p);
		bIsParticle.push_back(true);
		
		int whichVideo = (int)ofRandom(0,nVideos) % nVideos;
		associations.push_back(whichVideo);
	}
	
	
	for (int i = nVideos; i < rectangles.size(); i++){
		int whichVideo = (int)ofRandom(0,nVideos) % nVideos;
		
		associations.push_back(whichVideo);
		bIsParticle.push_back(false);
	}
	
}
void brandMode::end(){
	
	//for (int i = 0; i < 
	
}

//vector < ofRectangle > rectangles;
//vector < int > associations;


static float energy = 0;

void brandMode::update(){
	for (int i = 0; i < particles.size(); i++){
		particles[i].update();
	}
	
	if (ofGetKeyPressed('g')){
		energy = 0.96 * energy  + 0.04 * 1;
	} else {
		energy = 0.96 * energy  + 0.04 * 0;
	}
	
	
	// get stuff into the pointilist: 
	
	
	
	
}

void brandMode::draw(){
	
	
	//just experimenting. 
	//glRotatef(ofGetElapsedTimef()*10, 0,1,0);
	
	ofSetColor(255,255,255,energy*30);
	eagle.draw(0,0,eagle.width*2, eagle.height*2);
	
	
	int nVideos = DVM->danceVideos.size();
		
	ofSeedRandom(0);
	
	for (int i = 0; i < rectangles.size(); i++){
		
		if (energy < 0.01) continue;
		if (bIsParticle[i] == false){
			ofSetColor(255,255,255,energy*255);
			
			int whichVideo = associations[i] % nVideos; 
			
			
			pointilist->addPoint( rectangles[i].x + rectangles[i].width/2, rectangles[i].y + rectangles[i].height/2, 0,
								 rectangles[i].width,
								 1,1,1,energy,
								 DVM->danceVideos[whichVideo]->texIndex, 0,  DVM->danceVideos[whichVideo]->firstFrame +  DVM->danceVideos[whichVideo]->currentFrame
								 );
			
			//(int)ofRandom(0,nVideos) % nVideos;
			// TODO:we need to fix this. 
			//DVM->danceVideos[whichVideo]->draw(ofPoint(rectangles[i].x, rectangles[i].y), rectangles[i].width, rectangles[i].height);
		}
		//ofRect(rectangles[i].x*2, rectangles[i].y*2, rectangles[i].width*2, rectangles[i].height*2);
	}
	
	//for (int i = 0; i < particles.size(); i++){
//		ofSetColor(255,255,255,255);
//
//		//particles[i].draw();
//	}
	
	for (int i = 0; i < particles.size(); i++){
		
		ofPoint pt = particles[i].mixPt;
		float size = particles[i].origWH.x * particles[i].mixSize;
		
		
		pointilist->addPoint( pt.x, pt.y, pt.z,
							 size,
							 1,1,1,1,
							 particles[i].dvPtr->texIndex, 0,  particles[i].dvPtr->firstFrame +  particles[i].dvPtr->currentFrame
							 );
	}
	
	pointilist->draw();
}

