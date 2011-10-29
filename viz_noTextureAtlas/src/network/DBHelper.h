#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "ASIFormDataRequest.h"
#import "CJSONDeserializer.h"

#include "DanceInfo.h"
#include <string>
#include <vector>
using namespace std;

@interface DBHelper : NSObject {
    
    
    vector<DanceInfo> danceInfosWithoutVideos;
    vector<DanceInfo> danceInfos;
    vector<string> danceHashesWithLargeVideos;
    bool isRequestingRecentDanceInfos;
    NSTimeInterval requestInterval;
    int newestId;
}

@property (assign, nonatomic) vector<DanceInfo> danceInfos;
@property (assign, nonatomic) vector<string> danceHashesWithLargeVideos;
@property (assign, nonatomic) bool isRequestingRecentDanceInfos;
@property (assign, nonatomic) NSTimeInterval requestInterval;
@property (assign, nonatomic) int newestId;

- (bool)isProcessingDanceInfosWithoutVideos;

- (void)requestDancesSince;
- (void)dancesSinceRequestDidFinish:(ASIHTTPRequest*)request;
- (void)dancesSinceRequestDidFail:(ASIHTTPRequest*)request;

- (void)requestRecentDances:(int)num;
- (void)recentDanceRequestDidFinish:(ASIHTTPRequest*)request;
- (void)recentDanceRequestDidFail:(ASIHTTPRequest*)request;

- (void)requestLargeVideoFile:(DanceInfo)di;
- (void)largeVideoRequestDidFinish:(ASIHTTPRequest*)request;
- (void)largeVideoRequestDidFail:(ASIHTTPRequest*)request;

- (void)requestVideoFile:(DanceInfo)di;
- (void)videoRequestDidFinish:(ASIHTTPRequest*)request;
- (void)videoRequestDidFail:(ASIHTTPRequest*)request;

- (void)processDanceInfos:(NSArray *)dances thatAreNew:(BOOL)areNew;
- (void)clearDanceInfos;
- (void)clearLargeVideoHashes;

@end
