#pragma once

#include "ofMain.h"


class locationManager {
	
	public: 

		void setup(); 

		ofVec2f latLonForCity(int whichCity){
			ofVec2f latLon = cityLatLonHash[ cities[ whichCity ] ];
			return latLon;
		}
	
		vector<string> cities;
		map<string,ofVec2f> cityLatLonHash;
		

	
};

