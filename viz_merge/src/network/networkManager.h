#pragma once


#include "ofMain.h"
#include "DBHelper.h"
#include "DPManager.h"
#include "DVManager.h"


class networkManager {
	
	
	public: 
	
	
		void setup();
		void update();
        void requestLargeVideo( danceVideo *dv );
        void requestHandshake( int version );
	
		DBHelper *dbHelper;
		DPManager * dpManager;
        DVManager * dvManager;
        
        bool isRequestingHandshake;
		bool isRequestingRecentDances;
        bool isRequestingInitialDances;
	
	
};