

#include "brandMode.h"


// 0.193359, 0.217014
// 0.581055, 0.217014
// 0.585938, 0.800347
// 0.193359, 0.800347


void brandMode::setup(){
	loadSquareSettings();
	eagle.loadImage("images/eagle.png");
	ofEnableArbTex();
	video.loadMovie("movies/NEP-Title-v5.mov");
	video.play();
	//video.setSpeed(0.4);
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
		
		rectanglesPre.push_back(rect);
		rectangles.push_back(rect);
		
		XML.popTag();
	}
	
}


void brandMode::start(){
	
	ofSeedRandom(0);
	bHitPause = false;
	
	video.setPosition(0);
	video.setSpeed(1);
	video.play();
	video.update();
	
	float ratioV = 16.0 / 9.0;
	float ratioS = (float)ofGetWidth() / (float)ofGetHeight();
	if (ratioS > ratioV){
		// wide screen	
		videoRect.y = 0;
		videoRect.height = ofGetHeight();
		videoRect.width = ofGetHeight() * (ratioV);
		videoRect.x = (ofGetWidth() - videoRect.width)/2;
	} else {
		// tall screen
		videoRect.x = 0;
		videoRect.width = ofGetWidth();
		videoRect.height = ofGetWidth() * (1/ratioV);
		videoRect.y = (ofGetHeight() - videoRect.height)/2;
	}
	//videoRect
	
	// 0.193359, 0.217014
	// 0.581055, 0.217014
	// 0.585938, 0.800347
	// 0.193359, 0.800347
	
	eagleRect.x = videoRect.x + 0.193359 * videoRect.width;
	eagleRect.y = videoRect.y + 0.217014 * videoRect.height;
	eagleRect.width = (0.581055 - 0.193359) * videoRect.width;
	eagleRect.height = (0.800347 - 0.217014) * videoRect.height;
	
	// 441 371
	// float scale = 
	
	for (int i = 0; i < rectanglesPre.size(); i++){
		rectangles[i].x = rectanglesPre[i].x * (eagleRect.width / (2*441.0)) + eagleRect.x;
		rectangles[i].y = rectanglesPre[i].y * (eagleRect.height / (2*371.0)) + eagleRect.y;
		rectangles[i].width = rectanglesPre[i].width * (eagleRect.width / (2*441.0));
		rectangles[i].height = rectanglesPre[i].height * (eagleRect.height / (2*371.0));
	}
	
	ofSeedRandom(ofGetElapsedTimeMillis());
	vector < int > shuffleMe;
	for (int i = 0; i < DVM->danceVideos.size(); i++){
		shuffleMe.push_back(i);
	}
	std::random_shuffle( shuffleMe.begin(), shuffleMe.end() );
	
	int nVideos = MIN(DVM->danceVideos.size(), 30);
	
	associations.clear();
	bIsParticle.clear();
	particles.clear();
	
	// let's make particles, then associations. 
	
	for (int i = 0; i < nVideos; i++){
		
		brandParticle p;
		p.setup();
		p.calc(DVM->danceVideos[shuffleMe[i]], rectangles[i]);
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
	
	rectanglesWithRandomness.clear();
	for (int i = 0; i < rectangles.size(); i++){
		rectanglesWithRandomness.push_back(rectangles[i]);
		float randomAngle = ofRandom(0,TWO_PI);
		float randomSpeed = ofRandom(10,135);
		rectanglesWithRandomness[i].x += randomSpeed * cos(randomAngle);
		rectanglesWithRandomness[i].y += randomSpeed * sin(randomAngle);
		
	}
	vector < ofRectangle > rectanglesWithRandomness;
	energy = 0;
	
	// calculate the video rectangle
	
	
}
void brandMode::end(){
	
	//for (int i = 0; i < 
	
}

//vector < ofRectangle > rectangles;
//vector < int > associations;




void brandMode::update(){
	
	//cout << video.getPosition() << endl;
	
	if (video.getPosition() > 0.614){
		video.setSpeed(0);
		if (!bHitPause){
			pauseTime = ofGetElapsedTimef();
			bHitPause = true;
		}
	}
	
	bool bDoing = false;
	if (bHitPause == true){
		float dt = ofGetElapsedTimef() - pauseTime;
		if (dt > 0.8 && dt < 8){
			bDoing = true;
		} 
	} 
	
	if (video.getSpeed() < 0.01){
		if (energy > 0.99f){
			//video.setPosition(0.99f);
			//video.update();
		}
	}
	
	
	for (int i = 0; i < particles.size(); i++){
		particles[i].update();
	}
	
	if (bDoing){ //ofGetKeyPressed('g')){
		energy = 0.96 * energy  + 0.04 * 1;
		for (int i = 0; i < particles.size(); i++){
			particles[i].bLocking = true;
		}
	} else {
		energy = 0.96 * energy  + 0.04 * 0;
		for (int i = 0; i < particles.size(); i++){
			particles[i].bLocking = false;
		}
	}
	
	
	// get stuff into the pointilist: 
	
	if (ofGetMousePressed(0)){
		float x = ofGetMouseX();
		float y = ofGetMouseY();
		float pctx = (x - videoRect.x)/ (float)videoRect.width;
		float pcty = (y - videoRect.y)/ (float)videoRect.height;
		printf("pct x y (%f, %f)\n", pctx, pcty);
	}
	
	
	video.update();
	
}

void brandMode::draw(){
	
	ofEnableAlphaBlending();
	//just experimenting. 
	//glRotatef(ofGetElapsedTimef()*10, 0,1,0);
	
	
	glPushMatrix();
	
	float maxScale = ofGetHeight() / eagleRect.height;
	maxScale -= 1;
	ofPoint offset = ofPoint(ofGetWidth()/2, ofGetHeight()/2,0) - ofPoint(eagleRect.x + eagleRect.width / 2.0, eagleRect.y + eagleRect.height/2.0, 0) ;
	
	glTranslated(eagleRect.x + eagleRect.width / 2.0 + offset.x*energy, eagleRect.y + eagleRect.height/2.0 + offset.y*energy, 0);
	glScalef(1 + maxScale * energy, 1 + maxScale * energy, 1);
	
	// TODO: add some rotate smartness. 
	//if (energy > 0) glRotatef(ofGetElapsedTimef()*10, 0,1*energy,0);
	
	glTranslated(-(eagleRect.x + eagleRect.width / 2.0), -(eagleRect.y + eagleRect.height/2.0), 0);
	
	ofSetColor(255,255,255,energy*30);
	//eagle.draw(eagleRect.x, eagleRect.y, eagleRect.width, eagleRect.height);
	
	
	int nVideos = DVM->danceVideos.size();
	
	ofSeedRandom(0);
	
	for (int i = 0; i < rectangles.size(); i++){
		
		if (energy < 0.01) continue;
		if (bIsParticle[i] == false){
			ofSetColor(255,255,255,energy*255);
			
			int whichVideo = associations[i] % nVideos; 
			
			float x = (rectangles[i].x + rectangles[i].width/2) * energy + (rectanglesWithRandomness[i].x + rectanglesWithRandomness[i].width/2) * (1-energy);
			float y = (rectangles[i].y + rectangles[i].height/2) * energy + (rectanglesWithRandomness[i].y + rectanglesWithRandomness[i].height/2) * (1-energy);
			
			pointilist->addPoint( x,y, 0,
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
	
	ofPoint eagleMid = ofPoint(eagleRect.x + (2*eagleRect.width) / 3.0, eagleRect.y + eagleRect.height/2.0, 0);
	for (int i = 0; i < particles.size(); i++){
		
		ofPoint pt = particles[i].mixPt;
		float size = particles[i].origWH.x * particles[i].mixSize;
		
		float dist = (ofPoint(pt.x, pt.y, 0) - eagleMid).length();
		float alpha = dist / 300.0;
		if (alpha > 1) alpha = 1;
		float alphaMe = energy * 1 + (1-energy)*alpha;
		
		
		
		pointilist->addPoint( pt.x, pt.y, pt.z,
							 size,
							 1,1,1,alphaMe,
							 particles[i].dvPtr->texIndex, 0,  particles[i].dvPtr->firstFrame +  particles[i].dvPtr->currentFrame
							 );
	}
	
	pointilist->draw();
	
	glDisable(GL_DEPTH_TEST);
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	float gray = powf((1-energy), 0.7)*255;
	ofSetColor(gray,gray,gray);
	video.draw(videoRect.x,videoRect.y, videoRect.width,videoRect.height);
	glEnable(GL_DEPTH_TEST);
	
	ofNoFill();
	ofSetColor(255,255,0);
	//ofRect(videoRect.x, videoRect.y, videoRect.width, videoRect.height);
	//ofRect(eagleRect.x, eagleRect.y, eagleRect.width, eagleRect.height);
	
	glPopMatrix();
	
}

