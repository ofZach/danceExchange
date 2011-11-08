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
    [dbHelper setRecentRequestInterval:5.0];
    [dbHelper setRandomRequestInterval:5.0];
    [dbHelper setHeroku:YES];
    [dbHelper setNumRandomToRequest:5];
//    [dbHelper requestRecentDances:300];
    isRequestingRecentDances = false;
    isRequestingHandshake = false;
    isRequestingInitialDances = false;
}

void networkManager::requestLargeVideo( danceVideo *dv ) {
    [dbHelper requestLargeVideoFile:dv->info];
}

void networkManager::requestHandshake( int version ) {
    [dbHelper requestHandshake:version];
    isRequestingHandshake = true;
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
    
    if ( isRequestingHandshake && ![dbHelper isRequestingHandshake] ) {
        isRequestingHandshake = false;
        
//        cout << "handshake request finished..." << endl;
        string updateUrl = [dbHelper appUpdateUrl];
        if ( updateUrl == "" ) {
//            cout << "seems like everything was up to date" << endl;
            [dbHelper requestInitial:dvManager->numRecentVideos withRandom:dvManager->numRandomVideos];
            isRequestingInitialDances = true;
            
//            [dbHelper requestRecentDances:200];
//            isRequestingRecentDances = true;
            
        }
        else {
//            cout << "we need an update from this url: " << updateUrl << endl;
            ofLaunchBrowser( updateUrl );
            ofExit();
        }
    }
    
    if ( isRequestingRandomDances && ![dbHelper isRequestingRandomDanceInfos] && ![dbHelper isProcessingDanceInfosWithoutVideos] ) {
        cout << "random dance request finished... " << endl;
        isRequestingRandomDances = false;
    }
    
    if ( isRequestingInitialDances && ![dbHelper isRequestingInitialDanceInfos] && ![dbHelper isProcessingDanceInfosWithoutVideos] ) {
        cout << "initial request finished..." << endl;
        isRequestingInitialDances = false;
        [dbHelper requestDancesSince];
        [dbHelper requestRandomDances];
    }
    
//    if ( isRequestingRecentDances && ![dbHelper isRequestingRecentDanceInfos] && ![dbHelper isProcessingDanceInfosWithoutVideos] ) {
//        cout << "initial request finished..." << endl;
//        isRequestingRecentDances = false;
//        [dbHelper requestDancesSince];
//    }
}

void networkManager::requestRandomDances( int num ) {
    [dbHelper requestRandomDances:num];
    isRequestingRandomDances = true;
}