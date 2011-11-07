
#include "burstMode.h"


void burstMode::setup(){

}


void burstMode::start(){
	
	nFrames = DVM->danceVideos.size() * 24;
	frameCount = 0;
	bFirstFrame = true;
	timeTotal = 0;
	
}

void burstMode::end(){
			
}


void burstMode::update(){
	
	float time = ofGetElapsedTimef();
	if (bFirstFrame == true){
		lastTimeF = time;
		bFirstFrame = false;
	}
	float diff = time - lastTimeF;
	
	float pctX = (float)ofGetMouseX() / (float)ofGetWidth();
	timeTotal += (pctX*0.3)*diff;
	
	//cout << timeTotal <<endl;
	// how many frames did we advance?

	float frameLength = 0.1f; // 100 millis?  faster ?
	float totalLength = nFrames * frameLength;
	
	while (timeTotal  > totalLength){
		timeTotal -= totalLength;
	}
	
	int frame = (int)(timeTotal / frameLength);
	//cout << frame <<endl;
	whichVideo = (int)(frame / 24);  ///  % DVM->danceVideos.size()  TODO: checks!
	whichFrame = (int)(frame % 24);
	
}

void burstMode::draw(){
	float pctX = (float)ofGetMouseX() / (float)ofGetWidth();
	
	
	danceVideo * temp = NULL;
	
	if (whichVideo <  DVM->danceVideos.size()){
		temp = DVM->danceVideos[whichVideo];
		
		pointilist->addPoint( ofGetWidth()/2, ofGetHeight()/2, 0,
							 200 + 2000*pctX,
							 1,1,1,0.96,
							 temp->texIndex, 0,  temp->firstFrame +  whichFrame
							 );
		pointilist->draw();
	}
}

