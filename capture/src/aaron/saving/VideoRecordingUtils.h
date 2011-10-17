/*
 *  VideoRecordingUtils.h
 *  akRecorder_09
 *
 *  Created by Golan Levin on 1/24/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */


#ifndef _VIDEO_RECORDING_UTILS
#define _VIDEO_RECORDING_UTILS


#include "ofMain.h"
//#include "ipp.h"
#include "cv.h"
#include "cvaux.h"
//#include "VisionManager.h"
//#include "faceRecording.h"

#include "ofxQtVideoSaver.h"
#include "ofxQtVideoSaverH264.h"



class VideoRecordingUtils {
	
public:

	VideoRecordingUtils (int faceVideoWidth, int faceVideoHeight, string _saveFilePath, int _bAbsolutePath);
	void draw();
//	void update (faceRecording *FR);
//	void createCompleteVideoFromFaceRecording( faceRecording *FR); 
//	void addNextFrameFromFaceRecording(Ipp8u *currentFaceImage);
	
	void toggleRecording();
	void stopRecording();
	void startRecording();
	bool isRecording();
	
	string	movieNameFromTime (bool bAppendDirectory);
	long	lastMovieStartTimeMillis;
	
	ofxQtVideoSaver		OFQTS;
	bool				bAmRecording;
	int					currentMovieFrameCount;
	
	string			fileName;
	string				saveFilePath;
	int					bAbsolutePath;
	
	void finalFileSavingSetting();

};

#endif	