/*
 *  networkManager.h
 *  emptyExample
 *
 *  Created by zachary lieberman on 10/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


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