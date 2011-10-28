#import "Helper.h"

@implementation Helper

@synthesize uploadProgress, isUploading, isEmailing, uploadFailed, lastGifUrl;
@synthesize sup, lastHash;
@synthesize app;
@synthesize appUpdateUrl, isRequestingHandshake;

- (id)init {
    self = [super init];
    if (self) {
        frames = [[NSMutableArray alloc] init];
        [self setIsEmailing:NO];
        [self setIsUploading:NO];
        [self setUploadProgress:0];
        [self setUploadFailed:NO];
        [self setLastGifUrl:@""];
        [self setAppUpdateUrl:""];
        
        NSError *error = nil;
        NSURL * bundle = [[NSBundle mainBundle] bundleURL];
        NSFileManager *fm = [NSFileManager defaultManager];
        NSURL * dataPath = [NSURL URLWithString:@"../data/capture.2011_10_08.12_38_05.mov" relativeToURL:bundle];
        NSLog( @"dataPath: %@", [dataPath path] );

        BOOL reachable = [dataPath checkResourceIsReachableAndReturnError:&error];
        if ( reachable ) NSLog( @"it is reachable!" );
        if ( error ) {
            NSLog( @"error: %@", [error localizedDescription] );
        }
        
        NSData *someData = [NSData dataWithContentsOfURL:dataPath options:nil error:&error];
        if ( error ) {
            NSLog( @"error: %@", [error localizedDescription] );
        } else {
            NSLog( @"data loaded was %u bytes long", [someData length] );
        }
        
//        NSArray *files = [fm contentsOfDirectoryAtPath:[dataPath path] error:&error];
//        NSLog( @"files count: %u", [files count] );
//        for ( NSString *file in files ) {
//            NSLog( @"file: %@", file );
//        }
        
        
//        NSURL * file = [NSURL URLWithString:@"../Data/file.txt" relativeToURL:bundle];
//        NSURL * absoluteFile = [file absoluteURL];
        
////        NSURL *url = [NSURL URLWithString:@"/Users/aaron/Documents/OpenFrameworks/apps/aaron/capture/bin/emptyExampleDebug.app/../data/capture.2011_10_08.12_40_17.mov"];
//        NSURL *url = [NSURL URLWithString:@"/Users/aaron/Desktop/capture.2011_10_08.12_40_17.mov"];
//        
//        [url checkResourceIsReachableAndReturnError:&error];
//        if ( error ) {
//            NSLog( @"error: %@", [error localizedDescription] );
//        }
//
//        NSData *someData = [NSData dataWithContentsOfURL:url options:nil error:&error];
//        if ( error ) {
//            NSLog( @"error: %@", [error localizedDescription] );
//        } else {
//            NSLog( @"data loaded was %u bytes long", [someData length] );
//        }
    }
    
    return self;
}

- (void)requestHandshake:(int)version {
    NSURL *url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/handshake.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:@"capture" forKey:@"app"];
    [request setPostValue:[NSNumber numberWithInt:version] forKey:@"version"];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(handshakeRequestDidFinish:)];
    [request setDidFailSelector:@selector(handshakeRequestDidFail:)];
    [request setTimeOutSeconds:120];
    [request startAsynchronous];
    
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
    
}

- (void)gifDecode:(NSString*)path {
    
//    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDesktopDirectory, NSUserDomainMask, YES);
//    NSString * desktopPath = [paths objectAtIndex:0];
//    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/test.gif", desktopPath]];
//    NSData *data = [NSData dataWithContentsOfURL:url];
//    NSImage *img = [[NSImage alloc] initWithContentsOfFile:[NSString stringWithFormat:@"%@/test.gif", desktopPath]];
//    NSLog( @"img with width %f", img.size.width );
//    
//    NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithData:data];
//    NSString *frameCount = [imageRep valueForProperty:NSImageFrameCount];
//    NSLog( @"frameCount: %@", frameCount );
    
}

- (void)startVideoUploadRequest:(NSString *)fileNameMinusExtension withNumFrames:(int)num {
    
    NSArray *pieces = [fileNameMinusExtension componentsSeparatedByString:@"/"];
    NSString *justFileName = [pieces lastObject];
    
    NSError *error = nil;
    NSURL * bundle = [[NSBundle mainBundle] bundleURL];
    NSString *regPath = [NSString stringWithFormat:@"../data/%@mov", justFileName];
    NSString *smallPath = [NSString stringWithFormat:@"../data/%@s.mov", justFileName];
    
    NSURL *regUrl = [NSURL URLWithString:regPath relativeToURL:bundle];
    NSURL *smallUrl = [NSURL URLWithString:smallPath relativeToURL:bundle];
    
    [self setIsUploading:YES];
    [self setUploadProgress:0];
    
    NSURL *url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/snap.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    
    [request setPostValue:@"blah" forKey:@"blah"];
    [request setPostValue:[NSNumber numberWithInt:num] forKey:@"num_frames"];
    [request setDelegate:self];
    [request setDidFailSelector:@selector(uploadRequestDidFail:)];
    [request setDidFinishSelector:@selector(uploadRequestDidFinish:)];
    [request setUploadProgressDelegate:self];
    [request setTimeOutSeconds:120.0];
    
    NSData *regularData = [NSData dataWithContentsOfURL:regUrl];
    NSData *smallData = [NSData dataWithContentsOfURL:smallUrl];
    
    [request addData:regularData withFileName:@"regular" andContentType:@"video/mov" forKey:@"regular"];
    [request addData:smallData withFileName:@"small" andContentType:@"video/mov" forKey:@"small"];
    
    [request startAsynchronous];
}

- (void)startGifRequest:(double)frameDelay {
    NSLog( @"startGifRequest with %u frames", [frames count] );
    
    [self setIsUploading:YES];
    [self setUploadProgress:0];
    
    
    NSURL *url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/snap.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:@"blah" forKey:@"blah"];
    [request setDelegate:self];
    [request setDidFailSelector:@selector(uploadRequestDidFail:)];
    [request setDidFinishSelector:@selector(uploadRequestDidFinish:)];
    [request setUploadProgressDelegate:self];
    [request setTimeOutSeconds:120.0];
    for ( int i=0; i<[frames count]; i++ ) {
		NSString *filename = [NSString stringWithFormat:@"%@%i%@", @"frame", i, @".jpg"];
        NSData *imageData = [frames objectAtIndex:i];
        [request addData:imageData withFileName:filename andContentType:@"image/jpeg" forKey:@"Filedata[]"];
	}
    NSString *frameDelayString = [NSString stringWithFormat:@"%f", frameDelay ];
    [request setPostValue:frameDelayString forKey:@"frameDelay"];
    
    [request startAsynchronous];
//    [request startSynchronous];
}

- (void)setDoubleValue:(double)doubleValue {
//    NSLog( @"setDoubleValue: %f", doubleValue );
    [self setUploadProgress:doubleValue];
}

- (void)uploadRequestDidFinish:(ASIHTTPRequest *)request {
    NSLog( @"uploadRequestDidFinish: %@", [request responseString] );
    
    [self setIsUploading:NO];
    
    NSString *baseUrl = @"http://aaron-meyers.com/smirnoff/";
    
    NSError *error;
    NSDictionary *dictionary = [[CJSONDeserializer deserializer] deserializeAsDictionary:[request responseData] error:&error];
    NSString *imageLocation = [dictionary valueForKey:@"image_location"];
    NSDictionary *animInfo = [dictionary valueForKey:@"data"];
    NSString *hash = [animInfo valueForKey:@"hash"];
//    [self setLastHash:[animInfo valueForKey:@"hash"]];
    lastGifUrl = [NSString stringWithFormat:@"%@%@%@.gif", baseUrl, imageLocation, hash];
    
//    std::string hi = "";
    sup = [[self lastGifUrl] cStringUsingEncoding:[NSString defaultCStringEncoding]];
    lastHash = [[animInfo valueForKey:@"hash"] cStringUsingEncoding:[NSString defaultCStringEncoding]];
    
    [frames removeAllObjects];
    
    
//    self.app->uploadFinished();
//    self.app->isUploading = false;
//    uploadMessage = [[helper lastGifUrl] cStringUsingEncoding:[NSString defaultCStringEncoding]];
}

- (void)uploadRequestDidFail:(ASIHTTPRequest *)request {
    NSLog( @"uploadRequestDidFail: %@", [[request error] localizedDescription] );
    
    [self setIsUploading:NO];
    [self setUploadFailed:YES];
    lastGifUrl = @"the upload failed :(";
    
    [frames removeAllObjects];
}

- (NSData*)getDataFromPixels:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height {
    
    NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&pixels pixelsWide:width pixelsHigh:height bitsPerSample:8 samplesPerPixel:3 hasAlpha:NO isPlanar:NO colorSpaceName:NSDeviceRGBColorSpace bytesPerRow:0 bitsPerPixel:0];
    
    NSDictionary *imageProps = [NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:.7] forKey:NSImageCompressionFactor];
    NSData *imageData = [imageRep representationUsingType:NSJPEGFileType properties:imageProps];
    
    [imageRep release];
    
    return imageData;
}

- (void)addFrame:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height {
    [frames addObject:[self getDataFromPixels:pixels withWidth:width andHeight:height]];
}

- (void)savePngFrom:(unsigned char *)pixels withWidth:(int)width andHeight:(int)height {
    
    NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&pixels pixelsWide:width pixelsHigh:height bitsPerSample:8 samplesPerPixel:3 hasAlpha:NO isPlanar:NO colorSpaceName:NSDeviceRGBColorSpace bytesPerRow:0 bitsPerPixel:0];
    // create an NSData object from the NSBitmapImageRep
    NSDictionary *imageProps = [NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:.7] forKey:NSImageCompressionFactor];
    NSData *imageData = [imageRep representationUsingType:NSJPEGFileType properties:imageProps];
    // write the image to the desktop
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDesktopDirectory, NSUserDomainMask, YES);
    NSString * desktopPath = [paths objectAtIndex:0];
    [imageData writeToFile:[NSString stringWithFormat:@"%@/aaron.jpg", desktopPath] atomically:NO]; 
    
    [imageRep release];
}

- (void)sendEmailTo:(NSString *)emailAddress {
    
    NSURL *url = [NSURL URLWithString:@"http://aaron-meyers.com/smirnoff/sendEmail.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request setPostValue:emailAddress forKey:@"email"];
    NSString *hashString = [NSString stringWithCString:lastHash.c_str() encoding:[NSString defaultCStringEncoding]];
    [request setPostValue:hashString forKey:@"hash"];
    
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(sendEmailRequestDidFinish:)];
    [request setDidFailSelector:@selector(sendEmailRequestDidFail:)];
    
    [request startAsynchronous];
    [self setIsEmailing:YES];
}

- (void)sendEmailRequestDidFinish:(ASIHTTPRequest *)request {
    NSLog( @"sendEmailRequestDidFinish: %@", [request responseString] );
    [self setIsEmailing:NO];
}

- (void)sendEmailRequestDidFail:(ASIHTTPRequest *)request {
    NSLog( @"sendEmailRequestDidFail: %@", [[request error] localizedDescription] );
    [self setIsEmailing:NO];
}

@end
