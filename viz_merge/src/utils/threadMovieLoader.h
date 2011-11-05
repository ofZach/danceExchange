/*
 *  threadMovieLoader.h
 *  emptyExample
 *
 *  Created by itotaka on 1/13/10.
 *  Copyright 2010 YCAM. All rights reserved.
 *
 */

#ifndef _THREAD_MOVIE_LOADER
#define _THREAD_MOVIE_LOADER

#include "ofMain.h"
#include "ofThread.h"
//#include "faceConstants.h"

#define MOVIE_WIDTH 160
#define MOVIE_HEIGHT 120
#define NUM_FRAMES 40
#define TH_MOVIE_CHANNELS 4

enum state_TH{
	
	TH_STATE_LOADING, TH_STATE_JUST_LOADED, TH_STATE_LOADED, TH_STATE_UNLOADED
	
};

class threadMovieLoader : public ofThread {
	
	public:
	
		threadMovieLoader( int totalFrames, int movieWidth, int movieHeight );
		~threadMovieLoader();
		
		bool    start(string & _filename, string & _fileId );
		void    stop();
		void    threadedFunction();
		void    loadMovieAsImageSequence();
		void	unloadMovie();

        int             totalFrames;
        int             movieWidth, movieHeight;
		int				nFrames;
		unsigned char * pixels;
		unsigned char	garbageFrame[MOVIE_WIDTH*MOVIE_HEIGHT];		// was testing....
	
	
		unsigned char * maskPixels;
	
        ofImage         temp;
		int				state;

	protected:
	
		string			filename;
        string          fileId;
		int				numTextures;
		int				textureIndex;
		bool            imgLoaded;
	
};

#endif





