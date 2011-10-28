#pragma once


#include "ofMain.h"
#include "DBHelper.h"
#include "DPManager.h"
#include "DVManager.h"


class networkManager {
	
	
	public: 
	
	
		void setup();
		void update();
	
		DBHelper *dbHelper;
		//DPManager * dpManager;
        DVManager * dvManager;
    
		bool isRequestingRecentDances;
	
	
};