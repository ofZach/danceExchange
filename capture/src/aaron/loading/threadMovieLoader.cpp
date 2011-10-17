/*
 *  threadMovieLoader.cpp
 *  emptyExample
 *
 *  Created by itotaka on 1/13/10.
 *  Copyright 2010 YCAM. All rights reserved.
 *
 */

#include "threadMovieLoader.h"

//--------------------------------------------------------------
threadMovieLoader::threadMovieLoader() {
	
	filename = "";
	imgLoaded = false;
	
	state = TH_STATE_UNLOADED;

	pixels = new unsigned char[MOVIE_HEIGHT*MOVIE_WIDTH*3*30*6];
	
	for (int i = 0; i < MOVIE_HEIGHT*MOVIE_WIDTH*3*30*6; i++){
		pixels[i] = 0;
	}
	
	ofImage tempImg;
	tempImg.setUseTexture(false);
	tempImg.loadImage("images/mask.png");
	tempImg.setImageType(OF_IMAGE_COLOR);
	if (tempImg.width != MOVIE_WIDTH || tempImg.height != MOVIE_HEIGHT){
		tempImg.resize(MOVIE_WIDTH, MOVIE_HEIGHT);
	}
									
	unsigned char * pixelsFromMaskImg = tempImg.getPixels();
	maskPixels = new unsigned char [MOVIE_WIDTH*MOVIE_HEIGHT*3];
	for (int i = 0; i < MOVIE_WIDTH*MOVIE_HEIGHT*3; i++){
		maskPixels[i] = pixelsFromMaskImg[i];
	}
	
}

//--------------------------------------------------------------
threadMovieLoader::~threadMovieLoader() {
}

//--------------------------------------------------------------
void threadMovieLoader::loadMovieAsImageSequence() {
	
	
	/*
	 
	
	this code **needs** ffmpeg and 
	a folder called "temp" in the data directory
	ie, 
	
	 
	-- bin
	----- *.app
	----- data
	--------- ffmpeg  (linux-y exe)
	--------- temp    (folder) 
	
	 
	how I got ffmpeg, some steps to recreate as needed:
	 
	download ffmpegx
	show package contents
	copy "ffmpeg" from resources
	sudo chown root:wheel ffmpeg
	sudo chmod 755  ffmpeg
	 
	sorry for the hack !! 
	it's better (less leaky) then quicktime and it's fairly simple 
	*/
	
	
	string command = "rm " + ofToDataPath("temp") + "/*.png";
	system(command.c_str());

	command =  "../../../data/./ffmpeg -v quiet -i " + ofToDataPath(filename) + " " + ofToDataPath("temp") + "/%d.png >/dev/null 2>&1";
	system(command.c_str());
	
	nFrames = 0;
	
	
	for (int i = 0; i < (6*30); i++){
		temp.setUseTexture(false); // important !! threaded !! 
		if (!temp.loadImage("temp/" + ofToString(i+1) + ".png")){
			break;
		} else {
			nFrames = i;
			
			if (temp.width != MOVIE_WIDTH || temp.height != MOVIE_HEIGHT){
				temp.resize(MOVIE_WIDTH, MOVIE_HEIGHT);
				
			}
			
			// funky for loop with some mask being applied ! 
			
			unsigned char * toPix = pixels + (i*MOVIE_HEIGHT*MOVIE_WIDTH*3);
			unsigned char * fromPix = temp.getPixels();
			for (int i = 0; i < MOVIE_HEIGHT*MOVIE_WIDTH*3; i++){
				float pct = maskPixels[i] / 255.0f;
				toPix[i] = pct * fromPix[i];
			}
			
			ofSleepMillis(10);
			
			//memcpy(pixels + (i*MOVIE_HEIGHT*MOVIE_WIDTH*3),temp.getPixels(), temp.width*temp.height*3);
		}
	}
	
	
	if (nFrames > 0){
		state = TH_STATE_JUST_LOADED;
		imgLoaded = true;
	} else {
		state = TH_STATE_UNLOADED;
		imgLoaded = false;
	}

	stop();

}

//--------------------------------------------------------------
void threadMovieLoader::unloadMovie() {
	
	if(imgLoaded){
		state = TH_STATE_UNLOADED;
		imgLoaded = false;
	}
}


//--------------------------------------------------------------
bool threadMovieLoader::start(string & _filename) {

	if (!isThreadRunning() && (_filename != "")) {
		state = TH_STATE_LOADING;
		filename = _filename;
		imgLoaded = false;
		startThread(true, false);  // blocking, verbose
		return true;
	}else{
		return false;
	}
}

//--------------------------------------------------------------
void threadMovieLoader::stop() {
	stopThread();
}

//--------------------------------------------------------------
void threadMovieLoader::threadedFunction() {
	while (isThreadRunning() != 0) {
			loadMovieAsImageSequence();
	}
}

//--------------------------------------------------------------

