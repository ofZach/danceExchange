#pragma once


#include "ofMain.h"
#include "DBHelper.h"
#include "DPManager.h"


class networkManager {
	
	
	public: 
	
	
		void setup();
		void update();
	
		DBHelper *dbHelper;
		DPManager * dpManager;
	
		bool isRequestingRecentDances;
	
	
};