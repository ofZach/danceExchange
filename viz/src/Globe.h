#pragma once

#include "ofMain.h"
#include "of3dPrimitives.h"

class Globe {
public:
    
    float                   radius;
    float                   xRotation;  // latitude
    float                   yRotation;  // longitude
    
    ofVec3f                 pos;
    ofTexture               earthTexture;
    
    void init( float radius ) {
        
        this->radius        = radius;
        this->xRotation     = 0;
        this->yRotation     = 0;
        
        ofImage earthTemp;
        earthTemp.setUseTexture( false );
        earthTemp.loadImage( "earthlights.jpg" );
        
        earthTexture.allocate( earthTemp.width, earthTemp.height, GL_RGB, false );
        earthTexture.loadData( earthTemp.getPixels(), earthTemp.width, earthTemp.height, GL_RGB );
    }
    
    void setLatLon( ofVec2f latLon ) {
        xRotation = latLon.x;
        yRotation = latLon.y;
    }
    
    void update( int deltaMillis ) {
        
    }
    
    ofVec3f getWorldCoordForLatLon( ofVec2f latLon ) {
        
//        ofVec3f pt( 0, 0, radius ); // point to transform
        ofVec3f pt( 0, 0, radius ); // point to transform
        cout << latLon << endl;
        
        ofMatrix4x4 mat;
        mat.scale( -1.0, -1.0, 1.0 );
        mat.rotate( latLon.x, 1, 0, 0 );
        mat.rotate( latLon.y, 0, 1, 0 );
        
        return ofMatrix4x4::transform3x3( pt, mat );
    }
    
    void draw() {
        
        ofPushMatrix();
        
        ofMatrix4x4 mat;
        mat.scale( -1.0, -1.0, 1.0 );
        mat.rotate( yRotation, 0, 1, 0 );
        mat.rotate( xRotation, 1, 0, 0 );
        
//        glRotatef( xRotation, 1, 0, 0 );
//        glRotatef( yRotation, 0, 1, 0 );
//        glScalef( -1.0, -1.0, 1.0 );
        
        glMultMatrixf( mat.getPtr() );
        
        earthTexture.bind();
        ofSphere( pos, radius );
        earthTexture.unbind();
        ofPopMatrix();
        
    }
    
    
    
};