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

void networkManager::requestLargeVideo( danceVideo *dv ) {
    [dbHelper requestLargeVideoFile:dv->info];
}

void networkManager::update(){
	vector<DanceInfo> danceInfos = dbHelper.danceInfos;
    if ( danceInfos.size() > 0 ) {
        for ( int i=0; i<danceInfos.size(); i++ ) {
            dvManager->createDanceVideo( danceInfos[i] );
        }
        [dbHelper clearDanceInfos];
    }
    
    vector<string> largeVideos = dbHelper.danceHashesWithLargeVideos;
    if ( largeVideos.size() > 0 ) {
        for ( int i=0; i<largeVideos.size(); i++ ) {
            cout << "loaded a large video with hash " << largeVideos[i] << endl;
            dvManager->loadLargeVideo( largeVideos[i] );
        }
        [dbHelper clearLargeVideoHashes];
    }
    
    if ( isRequestingRecentDances && ![dbHelper isRequestingRecentDanceInfos] && ![dbHelper isProcessingDanceInfosWithoutVideos] ) {
        cout << "initial request finished..." << endl;
        isRequestingRecentDances = false;
        [dbHelper requestDancesSince];
    }
}