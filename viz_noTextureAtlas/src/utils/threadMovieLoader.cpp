/*
 *  threadMovieLoader.cpp
 *  emptyExample
 *
 *  Created by itotaka on 1/13/10.
 *  Copyright 2010 YCAM. All rights reserved.
 *
 */

#include "threadMovieLoader.h"

#define USING_QT_EXPORT



//--------------------------------------------------------------
threadMovieLoader::threadMovieLoader( int totalFrames, int movieWidth, int movieHeight ) {
    
    this->totalFrames = totalFrames;
    this->movieWidth  = movieWidth;
    this->movieHeight = movieHeight;
	
	filename = "";
	imgLoaded = false;
	
	state = TH_STATE_UNLOADED;
    
	pixels = new unsigned char[movieHeight*movieWidth*TH_MOVIE_CHANNELS*totalFrames];
	
	for (int i = 0; i < movieHeight*movieWidth*TH_MOVIE_CHANNELS*totalFrames; i++){
		pixels[i] = 0;
	}
	
	ofImage tempImg;
	tempImg.setUseTexture(false);
	tempImg.loadImage("mask0.png");
	tempImg.setImageType(OF_IMAGE_COLOR_ALPHA);
    //    OF_IMAGE
	if (tempImg.width != movieWidth || tempImg.height != movieHeight){
		tempImg.resize(movieWidth, movieHeight);
	}
    
	unsigned char * pixelsFromMaskImg = tempImg.getPixels();
	maskPixels = new unsigned char [movieWidth*movieHeight*TH_MOVIE_CHANNELS];
	for (int i = 0; i < movieWidth*movieHeight*TH_MOVIE_CHANNELS; i++){
		maskPixels[i] = pixelsFromMaskImg[i];
	}
	
}

//--------------------------------------------------------------
threadMovieLoader::~threadMovieLoader() {
    
    delete pixels;
    delete maskPixels;
    
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
	
	string tempPath = ( movieWidth == 640 ? "big" : "" ) + fileId;
	string command = "rm " + ofToDataPath(tempPath) + "/*.png";
   
#ifdef USING_QT_EXPORT
	command = "../../../data/qt_export " + ofToDataPath(filename) + " --exporter=grex " + ofToDataPath(tempPath) + "/.png >/dev/null 2>&1";
	system(command.c_str());
#else
	command =  "../../../data/./ffmpeg -v quiet -i " + ofToDataPath(filename) + " " + ofToDataPath(tempPath) + "/%d.png >/dev/null 2>&1";
	system(command.c_str());
#endif
	
	nFrames = 0;
	
	
    for (int i = 0; i < totalFrames; i++ ) {
		temp.setUseTexture(false); // important !! threaded !!
		
		
		string nameT = ofToString(i+1);
		
#ifdef USING_QT_EXPORT		// QT export adds a 0 pad to file names. 
		if (nameT.size() == 1) nameT = "0" + nameT;
#endif 
		if (!temp.loadImage(tempPath+"/" + nameT + ".png")){
			break;
		} else {
            temp.setImageType( OF_IMAGE_COLOR_ALPHA );
			nFrames = i;
			
			if (temp.width != movieWidth || temp.height != movieHeight){
				temp.resize(movieWidth, movieHeight);
			}
			
			// funky for loop with some mask being applied ! 
			
			unsigned char * toPix = pixels + (i*movieHeight*movieWidth*TH_MOVIE_CHANNELS);
			unsigned char * fromPix = temp.getPixels();
			for (int j = 0; j < movieHeight*movieWidth*TH_MOVIE_CHANNELS; j++){
				float pct = maskPixels[j] / 255.0f;
                fromPix[j] = pct * fromPix[j];
                if ( j%4==3 ) {
                    // this is kind of hacky because my mask doesn't actually have an alpha layer
                    // its really just a black and white mask, so i just use the blue channel here
                    fromPix[j] = maskPixels[j-1];
                }
			}
			
			ofSleepMillis(10);
			
			memcpy(pixels + (i*movieHeight*movieWidth*TH_MOVIE_CHANNELS),temp.getPixels(), temp.width*temp.height*TH_MOVIE_CHANNELS);
		}
	}
	
	
	if (nFrames > 0){
		state = TH_STATE_JUST_LOADED;
		imgLoaded = true;
	} else {
		state = TH_STATE_UNLOADED;
		imgLoaded = false;
	}
    
	string removeCommand = "rm -r " + ofToDataPath(tempPath);
	system(removeCommand.c_str());
    
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
bool threadMovieLoader::start(string & _filename, string & _fileId ) {
    
	if (!isThreadRunning() && (_filename != "")) {
		state = TH_STATE_LOADING;
		filename = _filename;
        fileId = _fileId;
        
        string tempPath = ( movieWidth == 640 ? "big" : "" ) + fileId;
        string command = "mkdir " + ofToDataPath( tempPath );
        system(command.c_str());
        
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
