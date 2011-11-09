#import "DBHelper.h"

@implementation DBHelper

@synthesize danceInfos, danceHashesWithLargeVideos;
@synthesize isRequestingRecentDanceInfos, isRequestingHandshake;
@synthesize isRequestingInitialDanceInfos, isRequestingRandomDanceInfos, isRequestingDancesSince;
@synthesize isRequestingVideo;
@synthesize recentRequestInterval;
@synthesize randomRequestInterval;
@synthesize newestId;
@synthesize appUpdateUrl;
@synthesize queue;
@synthesize heroku;
@synthesize numRandomToRequest;

- (id)init {
    self = [super init];
    if (self) {        
        
        numHandshakeFailures = 0;
        maxHandshakeFailures = 10;
        
        [self setNumRandomToRequest:5];
        [self setNewestId:0];
        [self setAppUpdateUrl:""];
        [self setIsRequestingVideo:NO];
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
        
        [self setQueue:[[[NSOperationQueue alloc] init] autorelease]];
        [[self queue] setMaxConcurrentOperationCount:1];
    }
    
    return self;
}

- (bool)isProcessingDanceInfosWithoutVideos {
    return danceInfosWithoutVideos.size() > 0;
}

- (void)requestRandomDances {
    [self requestRandomDances:[self numRandomToRequest]];
}

- (void)requestRandomDances:(int)num {
    NSLog( @"requestRandomDances: %i", num );
    
    if ( num > maxRandom )
        num = maxRandom;
    
    NSURL *url;
    if ( [self heroku] )
        url = [NSURL URLWithString:@"http://dance-exchange.herokuapp.com/dances/random"];
    else
        url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/getRandom.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:[NSNumber numberWithInt:num] forKey:@"num"];
    [request setPostValue:[NSNumber numberWithInt:recentOffset] forKey:@"offset"];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(randomDanceRequestDidFinish:)];
    [request setDidFailSelector:@selector(randomDanceRequestDidFail:)];
    [request setTimeOutSeconds:120];
    [[self queue] addOperation:request];
    
    [self setIsRequestingRandomDanceInfos:YES];
}

- (void)randomDanceRequestDidFinish:(ASIHTTPRequest *)request {
    
    NSLog( @"randomDanceRequestDidFinish: %@", [request responseString] );
    
    NSError *error;
    NSArray *dances = [[CJSONDeserializer deserializer] deserialize:[request responseData] error:&error];
    
    NSLog( @"random dances: %i", [dances count] );
    
    [self processDanceInfos:dances thatAreNew:NO andAreRandom:YES];
    
    [self setIsRequestingRandomDanceInfos:NO];
    [self performSelector:@selector(requestRandomDances) withObject:self afterDelay:randomRequestInterval];
}

- (void)randomDanceRequestDidFail:(ASIHTTPRequest *)request {
    
    NSLog( @"randomDanceRequestDidFail: %@", [[request error] localizedDescription] );
    
    [self setIsRequestingRandomDanceInfos:NO];
    [self performSelector:@selector(requestRandomDances) withObject:self afterDelay:randomRequestInterval];
}

- (void)requestHandshake {
    [self requestHandshake:handshakeVersion];
}

- (void)requestHandshake:(int)version {
    
    NSLog( @"requestHandshake with version %i", version );
    
    handshakeVersion = version;
    
    NSURL *url;
    
    if ( [self heroku] )
        url = [NSURL URLWithString:@"http://dance-exchange.herokuapp.com/handshake"];
    else
        url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/handshake.php"];
    
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:@"viz" forKey:@"app"];
    [request setPostValue:[NSNumber numberWithInt:version] forKey:@"version"];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(handshakeRequestDidFinish:)];
    [request setDidFailSelector:@selector(handshakeRequestDidFail:)];
    [request setTimeOutSeconds:120];
//    [request startAsynchronous];
    [[self queue] addOperation:request];
    
    [self setIsRequestingHandshake:YES];
}

- (void)handshakeRequestDidFinish:(ASIHTTPRequest *)request {
    NSLog( @"handshake request did finish: %@", [request responseString] );
    
    NSError *error;
    NSArray *response = [[CJSONDeserializer deserializer] deserialize:[request responseData] error:&error];
    BOOL ok = [[response valueForKey:@"ok"] isEqualToNumber:[NSNumber numberWithInt:1]];
    
    if ( ok ) {
        NSLog( @"up to date" );
    }
    else {
        NSLog( @"not up to date" );
        NSString *url = [response valueForKey:@"url"];
        [self setAppUpdateUrl:[url cStringUsingEncoding:[NSString defaultCStringEncoding]]];
    }
    
    [self setIsRequestingHandshake:NO];
}

- (void)handshakeRequestDidFail:(ASIHTTPRequest *)request {
    
    if ( ++numHandshakeFailures < maxHandshakeFailures )
        [self performSelector:@selector(requestHandshake) withObject:self afterDelay:60.0];
    
    NSLog( @"handshake fail #%i", numHandshakeFailures );
    
}

- (void)requestDancesSince {
    
//    NSLog( @"requestDancesSince %i", [self newestId] );

    NSURL *url;
    if ( [self heroku] )
        url = [NSURL URLWithString:@"http://dance-exchange.herokuapp.com/dances/since"];
    else
        url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/getDancesSince.php"];
    
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:[NSNumber numberWithInt:[self newestId]] forKey:@"since_id"];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(dancesSinceRequestDidFinish:)];
    [request setDidFailSelector:@selector(dancesSinceRequestDidFail:)];
    [request setTimeOutSeconds:120];
//    [request startAsynchronous];
    [[self queue] addOperation:request];
    
    [self setIsRequestingDancesSince:YES];
}

- (void)dancesSinceRequestDidFinish:(ASIHTTPRequest *)request {
    
//    NSLog( @"dancesSinceRequestDidFinish" );
    
    NSError *error;
    NSArray *dances = [[CJSONDeserializer deserializer] deserialize:[request responseData] error:&error];
    
    [self processDanceInfos:dances thatAreNew:YES andAreRandom:NO];
    
    [self setIsRequestingDancesSince:NO];
    [self performSelector:@selector(requestDancesSince) withObject:self afterDelay:recentRequestInterval];
}

- (void)dancesSinceRequestDidFail:(ASIHTTPRequest *)request {
    NSLog( @"dancesSinceRequestDidFail" );
    [self setIsRequestingDancesSince:NO];
    [self performSelector:@selector(requestDancesSince) withObject:self afterDelay:recentRequestInterval];
}

- (void)requestInitial:(int)numRecent withRandom:(int)numRandom {
    NSLog( @"requestInitial: %i withRandom: %i", numRecent, numRandom );
    
    recentOffset = numRecent;
    maxRandom = numRandom;
    
    NSURL *url;
    
    if ( [self heroku] )
        url = [NSURL URLWithString:@"http://dance-exchange.herokuapp.com/dances/initial"];
    else
        url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/getInitial.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:[NSNumber numberWithInt:numRecent] forKey:@"numRecent"];
    [request setPostValue:[NSNumber numberWithInt:numRandom] forKey:@"numRandom"];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(initialRequestDidFinish:)];
    [request setDidFailSelector:@selector(initialRequestDidFail:)];
    [request setTimeOutSeconds:120];
//    [request startAsynchronous];
    [[self queue] addOperation:request];
    
    [self setIsRequestingInitialDanceInfos:YES];
}

- (void)initialRequestDidFinish:(ASIHTTPRequest *)request {
    NSLog( @"initialRequestDidFinish" );
    
    NSError *error;
    
//    [[request responseData] writeToFile:<#(NSString *)#> atomically:<#(BOOL)#>
    NSDictionary *dances = [[CJSONDeserializer deserializer] deserialize:[request responseData] error:&error];
    
    NSLog( @"recent dances to process: %i", [[dances valueForKey:@"recent"] count] );
    NSLog( @"random dances to process: %i", [[dances valueForKey:@"random"] count] );
    
    [self processDanceInfos:[dances valueForKey:@"recent"] thatAreNew:NO andAreRandom:NO];
    [self processDanceInfos:[dances valueForKey:@"random"] thatAreNew:NO andAreRandom:YES];
    
    [self setIsRequestingInitialDanceInfos:NO];
}

- (void)initialRequestDidFail:(ASIHTTPRequest *)request {
    
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
//    [request startAsynchronous];
    [[self queue] addOperation:request];
    
    [self setIsRequestingRecentDanceInfos:YES];
}

- (void)recentDanceRequestDidFinish:(ASIHTTPRequest *)request {
    
//    NSLog( @"recent dance request finished: %@", [request responseString] );
    NSError *error;
    NSArray *dances = [[CJSONDeserializer deserializer] deserialize:[request responseData] error:&error];
    
    [self processDanceInfos:dances thatAreNew:NO andAreRandom:NO];  
    
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
//    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://aaron-meyers.com/smirnoff/gifs/%@%@", subDir, filename]];
    NSURL *url = [NSURL URLWithString:[NSString stringWithCString:di.smallVideoUrl.c_str() encoding:[NSString defaultCStringEncoding]]];
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
//    [request startAsynchronous];
    [self setIsRequestingVideo:YES];
    [[self queue] addOperation:request];
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
    string hashString = [[[request userInfo] valueForKey:@"hash"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
    if ( danceInfosWithoutVideos.back().hash == hashString ) {
        danceInfos.push_back( danceInfosWithoutVideos.back() );
        danceInfosWithoutVideos.pop_back();
    }
    
    [self setIsRequestingVideo:NO];
    
    if ( danceInfosWithoutVideos.size() > 0 ) {
        DanceInfo &di = danceInfosWithoutVideos.back();
        [self requestVideoFile:di];
    }
}

- (void)videoRequestDidFail:(ASIHTTPRequest *)request {
    NSLog( @"videoRequestDidFail" );
    // TODO -- handle download errors
}

- (void)requestLargeVideoFile:(DanceInfo)di {
    NSString *subDir = [NSString stringWithCString:di.url.c_str()];
    NSString *hash = [NSString stringWithCString:di.hash.c_str()];
    NSNumber *dbId = [NSNumber numberWithInt:di.id];
    NSString *filename = [NSString stringWithFormat:@"%@.mov", hash];
    NSString *idFilename = [NSString stringWithFormat:@"%@.mov", dbId];
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://aaron-meyers.com/smirnoff/gifs/%@%@", subDir, filename]];
    NSLog( @"URL: %@", [url path] );
    
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(largeVideoRequestDidFinish:)];
    [request setDidFailSelector:@selector(largeVideoRequestDidFail:)];
    
    // make a dictionary with some information about this request
    NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
    [dictionary setValue:hash forKey:@"hash"];
    [dictionary setValue:idFilename forKey:@"filename"];
    [request setUserInfo:dictionary];
//    [request startAsynchronous];
    [[self queue] addOperation:request];
}

- (void)largeVideoRequestDidFinish:(ASIHTTPRequest *)request {
    NSData *videoFile = [request responseData];
    
    NSString *savePath = [NSString stringWithFormat:@"../data/videos/%@", [[request userInfo] valueForKey:@"filename"]];
    BOOL success = [videoFile writeToURL:[NSURL URLWithString:savePath relativeToURL:[[NSBundle mainBundle] bundleURL]] atomically:NO];
    if ( success )
        NSLog( @"%@ written successfully", [[request userInfo] valueForKey:@"filename"] );
    else
        NSLog( @"failed to write %@", [[request userInfo] valueForKey:@"filename"] );
    
    // find the dance info file, add it to the danceInfos vector, remove it from the other vector
//    danceInfosWithLargeVideos.push_back( )
    string hash = [[[request userInfo] valueForKey:@"hash"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
    danceHashesWithLargeVideos.push_back( hash );
}

- (void)largeVideoRequestDidFail:(ASIHTTPRequest *)request {
    
}

- (void)processDanceInfos:(NSArray *)dances thatAreNew:(BOOL)areNew andAreRandom:(BOOL)areRandom {
    
//    if ( areRandom )
//        NSLog( @"processDanceInfos (random) with count %i", [dances count] );
//    else
//        NSLog( @"processDanceInfos (recent) with count %i", [dances count] );
    
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
        if ( [self heroku] ) {
            di.hash = [[dance valueForKey:@"url_hash"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
            di.creationTime = [[dance valueForKey:@"created_at"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
            di.numFrames = [[dance valueForKey:@"frames"] intValue];
        }
        else {
            di.hash = [[dance valueForKey:@"hash"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
            di.url = [[dance valueForKey:@"url"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
            di.creationTime = [[dance valueForKey:@"creation_time"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
            di.numFrames = [[dance valueForKey:@"num_frames"] intValue];
        }
        di.city = [[dance valueForKey:@"city"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        di.smallVideoUrl = [[dance valueForKey:@"small_video_url"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        di.regularVideoUrl = [[dance valueForKey:@"regular_video_url"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
        di.isNew = areNew;
        di.isRandom = areRandom;
        
        
        if ( di.id > [self newestId] )
            [self setNewestId:di.id];
        
        // see if the large file exists
        NSString *largeFilePath = [NSString stringWithFormat:@"../data/videos/%@.mov", [dance valueForKey:@"id"]];
        if ( [[NSURL URLWithString:largeFilePath relativeToURL:[[NSBundle mainBundle] bundleURL]] checkResourceIsReachableAndReturnError:&error] )
            di.largeVideoDownloaded = true;
        else
            di.largeVideoDownloaded = false;
        
        // see if file exists yet
        NSString *filePath = [NSString stringWithFormat:@"../data/videos/%@_s.mov", [dance valueForKey:@"id"]];
        if ( [[NSURL URLWithString:filePath relativeToURL:[[NSBundle mainBundle] bundleURL]] checkResourceIsReachableAndReturnError:&error] ) {
//            NSLog( @"video already downloaded for %@", filePath );
            danceInfos.push_back( di );
        }
        else {
//            NSLog( @"dance info has no video %@", [dance valueForKey:@"hash"] );
            danceInfosWithoutVideos.push_back( di );
        }
    }
    
    // if there are dance infos with no downloaded videos, download them
    if ( danceInfosWithoutVideos.size() > 0 && ![self isRequestingVideo] ) {
        DanceInfo &di = danceInfosWithoutVideos.back();
        [self requestVideoFile:di];
    }
    
}

- (void)clearDanceInfos {
    danceInfos.clear();
}

- (void)clearLargeVideoHashes {
    danceHashesWithLargeVideos.clear();
}

@end
