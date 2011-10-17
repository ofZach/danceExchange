
#include "ofxQtVideoSaverH264.h"
#include <QuickTime/QuickTime.h>


//#define	ASK_SEQUENCE_GRABBER_TO_CONVERT_AUDIO_TO_AAC		0


// defines
#define BailErr(x) {err = x; if(err != noErr) goto bail;}
#define DisplayAndBail(x, y) {pMungData->err = x; if(x != noErr) { displayError(pMungData->pWindow, y, x); goto bail;}}

// constants
const EventTime kTimerInterval = kEventDurationSecond / 60;	// idle timer interval

// mung data struct
typedef struct {
    WindowRef					pWindow;	// window
	int							width;		// dest width
	int							height;		// dest height
	CodecType					codecType;	// codec
	SInt32						averageDataRate; // data rate
	HIViewRef					imageView;	// for displaying decoded frames
	ICMDecompressionSessionRef	decompressionSession; // decompresses and scales captured frames
	ICMCompressionSessionRef	compressionSession; // compresses frames
	Movie						outputMovie; // movie file for storing compressed frames
	Media						outputVideoMedia; // media for our video track in the movie
	DataHandler					outputMovieDataHandler; // storage for movie header
	CFURLRef					outputMovieURL;	// URL to the movie file (once we create it)
	Boolean						didBeginVideoMediaEdits;
	Boolean						verbose;
    SeqGrabComponent			seqGrab;	// sequence grabber
    TimeValue					lastTime;
    TimeScale					timeScale;
	int							desiredFramesPerSecond;
	TimeValue					minimumFrameDuration;
    long						frameCount;
    Boolean						isGrabbing;
    EventLoopTimerRef			timerRef;
    OSErr						err;
	
	SGChannel					audioChan;
	Media						soundMedia;
	Boolean						didBeginSoundMediaEdits;
    TimeScale                   audioTimeScale;
    SoundDescriptionHandle      audioDescH;
    AudioStreamBasicDescription asbd;
} MungDataRecord, *MungDataPtr;

static MungDataPtr initializeMungData(Rect inBounds);
static void disposeMungData(MungDataPtr pMungData);

static OSStatus setUpOutputMovie( MungDataPtr pMungData );
static void finishOutputMovie( MungDataPtr pMungData );
static void openOutputMovie( MungDataPtr pMungData );




static OSStatus 
writeEncodedFrameToMovie( void *encodedFrameOutputRefCon, 
						 ICMCompressionSessionRef session, 
						 OSStatus err,
						 ICMEncodedFrameRef encodedFrame,
						 void *reserved );
static OSStatus
createCompressionSession( 
						 int width, int height, CodecType codecType, SInt32 averageDataRate, TimeScale timeScale, 
						 ICMEncodedFrameOutputCallback outputCallback, void *outputRefCon,
						 ICMCompressionSessionRef *compressionSessionOut );


void testMe(){
	
	
	unsigned char data[320*240*3];
	
	for (int j = 0; j < 320*240*3; j++){
		data[j] = ofRandom(0,255);	
	}
	
	Rect portRect = { 0, 0, 240, 320 };
	
	MungDataPtr pMungData = initializeMungData(portRect);
	
	createCompressionSession( 
							 pMungData->width, pMungData->height, pMungData->codecType, 
							 pMungData->averageDataRate, pMungData->timeScale,
							 writeEncodedFrameToMovie, pMungData,
							 &pMungData->compressionSession );
	
	
	
	CVPixelBufferRef buffer = NULL;
 
	CVPixelBufferCreate(
						kCFAllocatorDefault,
						320,240, k24RGBPixelFormat,
						NULL /* pixel_buffer_attributes */,
						&buffer
						);

	
	
    
    //CVPixelBufferUnlockBaseAddress(buffer, 0);
    
	
	
	
	
	
	
	
	if( buffer ) {
		
		
		for (int i = 0; i < 100; i++){
			CVPixelBufferLockBaseAddress( buffer, 0 );
			
			void* base_address = CVPixelBufferGetBaseAddress( buffer );
			unsigned char * temp = (unsigned char *) base_address;
			size_t bytes_per_row = CVPixelBufferGetBytesPerRow( buffer );
			
				for (int j = 0; j < 320*240*3; j++){
					temp[j] = data[j] + i;
				}
		
			//cvSetData( image_rgb, base_address, bytes_per_row );
			//cvConvertImage( image, image_rgb, CV_CVTIMG_SWAP_RB )
			CVPixelBufferUnlockBaseAddress( buffer, 0 );
			
			
			ICMCompressionFrameOptionsRef frameOptions = NULL;
						
			// Draw in the corner of the pixelBuffer.
			//drawOnPixelBuffer( pixelBuffer, displayTime, pMungData->timeScale );
			
			// Display the frame in the imageView.
			//image = createCGImageFrom32XRGBCVPixelBuffer( pixelBuffer );
			//putCGImageInHIImageView( image, pMungData->imageView );
			//if( image ) CFRelease( image );
			
			
		
			
		
			TimeValue64 displayDuration = 600.0 / 120.0;
			ICMValidTimeFlags validTimeFlags = kICMValidTime_DisplayDurationIsValid;
			SInt64 timeStamp = ICMCompressionSessionGetTimeScale( pMungData->compressionSession ) * (1.0/60.0);
			// Feed the frame to the compression session.
			OSStatus err = ICMCompressionSessionEncodeFrame( pMungData->compressionSession, buffer,
												   i*timeStamp, 0, kICMValidTime_DisplayTimeStampIsValid,
												    nil, nil, nil );
			
			/*
			 SInt64 timeStamp = ICMCompressionSessionGetTimeScale( _compressionSession ) * (1.0/60.0);
			 
			 timeStamp*_currentFrame,0,kICMValidTime_DisplayTimeStampIsValid, NULL,
			*/
			
			
			if( err ) {
				fprintf( stderr, "ICMCompressionSessionEncodeFrame() failed (%d)\n", err );
			}
		}
	}
	
	
	
	if(pMungData) {
        //ICMDecompressionSessionRelease( pMungData->decompressionSession );
		
		// It is important to push out any remaining frames before we release the compression session.
		// If we knew the timestamp following the last source frame, you should pass it in here.
		ICMCompressionSessionCompleteFrames( pMungData->compressionSession, true, 0, 0 );
		ICMCompressionSessionRelease( pMungData->compressionSession );
		
		if( pMungData->outputMovie ) {
			finishOutputMovie( pMungData );
			openOutputMovie( pMungData );
		}
		
		if( pMungData->outputMovieURL )
			CFRelease( pMungData->outputMovieURL );
        free((Ptr)pMungData);
        pMungData = NULL;
    }
	
}

// --------------------
// initializeMungData
//
static MungDataPtr initializeMungData(Rect inBounds)
{
	MungDataPtr pMungData = NULL;
	HIViewRef contentView = NULL;
	HIViewID kImageID = { 'IMAG', 0 };
    
    OSErr err = noErr;
    
    // allocate memory for the data
    pMungData = (MungDataPtr)calloc(sizeof(MungDataRecord), 1);
    if (MemError() || NULL == pMungData ) return NULL;
    static  int TIME_SCALE = 600;
	
	//pMungData->pWindow = inWindow;
	pMungData->width = inBounds.right - inBounds.left;
	pMungData->height = inBounds.bottom - inBounds.top;
	pMungData->codecType = kH264CodecType;
	
	pMungData->timeScale = TIME_SCALE;
	pMungData->averageDataRate = 100000; // 100 kbyte/sec == 800 kbit/sec
	pMungData->desiredFramesPerSecond = 120;
	pMungData->verbose = true;
	
	//GetRootControl( pMungData->pWindow, &contentView );
	//HIViewFindByID( contentView, kImageID, &pMungData->imageView );
	
	err = setUpOutputMovie( pMungData );
	if( err )
		goto bail;
	
	return pMungData;
bail:
	// clean up and get out
	if (pMungData) {
		disposeMungData(pMungData);
	}
	
	return NULL;
}

// --------------------
// disposeMungData   END SHIT
//
static void disposeMungData(MungDataPtr pMungData)
{
    // clean up the bits
    if(pMungData) {
        if (pMungData->seqGrab)
    		CloseComponent(pMungData->seqGrab);
		ICMDecompressionSessionRelease( pMungData->decompressionSession );
		
		// It is important to push out any remaining frames before we release the compression session.
		// If we knew the timestamp following the last source frame, you should pass it in here.
		ICMCompressionSessionCompleteFrames( pMungData->compressionSession, true, 0, 0 );
		ICMCompressionSessionRelease( pMungData->compressionSession );
		
		if( pMungData->outputMovie ) {
			finishOutputMovie( pMungData );
			openOutputMovie( pMungData );
		}
		
		if( pMungData->outputMovieURL )
			CFRelease( pMungData->outputMovieURL );
		
        DisposeHandle((Handle)pMungData->audioDescH);
        
        free((Ptr)pMungData);
        pMungData = NULL;
    }
}

static OSStatus promptForOutputMovieDataRef( Handle *dataRefOut, OSType *dataRefTypeOut )
{
	OSStatus err = noErr;
	NavDialogCreationOptions navOptions = { kNavDialogCreationOptionsVersion, 0, };
	NavDialogRef navDialog = NULL;
	NavReplyRecord navReply = { kNavReplyRecordVersion, };
	AEDesc actualDesc = { 0, 0 };
	FSRef parentFSRef;
	
	err = NavGetDefaultDialogCreationOptions( &navOptions );
	if( err )
		goto bail;
	navOptions.windowTitle = CFSTR("Save Captured Movie As...");
	navOptions.message = CFSTR("Pick where to save the captured and compressed movie.  (Make sure your camera is plugged in!)");
	navOptions.saveFileName = CFSTR("captured.mov");
	navOptions.modality = kWindowModalityAppModal;
	
	err = NavCreatePutFileDialog( &navOptions, MovieFileType, 'TVOD', NULL, NULL, &navDialog );
	if( err )
		goto bail;
	
	err = NavDialogRun( navDialog );
	if( err )
		goto bail;
	
	if( kNavUserActionSaveAs != NavDialogGetUserAction( navDialog ) ) {
		err = userCanceledErr;
		goto bail;
	}
	
	err = NavDialogGetReply( navDialog, &navReply );
	if( err ) 
		goto bail;
	
	err = AECoerceDesc( &navReply.selection, typeFSRef, &actualDesc );
	if( err )
		goto bail;
	
	err = AEGetDescData( &actualDesc, &parentFSRef, sizeof( FSRef ) );
	if( err )
		goto bail;
	
	err = QTNewDataReferenceFromFSRefCFString( &parentFSRef, navReply.saveFileName, 0, dataRefOut, dataRefTypeOut );
	if( err ) {
		fprintf( stderr, "QTNewDataReferenceFromFSRefCFString() failed (%d)\n", err );
		goto bail;
	}
	
bail:
	NavDisposeReply( &navReply );
	NavDialogDispose( navDialog );
	AEDisposeDesc( &actualDesc );
	return err;
}

static OSStatus setUpOutputMovie( MungDataPtr pMungData )
{
	OSStatus err = noErr;
	Handle outputMovieDataRef = NULL;
	OSType outputMovieDataRefType = 0;
	CFStringRef outputMovieFullPathString = NULL;
	
	// Prompt the user for an output file.
	err = promptForOutputMovieDataRef( &outputMovieDataRef, &outputMovieDataRefType );
	if( err )
		goto bail;
	
	// Create a new movie file. 
	// If you're using CreateMovieFile, consider switching to CreateMovieStorage, which is long-file-name aware.
	err = CreateMovieStorage( outputMovieDataRef, outputMovieDataRefType, 'TVOD', 0, 
							 createMovieFileDeleteCurFile, &pMungData->outputMovieDataHandler, &pMungData->outputMovie );
	if( err ) {
		fprintf( stderr, "CreateMovieStorage failed (%d)\n", err );
		goto bail;
	}
	
	// Remember a URL for the movie file so we can open it in QuickTime Player afterwards.
	err = QTGetDataReferenceFullPathCFString( outputMovieDataRef, outputMovieDataRefType, 
											 kQTPOSIXPathStyle, &outputMovieFullPathString );
	if( err )
		goto bail;
	
	pMungData->outputMovieURL = CFURLCreateWithFileSystemPath( NULL, outputMovieFullPathString, kCFURLPOSIXPathStyle, false );
	
bail:
	DisposeHandle( outputMovieDataRef );
	if( outputMovieFullPathString )
		CFRelease( outputMovieFullPathString );
 	return err;
}

static void finishOutputMovie(MungDataPtr pMungData)
{
	OSStatus err = noErr;
	Track videoTrack = NULL;
	
	if( pMungData->didBeginVideoMediaEdits ) {
		// End the media sample-adding session.
		err = EndMediaEdits( pMungData->outputVideoMedia );
		if( err ) {
			fprintf( stderr, "EndMediaEdits() failed (%d)\n", err );
			goto bail;
		}
	}
	
	
	// Make sure things are extra neat.
	ExtendMediaDecodeDurationToDisplayEndTime( pMungData->outputVideoMedia, NULL );
	
	// Insert the stuff we added into the track, at the end.
	videoTrack = GetMediaTrack( pMungData->outputVideoMedia );
	err = InsertMediaIntoTrack( videoTrack, 
							   GetTrackDuration(videoTrack), 
							   0, GetMediaDisplayDuration( pMungData->outputVideoMedia ), // NOTE: use this instead of GetMediaDuration
							   fixed1 );
	if( err ) {
		fprintf( stderr, "InsertMediaIntoTrack() failed (%d)\n", err );
		goto bail;
	}
	
	
	
	// Write the movie header to the file.
	err = AddMovieToStorage( pMungData->outputMovie, pMungData->outputMovieDataHandler );
	if( err ) {
		fprintf( stderr, "AddMovieToStorage() failed (%d)\n", err );
		goto bail;
	}
	
	CloseMovieStorage( pMungData->outputMovieDataHandler );
	pMungData->outputMovieDataHandler = 0;
	
	DisposeMovie( pMungData->outputMovie );
	
bail:
	return;
}

// cool !! 
 static void openOutputMovie( MungDataPtr pMungData )
 {
 OSStatus err = noErr;
 
 // Open the movie we've created in QuickTime Player.
 err = LSOpenCFURLRef( pMungData->outputMovieURL, NULL );
 
 bail:
 return;
 }




static OSStatus
createCompressionSession( 
						 int width, int height, CodecType codecType, SInt32 averageDataRate, TimeScale timeScale, 
						 ICMEncodedFrameOutputCallback outputCallback, void *outputRefCon,
						 ICMCompressionSessionRef *compressionSessionOut )
{
	OSStatus err = noErr;
	ICMEncodedFrameOutputRecord encodedFrameOutputRecord = {0};
	ICMCompressionSessionOptionsRef sessionOptions = NULL;
	
	err = ICMCompressionSessionOptionsCreate( NULL, &sessionOptions );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionOptionsCreate() failed (%d)\n", err );
		goto bail;
	}
	
	// We must set this flag to enable P or B frames.
	err = ICMCompressionSessionOptionsSetAllowTemporalCompression( sessionOptions, true );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionOptionsSetAllowTemporalCompression() failed (%d)\n", err );
		goto bail;
	}
	
	// We must set this flag to enable B frames.
	err = ICMCompressionSessionOptionsSetAllowFrameReordering( sessionOptions, true );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionOptionsSetAllowFrameReordering() failed (%d)\n", err );
		goto bail;
	}
	
	// Set the maximum key frame interval, also known as the key frame rate.
	err = ICMCompressionSessionOptionsSetMaxKeyFrameInterval( sessionOptions, 30 );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionOptionsSetMaxKeyFrameInterval() failed (%d)\n", err );
		goto bail;
	}
	
	// This allows the compressor more flexibility (ie, dropping and coalescing frames).
	err = ICMCompressionSessionOptionsSetAllowFrameTimeChanges( sessionOptions, true );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionOptionsSetAllowFrameTimeChanges() failed (%d)\n", err );
		goto bail;
	}
	
	// We need durations when we store frames.
	err = ICMCompressionSessionOptionsSetDurationsNeeded( sessionOptions, true );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionOptionsSetDurationsNeeded() failed (%d)\n", err );
		goto bail;
	}
	
	// Set the average data rate.
	err = ICMCompressionSessionOptionsSetProperty( sessionOptions, 
												  kQTPropertyClass_ICMCompressionSessionOptions,
												  kICMCompressionSessionOptionsPropertyID_AverageDataRate,
												  sizeof( averageDataRate ),
												  &averageDataRate );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionOptionsSetProperty(AverageDataRate) failed (%d)\n", err );
		goto bail;
	}
	
	encodedFrameOutputRecord.encodedFrameOutputCallback = outputCallback;
	encodedFrameOutputRecord.encodedFrameOutputRefCon = outputRefCon;
	encodedFrameOutputRecord.frameDataAllocator = NULL;
	
	err = ICMCompressionSessionCreate( NULL, width, height, codecType, timeScale,
									  sessionOptions, NULL, &encodedFrameOutputRecord, compressionSessionOut );
	if( err ) {
		fprintf( stderr, "ICMCompressionSessionCreate() failed (%d)\n", err );
		goto bail;
	}
	
bail:
	ICMCompressionSessionOptionsRelease( sessionOptions );
	
	return err;
}

// Utility to wrap a CGBitmapContext around a 32ARGB CVPixelBuffer.
static OSStatus
createCGBitmapContextFor32ARGBCVPixelBuffer(
											CVPixelBufferRef pixelBuffer,
											CGContextRef *contextOut )
{
	size_t width, height, rowBytes;
	void *baseAddr;
	OSStatus err;
	CGColorSpaceRef colorspace = NULL;
	CGContextRef context = NULL;
	
	err = CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
	if( err ) {
		fprintf( stderr, "CVPixelBufferLockBaseAddress() failed (%d)\n", err );
		goto bail;
	}
	rowBytes = CVPixelBufferGetBytesPerRow( pixelBuffer );
	baseAddr = CVPixelBufferGetBaseAddress( pixelBuffer );
	width = CVPixelBufferGetWidth( pixelBuffer );
	height = CVPixelBufferGetHeight( pixelBuffer );
	
	colorspace = CGColorSpaceCreateDeviceRGB();
	
	context = CGBitmapContextCreate( baseAddr, width, height, 
									8, rowBytes, colorspace, kCGImageAlphaPremultipliedFirst );
	
	*contextOut = context;
	context = NULL;
	
bail:
	if( colorspace ) CGColorSpaceRelease( colorspace );
	if( context ) CGContextRelease( context );
	return err;
}

static void
unlockCVPixelBufferAndReleaseCGBitmapContext( 
											 CVPixelBufferRef pixelBuffer,
											 CGContextRef context )
{
	CGContextRelease( context );
	CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
}

// Utility to draw a string left- or right- justified with a stroked border in one color, filled with another color.
enum Justify {
	kLeftJustify, kRightJustify
};
static void showString( CGContextRef c, float x, float y, enum Justify justify, const char *str)
{
	CGPoint after;
	CGContextSaveGState( c );
	if( kRightJustify == justify ) {
		CGContextSetTextPosition( c, 0, 0 );
		CGContextSetTextDrawingMode( c, kCGTextInvisible );
		CGContextShowText( c, str, strlen( str ) );
		after = CGContextGetTextPosition( c );
		x = x - after.x;
		y = y - after.y;
	}
	CGContextSetTextPosition( c, x, y );
	CGContextSetTextDrawingMode( c, kCGTextFillStroke );
	CGContextShowText( c, str, strlen( str ) );
	CGContextRestoreGState( c );
}

// Draw "LIVE" and the number of seconds into the capture on the pixel buffer.
static void drawOnPixelBuffer( CVPixelBufferRef pixelBuffer, TimeValue64 displayTime, TimeScale timeScale )
{
	CGContextRef context = NULL;
	if( noErr == createCGBitmapContextFor32ARGBCVPixelBuffer( pixelBuffer, &context ) ) {
		int width = CVPixelBufferGetWidth( pixelBuffer );
		int timeInSeconds = displayTime / timeScale;
		char buf[100];
		
		sprintf( buf, "%d:%02d", timeInSeconds / 60, timeInSeconds % 60 );
		
		CGContextSelectFont( context, "Arial-Black", 32, kCGEncodingMacRoman );
		CGContextSetLineWidth( context, 1.5 );
		CGContextSetRGBStrokeColor( context, 1.0, 1.0, 1.0, 1.0 ); // white
		CGContextSetRGBFillColor( context, 0.2, 0.0, 0.0, 1.0 ); // dark red
		showString( context, 10, 10, kLeftJustify, "LIVE" );
		CGContextSetRGBFillColor( context, 0.0, 0.0, 0.3, 1.0 ); // dark blue
		showString( context, width-10, 10, kRightJustify, buf );
		
		unlockCVPixelBufferAndReleaseCGBitmapContext( pixelBuffer, context );
	}
}

// Utility to wrap a CGImage around a 32ARGB CVPixelBuffer.

static void releaseAndUnlockThis( void *info, const void *data, size_t size )
{
	CVPixelBufferRef pixelBuffer = ((CVPixelBufferRef)info);
	
	CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
	CVBufferRelease( pixelBuffer );
}

static CGImageRef createCGImageFrom32XRGBCVPixelBuffer( CVPixelBufferRef pixelBuffer )
{
	size_t width, height, rowBytes;
	void *baseAddr = NULL;
	CGColorSpaceRef colorspace = NULL;
	CGDataProviderRef provider = NULL;
	CGImageRef image = NULL;
	
	CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
	
	rowBytes = CVPixelBufferGetBytesPerRow( pixelBuffer );
	baseAddr = CVPixelBufferGetBaseAddress( pixelBuffer );
	width = CVPixelBufferGetWidth( pixelBuffer );
	height = CVPixelBufferGetHeight( pixelBuffer );
	
	colorspace = CGColorSpaceCreateDeviceRGB();
	CVBufferRetain( pixelBuffer );
	provider = CGDataProviderCreateWithData( pixelBuffer, baseAddr, rowBytes * height, releaseAndUnlockThis );
	image = CGImageCreate( width, height, 8, 32, rowBytes, colorspace, 
						  kCGImageAlphaNoneSkipFirst, provider, NULL, true, kCGRenderingIntentDefault );
	
bail:
	if( provider ) CGDataProviderRelease( provider );
	if( colorspace ) CGColorSpaceRelease( colorspace );
	return image;
}

// Replace the image in an HIImageView and force it to be displayed immediately.
static void putCGImageInHIImageView( CGImageRef image, HIViewRef imageView )
{
	OSStatus err;
	if( imageView ) {
		// Pass our view the new image
		err = HIImageViewSetImage( imageView, image );
		err = HIViewSetNeedsDisplay( imageView, true );
		err = HIViewRender( imageView );
		err = HIWindowFlush( HIViewGetWindow( imageView ) );
	}
}

/*
 // The tracking callback function for the decompression session.
 // Draw on pixel buffers, display them in the window and feed them to the compression session.
 static void 
 displayAndCompressFrame( 
 void *decompressionTrackingRefCon,
 OSStatus result,
 ICMDecompressionTrackingFlags decompressionTrackingFlags,
 CVPixelBufferRef pixelBuffer,
 TimeValue64 displayTime,
 TimeValue64 displayDuration,
 ICMValidTimeFlags validTimeFlags,
 void *reserved,
 void *sourceFrameRefCon )
 {
 OSStatus err = noErr;
 MungDataPtr pMungData = (MungDataPtr)decompressionTrackingRefCon;
 
 if( kICMDecompressionTracking_ReleaseSourceData & decompressionTrackingFlags ) {
 // if we were responsible for managing source data buffers, we should release the source buffer here,
 // using sourceFrameRefCon to identify it.
 }
 if( ( kICMDecompressionTracking_EmittingFrame & decompressionTrackingFlags ) && pixelBuffer ) {
 ICMCompressionFrameOptionsRef frameOptions = NULL;
 CGImageRef image = NULL;
 
 // Draw in the corner of the pixelBuffer.
 drawOnPixelBuffer( pixelBuffer, displayTime, pMungData->timeScale );
 
 // Display the frame in the imageView.
 image = createCGImageFrom32XRGBCVPixelBuffer( pixelBuffer );
 putCGImageInHIImageView( image, pMungData->imageView );
 if( image ) CFRelease( image );
 
 // Feed the frame to the compression session.
 err = ICMCompressionSessionEncodeFrame( pMungData->compressionSession, pixelBuffer,
 displayTime, displayDuration, validTimeFlags,
 frameOptions, NULL, NULL );
 if( err ) {
 fprintf( stderr, "ICMCompressionSessionEncodeFrame() failed (%d)\n", err );
 }
 }
 }
 */
// Create a video track and media to hold encoded frames.
// This is called the first time we get an encoded frame back from the compression session.
static OSStatus
createVideoMedia( 
				 MungDataPtr pMungData,
				 ImageDescriptionHandle imageDesc,
				 TimeScale timescale )
{
	OSStatus err = noErr;
	Fixed trackWidth, trackHeight;
	Track outputTrack = NULL;
	
	err = ICMImageDescriptionGetProperty( 
										 imageDesc,
										 kQTPropertyClass_ImageDescription, 
										 kICMImageDescriptionPropertyID_ClassicTrackWidth,
										 sizeof( trackWidth ),
										 &trackWidth,
										 NULL );
	if( err ) {
		fprintf( stderr, "ICMImageDescriptionGetProperty(kICMImageDescriptionPropertyID_DisplayWidth) failed (%d)\n", err );
		goto bail;
	}
	
	err = ICMImageDescriptionGetProperty( 
										 imageDesc,
										 kQTPropertyClass_ImageDescription, 
										 kICMImageDescriptionPropertyID_ClassicTrackHeight,
										 sizeof( trackHeight ),
										 &trackHeight,
										 NULL );
	if( err ) {
		fprintf( stderr, "ICMImageDescriptionGetProperty(kICMImageDescriptionPropertyID_DisplayHeight) failed (%d)\n", err );
		goto bail;
	}
	
	if( pMungData->verbose ) {
		printf( "creating %g x %g track\n", Fix2X(trackWidth), Fix2X(trackHeight) );
	}
	
	outputTrack = NewMovieTrack( pMungData->outputMovie, trackWidth, trackHeight, 0 );
	err = GetMoviesError();
	if( err ) {
		fprintf( stderr, "NewMovieTrack() failed (%d)\n", err );
		goto bail;
	}
	
	pMungData->outputVideoMedia = NewTrackMedia( outputTrack, VideoMediaType, timescale, 0, 0 );
	err = GetMoviesError();
	if( err ) {
		fprintf( stderr, "NewTrackMedia() failed (%d)\n", err );
		goto bail;
	}
	
	err = BeginMediaEdits( pMungData->outputVideoMedia );
	if( err ) {
		fprintf( stderr, "BeginMediaEdits() failed (%d)\n", err );
		goto bail;
	}
	pMungData->didBeginVideoMediaEdits = true;
	
bail:
	return err;
}

// This is the tracking callback function for the compression session.
// Write the encoded frame to the movie file.
// Note that this function adds each sample separately; better chunking can be achieved
// by flattening the movie after it is finished, or by grouping samples, writing them in 
// groups to the data reference manually, and using AddSampleTableToMedia.
static OSStatus 
writeEncodedFrameToMovie( void *encodedFrameOutputRefCon, 
						 ICMCompressionSessionRef session, 
						 OSStatus err,
						 ICMEncodedFrameRef encodedFrame,
						 void *reserved )
{
	MungDataPtr pMungData = (MungDataPtr)encodedFrameOutputRefCon;
	ImageDescriptionHandle imageDesc = NULL;
	TimeValue64 decodeDuration;
	
	if( err ) {
		fprintf( stderr, "writeEncodedFrameToMovie received an error (%d)\n", err );
		goto bail;
	}
	
	err = ICMEncodedFrameGetImageDescription( encodedFrame, &imageDesc );
	if( err ) {
		fprintf( stderr, "ICMEncodedFrameGetImageDescription() failed (%d)\n", err );
		goto bail;
	}
	
	if( ! pMungData->outputVideoMedia ) {
		err = createVideoMedia( pMungData, imageDesc, ICMEncodedFrameGetTimeScale( encodedFrame ) );
		if( err ) 
			goto bail;
	}
	
	decodeDuration = ICMEncodedFrameGetDecodeDuration( encodedFrame );
	if( decodeDuration == 0 ) {
		// You can't add zero-duration samples to a media.  If you try you'll just get invalidDuration back.
		// Because we don't tell the ICM what the source frame durations are,
		// the ICM calculates frame durations using the gaps between timestamps.
		// It can't do that for the final frame because it doesn't know the "next timestamp"
		// (because in this example we don't pass a "final timestamp" to ICMCompressionSessionCompleteFrames).
		// So we'll give the final frame our minimum frame duration.
		decodeDuration = pMungData->minimumFrameDuration * ICMEncodedFrameGetTimeScale( encodedFrame ) / pMungData->timeScale;
	}
	
	if( pMungData->verbose ) {
		printf( "adding %ld byte sample: decode duration %ld, display offset %ld, flags %#lx", 
			   (long)ICMEncodedFrameGetDataSize( encodedFrame ),
			   (long)decodeDuration, 
			   (long)ICMEncodedFrameGetDisplayOffset( encodedFrame ),
			   (long)ICMEncodedFrameGetMediaSampleFlags( encodedFrame ) );
		if( true ) {
			ICMValidTimeFlags validTimeFlags = ICMEncodedFrameGetValidTimeFlags( encodedFrame );
			if( kICMValidTime_DecodeTimeStampIsValid & validTimeFlags )
				printf( ", decode time stamp %ld", (long)ICMEncodedFrameGetDecodeTimeStamp( encodedFrame ) );
			if( kICMValidTime_DisplayTimeStampIsValid & validTimeFlags )
				printf( ", display time stamp %ld", (long)ICMEncodedFrameGetDisplayTimeStamp( encodedFrame ) );
		}
		printf( "\n" );
	}
	
	err = AddMediaSample2(
						  pMungData->outputVideoMedia,
						  ICMEncodedFrameGetDataPtr( encodedFrame ),
						  ICMEncodedFrameGetDataSize( encodedFrame ),
						  60,
						  ICMEncodedFrameGetDisplayOffset( encodedFrame ),
						  (SampleDescriptionHandle)imageDesc,
						  1,
						  ICMEncodedFrameGetMediaSampleFlags( encodedFrame ),
						  NULL );
	if( err ) {
		fprintf( stderr, "AddMediaSample2() failed (%d)\n", err );
		goto bail;
	}
	
	// Note: if you don't need to intercept any values, you could equivalently call:
	// err = AddMediaSampleFromEncodedFrame( pMungData->outputVideoMedia, encodedFrame, NULL );
	// if( err ) {
	//     fprintf( stderr, "AddMediaSampleFromEncodedFrame() failed (%d)\n", err );
	//     goto bail;
	// }
	
	
bail:
	return err;
}

//static OSStatus
//writeAudioToMovie(MungDataPtr pMungData, UInt8 *p, long len, long *offset, TimeValue time, long chRefCon)
//{
//    OSStatus err = noErr;
//    UInt32 numSamples = 0;
//    
//	if (!pMungData)
//		return -1;
//	
//	if (pMungData->soundMedia == NULL)
//	{
//        Track t;
//        // Get the timescale
//        err = SGGetChannelTimeScale(pMungData->audioChan, &pMungData->audioTimeScale);
//        if ( err ) {
//            fprintf( stderr, "SGGetChannelTimeScale(audioChan) failed (%d)\n", err );
//            goto bail;
//        }
//        
//		// create the sound track
//        t = NewMovieTrack(pMungData->outputMovie, 0, 0, kFullVolume);
//        err = GetMoviesError();
//        if ( err ) {
//            fprintf( stderr, "NewMovieTrack(SoundMediaType) failed (%d)\n", err );
//            goto bail;
//        }
//        
//        pMungData->soundMedia = NewTrackMedia(t, SoundMediaType, pMungData->audioTimeScale, NULL, 0);
//        err = GetMoviesError();
//        if ( err ) {
//            fprintf(stderr, "NewTrackMedia(SoundMediaType) failed (%d)\n", err );
//            goto bail;
//        }
//        
//        err = BeginMediaEdits( pMungData->soundMedia );
//        if( err ) {
//            fprintf( stderr, "BeginMediaEdits(soundMedia) failed (%d)\n", err );
//            goto bail;
//        }
//        pMungData->didBeginSoundMediaEdits = true;
//        
//        // cache the sound sample description
//        err = QTGetComponentProperty(pMungData->audioChan, kQTPropertyClass_SGAudio,
//									 kQTSGAudioPropertyID_SoundDescription, sizeof(pMungData->audioDescH),
//									 &pMungData->audioDescH, NULL);
//        if ( err ) {
//            fprintf( stderr, "QTGetComponentProperty(kQTSGAudioPropertyID_SoundDescription) failed (%d)\n", err );
//            goto bail;
//        }
//        
//        // and get the AudioStreamBasicDescription equivalent (see CoreAudioTypes.h for more info on this struct)
//        err = QTSoundDescriptionGetProperty(pMungData->audioDescH, kQTPropertyClass_SoundDescription,
//											kQTSoundDescriptionPropertyID_AudioStreamBasicDescription,
//											sizeof(pMungData->asbd), &pMungData->asbd, NULL);
//        if ( err ) {
//            fprintf( stderr, "QTSoundDescriptionGetProperty(ASBD) failed (%d)\n", err );
//            goto bail;
//        }
//	}
//    
//	// Handle CBR and VBR audio.  For VBR formats (such as AAC), an array of 
//	// AudioStreamPacketDescriptions accompanies each blob of audio packets (you'll find them
//	// wrapped in a CFDataRef in the sgdataproc's chRefCon parameter).  If you are writing
//	// VBR data, you must call AddMediaSample2 for each AudioStreamPacketDescription.
//	
//	if ( pMungData->asbd.mBytesPerPacket )
//	{
//		numSamples = (len / pMungData->asbd.mBytesPerPacket) * pMungData->asbd.mFramesPerPacket;
//		
//		err = AddMediaSample2(  pMungData->soundMedia,          // the Media
//							  p,                              // const UInt8 * dataIn
//							  len,                            // ByteCount size
//							  1,                              // TimeValue64 decodeDurationPerSample
//							  0,                              // TimeValue64 displayOffset
//							  (SampleDescriptionHandle)pMungData->audioDescH, // SampleDescriptionHandle sampleDescriptionH
//							  numSamples,                     // ItemCount numberOfSamples
//							  0,                              // MediaSampleFlags sampleFlags
//							  NULL );                         // TimeValue64 * sampleDecodeTimeOut
//		if( err ) {
//			fprintf( stderr, "AddMediaSample2(soundMedia) failed (%d)\n", err );
//			goto bail;
//		}
//	}
//	else {
//		AudioStreamPacketDescription * aspd = (chRefCon ? (AudioStreamPacketDescription*)CFDataGetBytePtr((CFDataRef)chRefCon) : NULL);
//		numSamples = CFDataGetLength((CFDataRef)chRefCon) / sizeof(AudioStreamPacketDescription);
//		UInt32 i;
//		
//		for (i = 0; i < numSamples; i++)
//		{
//			err = AddMediaSample2(  pMungData->soundMedia,
//								  p + aspd[i].mStartOffset,
//								  aspd[i].mDataByteSize,
//								  pMungData->asbd.mFramesPerPacket,
//								  0,
//								  (SampleDescriptionHandle)pMungData->audioDescH,
//								  1,
//								  0,
//								  NULL );
//			if ( err ) {
//				fprintf( stderr, "AddMediaSample2(soundMedia) #%lu of %lu failed (%d)\n", i, numSamples, err );
//				goto bail;
//			}
//		}
//		
//		if (chRefCon)
//			CFRelease((CFDataRef)chRefCon);
//	}
//    
//bail:
//    return err;
//}





/*

// Set up a compression session that will compress each frame and call 
// writeEncodedFrameToMovie with each output frame.
err = createCompressionSession( 
							   pMungData->width, pMungData->height, pMungData->codecType, 
							   pMungData->averageDataRate, pMungData->timeScale,
							   writeEncodedFrameToMovie, pMungData,
							   &pMungData->compressionSession );




// initialize our data that's going to be passed around
pMungData = initializeMungData( window, portRect, seqGrab );
BailErr(NULL == pMungData);
*/

