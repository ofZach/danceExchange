#include "EmailView.h"

EmailView::EmailView() {
    
    ofAddListener( ofEvents.keyPressed, this, &EmailView::keyPressed );
    verdana.loadFont( "verdana.ttf", 24, true );
    tradeGothic.loadFont( "TradeGothicLTStd-BdCn20.otf", 100 );
    emailAddress = "";
    
	utf8_helper.setup();
}

EmailView::~EmailView() {
    
    ofRemoveListener( ofEvents.keyPressed, this, &EmailView::keyPressed );
    
}

void EmailView::keyPressed( ofKeyEventArgs & args ) {
    
	
	bool bSendOn = true;
	
	switch (args.key){
			
		case OF_KEY_F1			:
		case OF_KEY_F2			:
		case OF_KEY_F3			:
		case OF_KEY_F4			:
		case OF_KEY_F5			:
		case OF_KEY_F6			:
		case OF_KEY_F7			:
		case OF_KEY_F8			:
		case OF_KEY_F9			:
		case OF_KEY_F10			:
		case OF_KEY_F11			:
		case OF_KEY_F12			:
		case OF_KEY_LEFT			:
		case OF_KEY_UP			:
		case OF_KEY_RIGHT			:
		case OF_KEY_DOWN			:
		case OF_KEY_PAGE_DOWN			:
		case OF_KEY_PAGE_UP			:
		case OF_KEY_HOME			:
		case OF_KEY_END			:
		case OF_KEY_INSERT			:
			bSendOn = false;
			break;
			
	}
	
	
	if (bSendOn == true){
		utf8_helper.keyPressed(args.key);
	}
	
	
	
	
	// cout << "EmailView::keyPressed: " << (char)args.key << " with code: " << args.key << endl;
    //if ( args.key == OF_KEY_BACKSPACE || args.key == OF_KEY_DEL ) {
//        int stringLength = strlen( emailAddress.c_str() );
//        if ( stringLength > 0 )
//            emailAddress = emailAddress.substr( 0, strlen( emailAddress.c_str() ) - 1 );
//    } // handled in typer
	
   if ( args.key == OF_KEY_RETURN ) {
		string email = utf8_helper.getString();
	   cout << email <<endl;
        ofNotifyEvent( emailAddressEnteredEvent,  email );
    }
    else {
        //emailAddress += (char)args.key;  // handled in typer
    }
    
}


void EmailView::init() {
    
}

void EmailView::update() {
    
}

void EmailView::draw() {
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    float targetWidth = aspectWidth * .75;
    
    ofSetColor( 255, 255, 255, 200 );
    ofPushMatrix();
    ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
    int theWidth = aspectWidth * .75;
    int theHeight = ofGetHeight() * .2;
    ofRect( -theWidth/2, -theHeight/2, theWidth, theHeight );
    
    ofRectangle heightRect = tradeGothic.getStringBoundingBox( "j", 0, 0 );
    ofRectangle rect = tradeGothic.getStringBoundingBox( utf8_helper.getString(), 0, 0 );
    rect.height = heightRect.height;

    float scaleFactor = (ofGetHeight()*.2*.5) / (float)rect.height;
    if ( rect.width > targetWidth * .75 ) {
        scaleFactor = (targetWidth *.875) / (float)rect.width;
    }
    
    rect.x *= scaleFactor;  rect.y *= scaleFactor;
    rect.width *= scaleFactor;  rect.height *= scaleFactor;
    ofTranslate( -rect.x - rect.width/2.0, rect.height*.5 );
    ofScale( scaleFactor, scaleFactor );
    
    ofSetColor( 0, 0, 0 );
    tradeGothic.drawString( utf8_helper.getString(), 0, 0 );
    
    ofPopMatrix();
}