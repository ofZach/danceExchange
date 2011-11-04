#include "EmailView.h"

EmailView::EmailView() {
    
    ofAddListener( ofEvents.keyPressed, this, &EmailView::keyPressed );
    verdana.loadFont( "verdana.ttf", 24, true );
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
    ofSetColor( 255, 255, 255, 100 );
    ofPushMatrix();
    ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
    int theWidth = ofGetWidth() * .75;
    int theHeight = ofGetHeight() * .25;
    ofRect( -theWidth/2, -theHeight/2, theWidth, theHeight );
    
    ofRectangle stringRect = verdana.getStringBoundingBox( utf8_helper.getString(), 0, 0 );
    float stringX = -stringRect.width / 2.0;
    float stringY = 0;// - stringRect.height / 2.0;
    ofSetColor( 0 );
    verdana.drawString( utf8_helper.getString(), stringX, stringY );
    
    ofPopMatrix();
}