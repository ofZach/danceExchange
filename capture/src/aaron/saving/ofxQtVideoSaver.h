#ifndef _QT_VIDEO_GRABBER_
#define _QT_VIDEO_GRABBER_

#include "ofConstants.h"
#include "ofQtUtils.h"
#include "ofUtils.h"

#define OF_QT_SAVER_CODEC_QUALITY_LOSSLESS		0x00000400
#define OF_QT_SAVER_CODEC_QUALITY_MAX			0x000003FF
#define OF_QT_SAVER_CODEC_QUALITY_HIGH			0x00000300
#define OF_QT_SAVER_CODEC_QUALITY_NORMAL		0x00000200
#define OF_QT_SAVER_CODEC_QUALITY_LOW			0x00000100
#define OF_QT_SAVER_CODEC_QUALITY_MIN			0x00000000

// this is likely better: 
//http://developer.apple.com/mac/library/samplecode/CaptureAndCompressIPBMovie/listing3.html
// or: 
// http://www.openlibraries.org/browser/trunk/src/openmedialib/plugins/quicktime?rev=997
// or the new opencv videoWriter (does it support h264?).
// http://path.berkeley.edu/~zuwhan/QTforOpenCVWin/cvcap_qt.cpp   (says "RAW")


class ofxQtVideoSaver{

	public :
		
	
		ofxQtVideoSaver();
	
		void setup(int width , int height);
		void startMovie(string fileName = "");
		void finishMovie();
		void addFrame(unsigned char* data, float frameLengthInSecs = (1/30.0f));
		void clear();
	
	
		void listCodecs			();
		void setCodecType		(int chosenCodec );
		void setCodecType		(string codeName );
		void setGworldPixel		(GWorldPtr gwPtr, int r, int g, int b, short x, short y);  // just for checking...
		
		void setCodecQualityLevel	(int level);		// see the #defines above...
		
		bool bAmSetupForRecording	() { return bSetupForRecordingMovie; }
		void addAudioTrack			(string audioPath);
		
		void setRecording(bool bAmRecording) {  bRecording = bAmRecording; }
		bool bAmRecording(){ return bRecording; }
    
        string finalFileSavingSetting();
        string movieNameFromTime(bool bAppendDirectory);
	
	private:
		
		//-------------------------------- movie
		
		bool					bSetupForRecordingMovie;
		bool					bHaveFile;
		bool					bRecording;
	
		
		int						w; 
		int						h;
	    int						codecQualityLevel;
		OSErr 					osErr;
	    short 					sResId;
	    short 					sResRefNum;
	    FSSpec 					fsSpec;
	    Movie 					movie;
	    Track 					track;
	    Media 					media;
	    string					fileName;
	    FSRef					fsref;

		// ------------------------------- frame
		Rect 					rect;
		RGBColor 				rgbColor;
		CodecType 				codecType;
		GWorldPtr 				pMovieGWorld;
		PixMapHandle 			pixMapHandle;
		Ptr 					pCompressedData;
		CGrafPtr 				pSavedPort;
		GDHandle 				hSavedDevice;
		Handle 					hCompressedData;
		long 					lMaxCompressionSize;
		ImageDescriptionHandle	hImageDescription;
};

#endif
