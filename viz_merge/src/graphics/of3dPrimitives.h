#pragma once

#include "ofGraphics.h"
#include "ofVectorMath.h"


bool ofGetPrimitiveGenerateTexCoords();
void ofEnablePrimitiveGenerateTexCoords();
void ofDisablePrimitiveGenerateTexCoords();

// The default resolutions for the primitives are set in their helper classes at the bottom

//----------------------------------------------------------
// Sphere

void ofxSphere(float x, float y, float z, float radius);
void ofxSphere(float x, float y, float radius);
void ofxSphere(const ofPoint& position, float radius);
void ofxSphere(float radius);

void ofSetSphereResolution( int _numRings, int _numSegments );
void ofGenerateSphereMesh( ofMesh& _mesh, float _radius, int _numRings = 16, int _numSegments = 16 );

// Small helper classes to keep track of with what settings we've generated meshes with

//----------------------------------------------------------
class ofSphereParameters{
public:	
	
	ofSphereParameters() { 
		radius = -1.0f; numRings = 32; numSegments = 32; // set defaults, apart from radius
	}
	
	bool equals( float _radius, int _numRings, int _numSegments ){
		if( radius == _radius && numRings == _numRings && numSegments == _numSegments ){
			return true;
		} else {
			return false;			
		}
	}
	
	float radius;
	int numRings;
	int numSegments;
};
