#ifndef _TYPER
#define _TYPER

#include "ofMain.h"

class typer {
	
	public:
		
			
		void setup();
		void reset();
		void keyPressed (int key);

		vector < int > characters;
		string hmm;
	
		string getString();
	

	
};

#endif
	
