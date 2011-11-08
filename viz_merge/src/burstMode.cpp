
#include "burstMode.h"


void burstMode::setup(){

}


void burstMode::start(){
	
	nFrames = DVM->danceVideos.size() * 24;
	frameCount = 0;
	bFirstFrame = true;
	timeTotal = 0;
	energy = 0;
	startTime = ofGetElapsedTimef();
	
}

void burstMode::end(){
			
}


void burstMode::update(){
	
	if (DVM->danceVideos.size() == 0){
		return;
	}
	
	if ( (ofGetElapsedTimef() - startTime) < 17){
		energy = 0.9985f * energy + 0.0015f * 1;	
	} else {
		energy = 0.996f * energy + 0.004f * 0;	
		
	}
	 
	float time = ofGetElapsedTimef();
	if (bFirstFrame == true){
		lastTimeF = time;
		bFirstFrame = false;
	}
	float diff = time - lastTimeF;
	
	float pctX = (float)(energy*300) / (float)ofGetWidth();
	timeTotal += (0.0003 + pctX*0.1)*diff;
	
	//cout << timeTotal <<endl;
	// how many frames did we advance?

	
	nFrames = DVM->danceVideos.size() * 24;
	
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
	//float pctX = (float)(energy*300)  / (float)ofGetWidth();
	
	if (DVM->danceVideos.size() == 0){
		return;
	}
	
	danceVideo * temp = NULL;
	
	if (whichVideo <  DVM->danceVideos.size()){
		temp = DVM->danceVideos[whichVideo];
		
		pointilist->addPoint( ofGetWidth()/2, ofGetHeight()/2, 0,
							 200 + (ofGetWidth()-200)*energy,
							 1,1,1,0.96,
							 temp->texIndex, 0,  temp->firstFrame +  whichFrame
							 );
		pointilist->draw();
	}
}

