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
    NSString *lastGifUrl;
    string sup;
    
    testApp *app;
}


@property (nonatomic, assign) testApp *app;
@property (nonatomic, assign) string sup;
@property (assign, nonatomic) double uploadProgress;
@property (assign, nonatomic) bool isUploading;
@property (assign, nonatomic) bool uploadFailed;
@property (assign, nonatomic) NSString *lastGifUrl;

- (void)startGifRequest:(double)frameDelay;
- (void)startVideoUploadRequest:(NSString *)fileNameMinusExtension withNumFrames:(int)num;
- (void)uploadRequestDidFinish:(ASIHTTPRequest*)request;
- (void)uploadRequestDidFail:(ASIHTTPRequest*)request;

- (void)savePngFrom:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height;
- (NSData*)getDataFromPixels:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height;
- (void)addFrame:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height;
- (void)gifDecode:(NSString*)path;

@end
