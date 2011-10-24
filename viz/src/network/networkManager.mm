/*
 *  networkManager.cpp
 *  emptyExample
 *
 *  Created by zachary lieberman on 10/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "networkManager.h"



void networkManager::setup(){
	dbHelper = [[DBHelper alloc] init];
    [dbHelper setRequestInterval:5.0];
    [dbHelper requestRecentDances:300];
    isRequestingRecentDances = true;
}

void networkManager::update(){
	vector<DanceInfo> danceInfos = dbHelper.danceInfos;
    if ( danceInfos.size() > 0 ) {
        for ( int i=0; i<danceInfos.size(); i++ ) {
//            dpManager->createParticle( danceInfos[i] );
            dvManager->createDanceVideo( danceInfos[i] );
        }
        [dbHelper clearDanceInfos];
		//        cout << "local dance info count: " << danceInfos.size() << endl;
		//        cout << "db helper dance info count: " << dbHelper.danceInfos.size() << endl;
    }
    
    if ( isRequestingRecentDances && ![dbHelper isRequestingRecentDanceInfos] && ![dbHelper isProcessingDanceInfosWithoutVideos] ) {
        cout << "initial request finished..." << endl;
        isRequestingRecentDances = false;
        [dbHelper requestDancesSince];
    }
}