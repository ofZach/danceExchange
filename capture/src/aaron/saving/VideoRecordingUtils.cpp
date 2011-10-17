/*
 *  VideoRecordingUtils.cpp
 *  akRecorder_09
 *
 *  Created by Golan Levin on 1/24/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */

#include "VideoRecordingUtils.h"

VideoRecordingUtils::VideoRecordingUtils (int faceVideoWidth, int faceVideoHeight, string _saveFilePath, int _bAbsolutePath){

	// OFQTS.listCodecs();
	OFQTS.setCodecType("H.264");
	OFQTS.setup(faceVideoWidth, faceVideoHeight);
	bAmRecording = false;
	lastMovieStartTimeMillis = -1;
	currentMovieFrameCount = 0;
	bAbsolutePath = _bAbsolutePath;
	saveFilePath = _saveFilePath;
	
	cout << "saveFilePath!!! :" << saveFilePath << " bap: " << bAbsolutePath << endl;
}

//--------------------------------------------------------------
bool VideoRecordingUtils::isRecording(){
	return bAmRecording;
}

//--------------------------------------------------------------
void VideoRecordingUtils::draw (){
	if (bAmRecording == true){
		ofSetColor(255, 0, 0);
		ofFill();
		ofRect(0,0,10,10);
	}
}


//--------------------------------------------------------------
void VideoRecordingUtils::startRecording(){
	if (bAmRecording == false){
		bAmRecording = true;
		currentMovieFrameCount = 0;
		fileName = movieNameFromTime(false);
		
		if (bAbsolutePath == 1) ofDisableDataPath();
		OFQTS.startMovie(saveFilePath + "/" + fileName);
		ofEnableDataPath();
		
		lastMovieStartTimeMillis = ofGetElapsedTimeMillis();
	}
}

//--------------------------------------------------------------
void VideoRecordingUtils::stopRecording(){
	if (bAmRecording == true){
		bAmRecording = false;
		OFQTS.finishMovie();
		finalFileSavingSetting();
		//printf("currentMovieFrameCount = %d\n", currentMovieFrameCount); 
	}
}

//--------------------------------------------------------------
//void VideoRecordingUtils::toggleRecording(){
//	bAmRecording = !bAmRecording;
//	if (bAmRecording == false){
//		OFQTS.finishMovie();
//	} else {
//		fileName = movieNameFromTime(false);
//	
//		if (bAbsolutePath == 1) ofDisableDataPath();
//		OFQTS.startMovie(saveFilePath + "/" + fileName);
//		ofEnableDataPath();
//		
//		lastMovieStartTimeMillis = ofGetElapsedTimeMillis();
//	}
//}
//--------------------------------------------------------------
//void VideoRecordingUtils::update (faceRecording *FR){
//	
//	if (bAmRecording){
//		Ipp8u* currentFaceImage = FR->getProcessedFrame();
//		OFQTS.addFrame((unsigned char *) currentFaceImage); 
//		currentMovieFrameCount++;
//	}
//}


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//void VideoRecordingUtils::createCompleteVideoFromFaceRecording (faceRecording *FR){
//	bAmRecording = false;
//	int nFrames = FR->currentPreviewRecordingFrameCount;
//	if ((nFrames > 0) && (nFrames < MAX_N_FRAMES_PER_VIDEO)){
//		startRecording();
//		for (int i=0; i<nFrames; i++){
//			Ipp8u* currentFaceImage = FR->ipp8u_previewVideo[i]; 
//			OFQTS.addFrame((unsigned char *) currentFaceImage); 
//			currentMovieFrameCount++;
//		}
//		stopRecording();
//	}
//}
//
//void VideoRecordingUtils::addNextFrameFromFaceRecording(Ipp8u *currentFaceImage){
//	if (bAmRecording){
//		OFQTS.addFrame((unsigned char *) currentFaceImage); 
//		currentMovieFrameCount++;
//	}
//}


//--------------------------------------------------------------
string VideoRecordingUtils::movieNameFromTime(bool bAppendDirectory){
	
	stringstream output;
	streamsize wid(2);
	string result;
	result = "";
	
	if (bAppendDirectory == true){
		// added 2010/01/.....   for example to the file name (for saving into appropriate directories....)
		output.str( "" );
		output << setfill('0') << setw(wid) << ofGetYear();
		result += (output.str());
		result += "/";
		output.str( "" );
		output << setfill('0') << setw(wid) << ofGetMonth();
		result += (output.str());
		result += "/";
	}
	
	result += "reface.";
	output.str( "" );
	output << setfill('0') << setw(wid) << ofGetYear();
	result += (output.str());
	output.str( "" );
	output << setfill('0') << setw(wid) << ofGetMonth();
	result += ("_" + output.str());
	output.str( "" );
	output << setfill('0') << setw(wid) << ofGetDay();
	result += ("_" + output.str());
	
	result += ".";
	output.str( "" );
	output << setfill('0') << setw(wid) << ofGetHours();
	result += ("" + output.str());
	output.str( "" );
	output << setfill('0') << setw(wid) << ofGetMinutes();
	result += ("_" + output.str());
	output.str( "" );
	output << setfill('0') << setw(wid) << ofGetSeconds();
	result += ("_" + output.str());
	result += ".mov";
	
	return result;
	
	/*
	//http://www.xvsxp.com/files/forbidden.php
	*/
}




//--------------------------------------------------------------
void VideoRecordingUtils::finalFileSavingSetting(){
	
	/* IMPORTANT INSTRUCTIONS:
	 download ffmpegx
	 show package contents
	 copy "ffmpeg" from resources into 'data' folder of sketch
	 sudo chown root:wheel ffmpeg
	 sudo chmod 755  ffmpeg
	 */
	
	//printf("------------------------------- \n");
	
	vector < string > movieNameSplit = ofSplitString(fileName, ".");
	string fileNameMinusExtension = "";
	for (int i = 0; i < movieNameSplit.size()-1; i++){
		fileNameMinusExtension += movieNameSplit[i];
		fileNameMinusExtension += ".";
	}
	//string saveFilePath = " ../../../data";
	
	string finalSettingCommnand1 =  "../../../data/./ffmpeg -i " + saveFilePath + "/" + fileName + " -vcodec h264 -b 4000 " + saveFilePath + "/" + fileNameMinusExtension + "_temp.mov  >/dev/null 2>&1";
	string finalSettingCommnand2 =  "mv -f " + saveFilePath + "/" + fileNameMinusExtension + "_temp.mov "  + saveFilePath + "/" + fileName + "  >/dev/null 2>&1";
	string finalSettingCommnand3 =  "../../../data/./ffmpeg -ss -1 -i " + saveFilePath + "/" + fileName + " -vcodec mjpeg -vframes 1 -an -f rawvideo -s 40x72 " + saveFilePath + "/" + fileNameMinusExtension + "jpg >/dev/null 2>&1";
	string finalSettingCommnand4 =  "../../../data/./ffmpeg -i " + saveFilePath + "/" + fileName + " -s 40x72 -r 4  " + saveFilePath + "/" + fileNameMinusExtension + "gif >/dev/null 2>&1";
	
	/*
	// these don't pipe to dev/null...but they poop in the console:
	string finalSettingCommnand1 =  "../../../data/./ffmpeg -i " + saveFilePath + "/" + fileName + " -vcodec h264 -b 4000 " + saveFilePath + "/" + fileNameMinusExtension + "_temp.mov";
	string finalSettingCommnand2 =  "mv -f " + saveFilePath + "/" + fileNameMinusExtension + "_temp.mov "  + saveFilePath + "/" + fileName ;
	string finalSettingCommnand3 =  "../../../data/./ffmpeg -ss -1 -i " + saveFilePath + "/" + fileName + " -vcodec mjpeg -vframes 1 -an -f rawvideo -s 40x72 " + saveFilePath + "/" + fileNameMinusExtension + "jpg";
	string finalSettingCommnand4 =  "../../../data/./ffmpeg -i " + saveFilePath + "/" + fileName + " -s 40x72 -r 4  " + saveFilePath + "/" + fileNameMinusExtension + "gif";
	*/
	
	
	//cout << finalSettingCommnand1 << endl;
	//cout << finalSettingCommnand1 << endl;
	//cout << finalSettingCommnand1 << endl;
	
	system(finalSettingCommnand1.c_str());
	system(finalSettingCommnand2.c_str());
	system(finalSettingCommnand3.c_str());
	system(finalSettingCommnand4.c_str());
	
	//printf("------------------------------- \n");
	
	
}


