#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "ASIFormDataRequest.h"
#import "CJSONDeserializer.h"

#include "DanceInfo.h"
#include <string>
#include <vector>
using namespace std;

@interface DBHelper : NSObject {
    
    
    NSOperationQueue *queue;
    
    vector<DanceInfo> danceInfosWithoutVideos;
    vector<DanceInfo> danceInfos;
    vector<string> danceHashesWithLargeVideos;
    bool isRequestingRecentDanceInfos;
    bool isRequestingHandshake;
    bool isRequestingInitialDanceInfos;
    bool isRequestingRandomDanceInfos;
    bool isRequestingDancesSince;
    bool isRequestingVideo;
    string appUpdateUrl;
    NSTimeInterval recentRequestInterval;
    NSTimeInterval randomRequestInterval;
    
    int numHandshakeFailures;
    int maxHandshakeFailures;
    int handshakeVersion;
    
    int newestId;
    int recentOffset;
    int maxRandom;
    int numRandomToRequest;
    bool heroku;
}

@property (assign, nonatomic) vector<DanceInfo> danceInfos;
@property (assign, nonatomic) vector<string> danceHashesWithLargeVideos;
@property (assign, nonatomic) bool isRequestingRecentDanceInfos;
@property (assign, nonatomic) bool isRequestingHandshake;
@property (assign, nonatomic) bool isRequestingInitialDanceInfos;
@property (assign, nonatomic) bool isRequestingRandomDanceInfos;
@property (assign, nonatomic) bool isRequestingDancesSince;
@property (assign, nonatomic) bool isRequestingVideo;
@property (assign, nonatomic) NSTimeInterval recentRequestInterval;
@property (assign, nonatomic) NSTimeInterval randomRequestInterval;
@property (assign, nonatomic) int newestId;
@property (assign, nonatomic) string appUpdateUrl;
@property (assign, nonatomic) NSOperationQueue *queue;
@property (assign, nonatomic) bool heroku;
@property (assign, nonatomic) int numRandomToRequest;

- (bool)isProcessingDanceInfosWithoutVideos;

- (void)requestHandshake;
- (void)requestHandshake:(int)version;
- (void)handshakeRequestDidFinish:(ASIHTTPRequest*)request;
- (void)handshakeRequestDidFail:(ASIHTTPRequest*)request;

- (void)requestRandomDances;
- (void)requestRandomDances:(int)num;
- (void)randomDanceRequestDidFinish:(ASIHTTPRequest*)request;
- (void)randomDanceRequestDidFail:(ASIHTTPRequest*)request;

- (void)requestInitial:(int)numRecent withRandom:(int)numRandom;
- (void)initialRequestDidFinish:(ASIHTTPRequest*)request;
- (void)initialRequestDidFail:(ASIHTTPRequest*)request;

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

- (void)processDanceInfos:(NSArray *)dances thatAreNew:(BOOL)areNew andAreRandom:(BOOL)areRandom;
- (void)clearDanceInfos;
- (void)clearLargeVideoHashes;

@end
