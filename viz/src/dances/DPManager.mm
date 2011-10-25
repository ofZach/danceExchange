#include "DPManager.h"

void DPManager::init( Pointilist *pointilist ) {
    
    this->pointilist = pointilist;
    globalScale = 1.0;
    paused = false;
    
    ofAddListener( globalScaleTween.end_E, this, &DPManager::tweenEnded );
    ofAddListener( modeChangeTween.end_E, this, &DPManager::modeChanged );
    
    mode = STARFIELD_MODE;
}

void DPManager::modeChanged( int & theId ) {
    mode = nextMode;
    
    if ( mode == GLOBE_MODE ) cout << "dpmanager mode changed to globe mode" << endl;
    if ( mode == STARFIELD_MODE ) cout << "dpmanager mode changed to starfield mode" << endl;
}

void DPManager::update( int deltaMillis ) { 
    
    if ( globalScaleTween.isRunning() ) {
        globalScale = globalScaleTween.update();    
        
        cout << "globalScale: " << globalScale << endl;
    }
    
    modeChangeTween.update();
    
    switch( mode ) {
            
        case GLOBE_MODE:
            updateGlobe( deltaMillis );
            break;
            
        case STARFIELD_MODE:
            updateStarfield( deltaMillis );
            break;
            
        default:
            
            break;
    }
}

void DPManager::updateGlobe( int deltaMillis ) {
    
    for ( int i=0; i<cityParticles.size(); i++ ) {
        DanceParticle *dp = cityParticles[i];
        // for now, call the particles globe update because we know this particle is doing something
        // in relation to the globe, but this should really get restructured somehow
        dp->updateGlobe( deltaMillis, false );
        pointilist->addPoint( dp->pos.x, dp->pos.y, dp->pos.z,
                             50.0 * dp->posTween.getTarget( 0 ),
                             dp->r, dp->g, dp->b, dp->alpha,
                             dp->DV->texIndex, 0, dp->DV->firstFrame + dp->DV->currentFrame
                             );
    }
}

void DPManager::updateStarfield( int deltaMillis ) {
    
    
    for ( int i=0; i<dpVector.size(); i++ ) {
        DanceParticle *dp = dpVector[i];
        dp->updateStarfield( deltaMillis, paused );
        
        if ( !frustumHelp.isPointInFrustum( dp->pos ) ) {
            
            dp->pos.set( frustumHelp.getRandomPointOnFarPlane() );
            dp->alpha = 0;
        }
        pointilist->addPoint( dp->pos.x, dp->pos.y, dp->pos.z, // 3D position
                             200.0 * globalScale, // size
                             dp->r, dp->g, dp->b, dp->alpha, // rgba
                             dp->DV->texIndex, 0, dp->DV->firstFrame + dp->DV->currentFrame // texture unit, rotation (not used in this), cell number
                             );
    }
}

void DPManager::draw() {
    // maybe the test app should call this?
    pointilist->draw();
}

void DPManager::animateParticlesForCity( string cityName, ofVec3f worldPos ) {
    
    // abruptly clear any that are there for now
    cityParticles.clear();
    
    // go through and get all the particles corresponding to this city
    for ( vector<DanceParticle*>::iterator it = dpVector.begin(); it != dpVector.end(); it++ ) {
        DanceParticle* dp = *it;
        if ( dp->DV->info.city == cityName )
            cityParticles.push_back( dp );
    }
    
    // now go through the city particles and assign them positions
    for ( int i=0; i<cityParticles.size(); i++ ) {
        DanceParticle* dp = cityParticles[i];
        
        float randomAngle = ofRandom( 0, TWO_PI );
        float xyScale = 200.0;
        ofVec3f position( cosf( randomAngle ) * xyScale, sinf( randomAngle ) * xyScale, worldPos.z );
        dp->pos.set( worldPos );
        dp->startPos.set( worldPos );
        dp->targetPos.set( position );
        dp->vel.set( 0, 0, 0 );
        dp->startPosTween( i * 70 );
        dp->alpha = 1.0;
    }
    
    cout << "there are " << cityParticles.size() << " dances for " << cityName << endl;
    
}

void DPManager::transitionToGlobeMode( int duration, int delay ) {
    tweenParticlesToScale( 0, duration, delay );
    nextMode = GLOBE_MODE;
    modeChangeTween.setParameters( linearEasing, ofxTween::easeInOut, 0, 1, duration, delay );
}

void DPManager::transitionToStarfieldMode( int duration, int delay ) {
    tweenParticlesToScale( 1, duration, delay );
    // clear city particles
    cityParticles.clear();
    for ( vector<DanceParticle*>::iterator it = dpVector.begin(); it != dpVector.end(); it++ ) {
        DanceParticle *dp = *it;
        dp->vel.set( 0, 0, -10 );
    }
    nextMode = STARFIELD_MODE;
    modeChangeTween.setParameters( linearEasing, ofxTween::easeInOut, 0, 1, duration, 0 );
}

void DPManager::tweenParticlesToScale( float desiredScale, float duration, float delay ) {
    
    globalScaleTween.setParameters( DPMANAGER_GLOBAL_SCALE, quadEasing, ofxTween::easeInOut, globalScale, desiredScale, duration, delay );
    
}

void DPManager::createParticle( danceVideo * dv ) {
    
    DanceParticle *dp = new DanceParticle( dv );
    dpVector.push_back( dp );
    dpMap[dp->DV->hash] = dp;
    
    dp->pos.set( frustumHelp.getRandomPointOnFarPlane() );
    dp->vel.set( 0, 0, -ofRandom( 10.0, 20.0 ) );
    
}

void DPManager::tweenEnded( int & theId ) {
    
}