#include "WarningView.h"

WarningView::WarningView( ofTrueTypeFont & font, string warningMessage ) {
    this->font = font;
    this->message = warningMessage;
}

WarningView::~WarningView() {

}

void WarningView::init() {
    
}

void WarningView::update() {
    
}

void WarningView::draw() {
    
    float aspectWidth = ((float)ofGetHeight()) * ( 4.0 / 3.0 );
    float xOffset = ( ofGetWidth() - aspectWidth ) / 2.0;
    float targetWidth = aspectWidth * .75;
    
    ofRectangle rect = font.getStringBoundingBox( message, 0, 0 );
    float scaleFactor = targetWidth / (float)rect.width;
    rect.x *= scaleFactor;  rect.y *= scaleFactor;
    rect.width *= scaleFactor;  rect.height *= scaleFactor;
    
    ofSetColor( 0, 0, 0 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x, -rect.y + ofGetHeight() * .05 );
    ofScale( scaleFactor, scaleFactor );
    font.drawString( message, 0, 0 );
    ofPopMatrix();
    
    ofSetColor( 255, 255, 255 );
    ofPushMatrix();
    ofTranslate( xOffset + (aspectWidth-targetWidth)*.5 - rect.x - 5, -rect.y - 5 + ofGetHeight() * .05 );
    ofScale( scaleFactor, scaleFactor );
    font.drawString( message, 0, 0 );
    ofPopMatrix();
}