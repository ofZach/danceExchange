#pragma once

#include "ofMain.h"

class FrustumHelp {
public:
    enum { TOP, BOT, LEF, RIG, NEA, FARP };
    enum { NTL, NTR, NBL, NBR };
    enum { FTL, FTR, FBL, FBR };
    
    ofVec3f frustumPlaneNormals[6];
    ofVec3f frustumPlanePoints[6];
    float frustumPlaneDists[6];
    
    ofVec3f nearClip[4];
    ofVec3f farClip[4];
    
    float nearClipVal;
    float farClipVal;
    float fov;
    float aspectRatio;
    
    void calcFrustumPlane( ofVec3f &fNormal, ofVec3f &fPoint, float &fDist, const ofVec3f &v1, const ofVec3f &v2, const ofVec3f &v3 )
    {
        ofVec3f aux1, aux2;
        
        aux1 = v1 - v2;
        aux2 = v3 - v2;
        
        fNormal = aux2.cross( aux1 );
        fNormal.normalize();
        fPoint.set( v2 );
        fDist = -( fNormal.dot( fPoint ) );
    }
    
    void getNearClipCoordinates( ofEasyCam &cam, ofVec3f *topLeft, ofVec3f *topRight, ofVec3f *bottomLeft, ofVec3f *bottomRight ) {
        
        ofVec3f viewDirection = cam.getLookAtDir();
        viewDirection.normalize();
        
        double left, right, bottom, top, nearVal, farVal;
        cam.getProjectionMatrix().getFrustum( left, right, bottom, top, nearVal, farVal );
        
//        float nearClip = 1.0; // hardcoded for now... can't access it from ofCamera
        
        *topLeft        = cam.getPosition() + ( nearClipVal * viewDirection ) + ( top * cam.getUpDir() ) + ( left * cam.getSideDir() );
        *topRight       = cam.getPosition() + ( nearClipVal * viewDirection ) + ( top * cam.getUpDir() ) + ( right * cam.getSideDir() );
        *bottomLeft     = cam.getPosition() + ( nearClipVal * viewDirection ) + ( bottom * cam.getUpDir() ) + ( left * cam.getSideDir() );
        *bottomRight    = cam.getPosition() + ( nearClipVal * viewDirection ) + ( bottom * cam.getUpDir() ) + ( right * cam.getSideDir() );
        
    }
    
    void getFarClipCoordinates( ofEasyCam &cam, ofVec3f *topLeft, ofVec3f *topRight, ofVec3f *bottomLeft, ofVec3f *bottomRight ) {
        
        ofVec3f viewDirection = cam.getLookAtDir();
        viewDirection.normalize();
        
//        float farClip = 10000.0;
//        float fov = 60.0f;
//        float aspectRatio = 1.33;
        
        float frustumTop	=  farClipVal * tanf( (float)M_PI / 180.0f * fov * 0.5f );
        float frustumBottom	= -frustumTop;
        float frustumRight	=  frustumTop * aspectRatio;
        float frustumLeft	= -frustumRight;
        
        
        *topLeft        = cam.getPosition() + ( farClipVal * viewDirection ) + ( frustumTop * cam.getUpDir() ) + ( frustumLeft * cam.getSideDir() );
        *topRight       = cam.getPosition() + ( farClipVal * viewDirection ) + ( frustumTop * cam.getUpDir() ) + ( frustumRight * cam.getSideDir() );
        *bottomLeft     = cam.getPosition() + ( farClipVal * viewDirection ) + ( frustumBottom * cam.getUpDir() ) + ( frustumLeft * cam.getSideDir() );
        *bottomRight    = cam.getPosition() + ( farClipVal * viewDirection ) + ( frustumBottom * cam.getUpDir() ) + ( frustumRight * cam.getSideDir() );
        
    }
    
    void calcNearAndFarClipCoordinates( ofEasyCam &cam )
    {
        getNearClipCoordinates( cam, &nearClip[NTL], &nearClip[NTR], &nearClip[NBL], &nearClip[NBR] );
        
        getFarClipCoordinates( cam, &farClip[FTL], &farClip[FTR], &farClip[FBL], &farClip[FBR] );
        
        calcFrustumPlane( frustumPlaneNormals[TOP], frustumPlanePoints[TOP], frustumPlaneDists[TOP], nearClip[NTR], nearClip[NTL], farClip[FTL] );
        calcFrustumPlane( frustumPlaneNormals[BOT], frustumPlanePoints[BOT], frustumPlaneDists[BOT], nearClip[NBL], nearClip[NBR], farClip[FBR] );
        calcFrustumPlane( frustumPlaneNormals[LEF], frustumPlanePoints[LEF], frustumPlaneDists[LEF], nearClip[NTL], nearClip[NBL], farClip[FBL] );
        calcFrustumPlane( frustumPlaneNormals[RIG], frustumPlanePoints[RIG], frustumPlaneDists[RIG], farClip[FTR], farClip[FBR], nearClip[NBR] );
        calcFrustumPlane( frustumPlaneNormals[NEA], frustumPlanePoints[NEA], frustumPlaneDists[NEA], nearClip[NTL], nearClip[NTR], nearClip[NBR] );
        calcFrustumPlane( frustumPlaneNormals[FARP], frustumPlanePoints[FARP], frustumPlaneDists[FARP], farClip[FTR], farClip[FTL], farClip[FBL] );
    }
    
    ofVec3f getRandomPointOnFarPlane() {
        float x = ofLerp( farClip[FTL].x, farClip[FTR].x, ofRandom( 1.0 ) );
        float y = ofLerp( farClip[FTL].y, farClip[FBL].y, ofRandom( 1.0 ) );
        float z = farClip[FTL].z;
        
        return ofVec3f(x,y,z);
    }
    
    bool isPointInFrustum( const ofVec3f &loc )
    {
        float d;
        bool result = true;
        
        for( int i=0; i<6; i++ ){
            d = frustumPlaneDists[i] + frustumPlaneNormals[i].dot( loc );
            if( d < 0 )
                return( false );
        }
        
        return( result );
    }
};