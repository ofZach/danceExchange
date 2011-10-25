#pragma once

#include "ofMain.h"
#include "of3dPrimitives.h"
#include "ofxTween.h"

enum GlobeTween {
    GLOBE_SCALE,
    GLOBE_LAT_LON
};

class Globe {
public:
    
    float                   radius;
    float                   xRotation;  // latitude
    float                   yRotation;  // longitude
    
    ofVec3f                 pos;
    ofTexture               earthTexture;
    
    float                   scale;
    ofxTween                scaleTween;
    ofxTween                latLonTween;
    ofxEasingQuad           quadEasing;
    
    ofEvent<int>            latLonTweenEnded;
    
    void init( float radius ) {
        
        this->radius        = radius;
        this->xRotation     = 0;
        this->yRotation     = 0;
        
        this->scale         = 0;
        
        ofImage earthTemp;
        earthTemp.setUseTexture( false );
        earthTemp.loadImage( "earthlights.jpg" );
        
        earthTexture.allocate( earthTemp.width, earthTemp.height, GL_RGB, false );
        earthTexture.loadData( earthTemp.getPixels(), earthTemp.width, earthTemp.height, GL_RGB );
        
        ofSetSphereResolution( 32 );
        
        ofAddListener( scaleTween.end_E, this, &Globe::tweenEnded );
        ofAddListener( latLonTween.end_E, this, &Globe::tweenEnded );
        
    }
    
    void setLatLon( ofVec2f latLon, float duration = 500, float delay = 0 ) {
//        xRotation = latLon.x;
//        yRotation = latLon.y;
        
        if ( abs( latLon.y - yRotation ) < 90 ) {
            yRotation -= 360;
        }
        
        latLonTween.setParameters( GLOBE_LAT_LON, quadEasing, ofxTween::easeInOut, xRotation, latLon.x, duration, delay );
        latLonTween.addValue( yRotation, latLon.y );
        latLonTween.start();
    }
    
    void update( int deltaMillis ) {
        
        if ( scaleTween.isRunning() ) {
            scale = scaleTween.update();
        }
        if ( latLonTween.isRunning() ) {
            xRotation = latLonTween.update();
            yRotation = latLonTween.getTarget(1);
        }
        
    }
    
    void tweenEnded( int & theId ) {
        
        if ( theId == GLOBE_LAT_LON )
            ofNotifyEvent( latLonTweenEnded, theId );
    }
    
    void tweenGlobeToScale( float desiredScale, float duration, float delay = 0 ) {
        
        scaleTween.setParameters( GLOBE_SCALE, quadEasing, ofxTween::easeInOut, scale, desiredScale, duration, delay );
//        scaleTween.start();
        
    }
    
    ofVec3f getWorldCoordForLatLon( ofVec2f latLon ) {
        
        ofVec3f pt( 0, 0, radius ); // point to transform
        
        ofMatrix4x4 globeMat;
        globeMat.rotate( yRotation, 0, 1, 0 );
        globeMat.rotate( xRotation, 1, 0, 0 );
        
        ofMatrix4x4 cityMat;
        cityMat.rotate( latLon.x, 1, 0, 0 );
        cityMat.rotate( 180 + (-latLon.y), 0, 1, 0 );
        
        return ofMatrix4x4::transform3x3( pt, cityMat * globeMat ) + pos;
    }
    
    void draw() {
        
        if ( scale == 0 )
            return;
        
        ofPushMatrix();
        
        ofMatrix4x4 mat;
        mat.scale( -scale, -scale, scale );
        mat.rotate( yRotation, 0, 1, 0 );
        mat.rotate( xRotation, 1, 0, 0 );
        mat.translate( pos );
        
        glMultMatrixf( mat.getPtr() );
        
        earthTexture.bind();
        ofxSphere( ofVec3f(0,0,0), radius );
        earthTexture.unbind();
        ofPopMatrix();
        
    }
    
    
    
};