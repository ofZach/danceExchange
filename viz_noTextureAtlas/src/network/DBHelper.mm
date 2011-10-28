#import "DBHelper.h"

@implementation DBHelper

@synthesize danceInfos;
@synthesize isRequestingRecentDanceInfos;
@synthesize requestInterval;
@synthesize newestId;

- (id)init
{
    self = [super init];
    if (self) {        
        [self setNewestId:0];
        // check to see that the videos directory exists...
        NSURL *videoPath = [NSURL URLWithString:@"../data/videos/" relativeToURL:[[NSBundle mainBundle] bundleURL]];
        NSURL *tempPath = [NSURL URLWithString:@"../data/temp/" relativeToURL:[[NSBundle mainBundle] bundleURL]];
        
        BOOL isDir;
        BOOL videoDirectoryExists = [[NSFileManager defaultManager] fileExistsAtPath:[videoPath path] isDirectory:&isDir];
        if ( !videoDirectoryExists ) {
            NSError *error = nil ;
            [[NSFileManager defaultManager] createDirectoryAtPath:[videoPath path] withIntermediateDirectories:YES attributes:nil error:&error];
            if ( error != nil ) {
                NSLog( @"there was an error creating the video directory: %@", [error localizedDescription] );
            }
        }
        
        BOOL tempDirectoryExists = [[NSFileManager defaultManager] fileExistsAtPath:[tempPath path] isDirectory:&isDir];
        if ( !tempDirectoryExists ) {
            NSError *error = nil;
            [[NSFileManager defaultManager] createDirectoryAtPath:[tempPath path] withIntermediateDirectories:YES attributes:nil error:&error];
            if ( error != nil ) {
                NSLog( @"there was an error creating the temp directory: %@", [error localizedDescription] );
            }
        }
        
    }
    
    return self;
}

- (bool)isProcessingDanceInfosWithoutVideos {
    return danceInfosWithoutVideos.size() > 0;
}

- (void)requestDancesSince {
    
//    NSLog( @"requestDancesSince %i", [self newestId] );
    
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://aaron-meyers.com/smirnoff/getDancesSince.php?id=%i", [self newestId]]];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(dancesSinceRequestDidFinish:)];
    [request setDidFailSelector:@selector(dancesSinceRequestDidFail:)];
    [request setTimeOutSeconds:120];
    [request startAsynchronous];
}

- (void)dancesSinceRequestDidFinish:(ASIHTTPRequest *)request {
    
//    NSLog( @"dancesSinceRequestDidFinish" );
    
    NSError *error;
    NSArray *dances = [[CJSONDeserializer deserializer] deserialize:[request responseData] error:&error];
    
    [self processDanceInfos:dances thatAreNew:YES];
    
    [self performSelector:@selector(requestDancesSince) withObject:self afterDelay:requestInterval];
}

- (void)dancesSinceRequestDidFail:(ASIHTTPRequest *)request {
    NSLog( @"dancesSinceRequestDidFail" );
}

- (void)requestRecentDances:(int)num {
    
    NSLog( @"requestRecentDances" );
    
    NSURL *url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/getRecent.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:@"blah" forKey:@"blah"];
    [request setPostValue:[NSNumber numberWithInt:num] forKey:@"num"];
    [request setDelegate:self];
    [request setDidFailSelector:@selector(recentDanceRequestDidFail:)];
    [request setDidFinishSelector:@selector(recentDanceRequestDidFinish:)];
    [request setTimeOutSeconds:120];
    [request startAsynchronous];
    
    [self setIsRequestingRecentDanceInfos:YES];
}

- (void)recentDanceRequestDidFinish:(ASIHTTPRequest *)request {
    
//    NSLog( @"recent dance request finished: %@", [request responseString] );
    NSError *error;
    NSArray *dances = [[CJSONDeserializer deserializer] deserialize:[request responseData] error:&error];
    
    [self processDanceInfos:dances thatAreNew:NO];  
    
    [self setIsRequestingRecentDanceInfos:NO];
}

- (void)recentDanceRequestDidFail:(ASIHTTPRequest *)request {
    
    NSLog( @"recent dance request failed: %@", [[request error] localizedDescription] );
    
}

- (void)requestVideoFile:(DanceInfo)di {
    
    NSLog( @"requestVideoFile" );
    NSString *subDir = [NSString stringWithCString:di.url.c_str()];
    NSString *hash = [NSString stringWithCString:di.hash.c_str()];
    NSNumber *dbId = [NSNumber numberWithInt:di.id];
    NSString *filename = [NSString stringWithFormat:@"%@_s.mov", hash];
    NSString *idFilename = [NSString stringWithFormat:@"%@_s.mov", dbId];
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://aaron-meyers.com/smirnoff/gifs/%@%@", subDir, filename]];
    NSLog( @"URL: %@", [url path] );
    
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(videoRequestDidFinish:)];
    [request setDidFailSelector:@selector(videoRequestDidFail:)];
    
    // make a dictionary with some information about this request
    NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
    [dictionary setValue:hash forKey:@"hash"];
    [dictionary setValue:idFilename forKey:@"filename"];
    [request setUserInfo:dictionary];
    [request startAsynchronous];
}

- (void)videoRequestDidFinish:(ASIHTTPRequest *)request {
    NSData *videoFile = [request responseData];
    
    NSString *savePath = [NSString stringWithFormat:@"../data/videos/%@", [[request userInfo] valueForKey:@"filename"]];
    BOOL success = [videoFile writeToURL:[NSURL URLWithString:savePath relativeToURL:[[NSBundle mainBundle] bundleURL]] atomically:NO];
    if ( success )
        NSLog( @"%@ written successfully", [[request userInfo] valueForKey:@"filename"] );
    else
        NSLog( @"failed to write %@", [[request userInfo] valueForKey:@"filename"] );
    
    // find the dance info file, add it to the danceInfos vector, remove it from the other vector
    if ( danceInfosWithoutVideos.back().hash == [[[request userInfo] valueForKey:@"hash"] cStringUsingEncoding:[NSString defaultCStringEncoding]] ) {
        danceInfos.push_back( danceInfosWithoutVideos.back() );
        danceInfosWithoutVideos.pop_back();
    }
    
    if ( danceInfosWithoutVideos.size() > 0 ) {
        DanceInfo &di = danceInfosWithoutVideos.back();
        [self requestVideoFile:di];
    }
}

- (void)videoRequestDidFail:(ASIHTTPRequest *)request {
    NSLog( @"videoRequestDidFail" );
    // TODO -- handle download errors
}

- (void)processDanceInfos:(NSArray *)dances thatAreNew:(BOOL)areNew {
    
//    NSLog( @"processDanceInfos" );
    
    NSError *error;
    for ( NSDictionary *dance in dances ) {
        if ( [[dance valueForKey:@"hash"] isEqualToString:@""] ) {
            // the hash is only set after the video file is copied over
            // so if the hash hasn't been set, it means that we managed to
            // query in the split second before the video had been copied
            // so we'll wait for the next update because it'll be done by then
            continue;
        }
        
        DanceInfo di;
        di.id = [[dance valueForKey:@"id"] intValue];
        di.hash = [[dance valueForKey:@"hash"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        di.city = [[dance valueForKey:@"city"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        di.url = [[dance valueForKey:@"url"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        di.creationTime = [[dance valueForKey:@"creation_time"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        di.numFrames = [[dance valueForKey:@"num_frames"] intValue];
        di.isNew = areNew;
        
        if ( areNew ) {
            NSLog( @"new dance info with id: %i and hash: %@", di.id, [dance valueForKey:@"hash"] );
        }
        
        if ( di.id > [self newestId] )
            [self setNewestId:di.id];
        
        // see if file exists yet
        NSString *filePath = [NSString stringWithFormat:@"../data/videos/%@_s.mov", [dance valueForKey:@"id"]];
        if ( [[NSURL URLWithString:filePath relativeToURL:[[NSBundle mainBundle] bundleURL]] checkResourceIsReachableAndReturnError:&error] ) {
//            NSLog( @"video already downloaded for %@", filePath );
            danceInfos.push_back( di );
        }
        else {
            NSLog( @"dance info has no video %@", [dance valueForKey:@"hash"] );
            danceInfosWithoutVideos.push_back( di );
        }
    }
    
    // if there are dance infos with no downloaded videos, download them
    if ( danceInfosWithoutVideos.size() > 0 ) {
        DanceInfo &di = danceInfosWithoutVideos.back();
        [self requestVideoFile:di];
    }
    
}

- (void)clearDanceInfos {
    danceInfos.clear();
}

@end
