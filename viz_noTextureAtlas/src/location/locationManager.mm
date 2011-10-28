/*
 *  locationManager.cpp
 *  emptyExample
 *
 *  Created by zachary lieberman on 10/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "locationManager.h"


	
void locationManager::setup(){
		
	// parse cities csv file
    NSURL *citiesPath = [NSURL URLWithString:@"../data/cities.txt" relativeToURL:[[NSBundle mainBundle] bundleURL]];
    NSError *error;
    NSString *citiesRaw = [NSString stringWithContentsOfURL:citiesPath encoding:NSUTF8StringEncoding error:&error];
    NSArray *cityLines = [citiesRaw componentsSeparatedByString:@"\n"];
    for ( NSString *city in cityLines ) {
        
        NSArray *cityInfo = [city componentsSeparatedByString:@","];
        string cityName = [[cityInfo objectAtIndex:0] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        ofVec2f latLon( [[cityInfo objectAtIndex:1] floatValue], [[cityInfo objectAtIndex:2] floatValue] );
        
        cities.push_back( cityName );
        cityLatLonHash[ cityName ] = latLon;
    }
    
}
	
