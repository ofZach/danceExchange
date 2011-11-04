#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "ASIFormDataRequest.h"
#import "CJSONDeserializer.h"
#include <string>
using namespace std;

class testApp;


@interface Helper : NSObject {
    NSMutableArray *frames;
    double uploadProgress;
    bool isUploading;
    bool uploadFailed;
    bool isEmailing;
    NSString *lastGifUrl;
    string sup;
    string lastHash;
    bool isRequestingHandshake;
    bool heroku;
    string appUpdateUrl;
    
    testApp *app;
}


@property (nonatomic, assign) testApp *app;
@property (nonatomic, assign) string sup;
@property (nonatomic, assign) string lastHash;
@property (assign, nonatomic) double uploadProgress;
@property (assign, nonatomic) bool isUploading;
@property (assign, nonatomic) bool isEmailing;
@property (assign, nonatomic) bool uploadFailed;
@property (assign, nonatomic) NSString *lastGifUrl;
@property (assign, nonatomic) string appUpdateUrl;
@property (assign, nonatomic) bool isRequestingHandshake;
@property (assign, nonatomic) bool heroku;


- (void)requestHandshake:(int)version;
- (void)handshakeRequestDidFinish:(ASIHTTPRequest*)request;
- (void)handshakeRequestDidFail:(ASIHTTPRequest*)request;

- (void)startGifRequest:(double)frameDelay;
- (void)startVideoUploadRequest:(NSString *)fileNameMinusExtension withNumFrames:(int)num;
- (void)uploadRequestDidFinish:(ASIHTTPRequest*)request;
- (void)uploadRequestDidFail:(ASIHTTPRequest*)request;

- (void)sendEmailTo:(NSString *)emailAddress;
- (void)sendEmailRequestDidFinish:(ASIHTTPRequest*)request;
- (void)sendEmailRequestDidFail:(ASIHTTPRequest*)request;

- (void)savePngFrom:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height;
- (NSData*)getDataFromPixels:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height;
- (void)addFrame:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height;
- (void)gifDecode:(NSString*)path;

@end
