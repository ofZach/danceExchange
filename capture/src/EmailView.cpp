#include "EmailView.h"

EmailView::EmailView() {
    
    ofAddListener( ofEvents.keyPressed, this, &EmailView::keyPressed );
    verdana.loadFont( "verdana.ttf", 24, true );
    emailAddress = "";
    
}

EmailView::~EmailView() {
    
    ofRemoveListener( ofEvents.keyPressed, this, &EmailView::keyPressed );
    
}

void EmailView::keyPressed( ofKeyEventArgs & args ) {
    
    cout << "EmailView::keyPressed: " << (char)args.key << " with code: " << args.key << endl;
    if ( args.key == OF_KEY_BACKSPACE || args.key == OF_KEY_DEL ) {
        int stringLength = strlen( emailAddress.c_str() );
        if ( stringLength > 0 )
            emailAddress = emailAddress.substr( 0, strlen( emailAddress.c_str() ) - 1 );
    }
    else if ( args.key == OF_KEY_RETURN ) {
        ofNotifyEvent( emailAddressEnteredEvent, emailAddress );
    }
    else {
        emailAddress += (char)args.key;
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
    
    ofRectangle stringRect = verdana.getStringBoundingBox( emailAddress, 0, 0 );
    float stringX = -stringRect.width / 2.0;
    float stringY = 0;// - stringRect.height / 2.0;
    ofSetColor( 0 );
    verdana.drawString( emailAddress, stringX, stringY );
    
    ofPopMatrix();
}