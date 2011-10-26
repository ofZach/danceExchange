#include "DPManager.h"

const float GLOBE_PARTICLE_SIZE = 150.0f;

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
                             GLOBE_PARTICLE_SIZE * dp->posTween.getTarget( 0 ),
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


int DPManager::getNumDancesInCity( string cityName ) {
    int num = 0;
    for ( vector<DanceParticle*>::iterator it = dpVector.begin(); it != dpVector.end(); it++ ) {
        DanceParticle* dp = *it;
        if ( dp->DV->city == cityName )
            num++;
    }
    return num;
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
    
    // for now, putting particles in columns and rows
    // in screen space
    int numRows = 4;
    float gridCellWidth = GLOBE_PARTICLE_SIZE * 1.333;
    float gridCellHeight = GLOBE_PARTICLE_SIZE;
    float startX = -ofGetWidth()/2 + gridCellWidth/2.0;
    float startY = -ofGetHeight()/2 + gridCellHeight/2.0 + 100;
    
    // now go through the city particles and assign them positions
    for ( int i=0; i<cityParticles.size(); i++ ) {
        DanceParticle* dp = cityParticles[i];
        
        int colNum = i / numRows;
        int rowNum = i % numRows;
        
        float randomAngle = ofRandom( 0, TWO_PI );
        float xyScale = 50.0;
//        ofVec3f position( cosf( randomAngle ) * xyScale, sinf( randomAngle ) * xyScale, 0 );
//        position += worldPos;
        ofVec3f position( colNum * gridCellWidth + startX, rowNum * gridCellHeight + startY, 0 );
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

void DPManager::zoomParticle( DanceParticle * dp ) {
    // first put the particle against the rear of the frustum
    dp->pos.set( frustumHelp.getRandomPointOnFarPlane() );
    // set its start pos and target pos
    dp->startPos.set( dp->pos );
    dp->targetPos.set( 0, 0, -300 );
    // start the tween
    dp->startZoomTween();
    ofAddListener( dp->zoomFinished, this, &DPManager::particleZoomed );
}

void DPManager::particleZoomed( DanceParticle & dp ) {
    cout << "the particle zoomed" << endl;
    // set start pos and target pos
    dp.startPos.set( dp.pos );
    dp.targetPos.set( frustumHelp.getRandomPointOnFarPlane() );
    dp.targetPos.y = 0;
    dp.targetPos.x = 0;
    // start the tween
    dp.startZoomOutTween( 700 );
    ofAddListener( dp.zoomOutFinished, this, &DPManager::particleZoomedOut );
}

void DPManager::particleZoomedOut( DanceParticle & dp ) {
    cout << "the particle zoomed out" << endl;
    dp.pos.set( frustumHelp.getRandomPointOnFarPlane() );
    dp.alpha = 0.0;
}

void DPManager::createParticle( danceVideo * dv, bool shouldZoom ) {
    
    DanceParticle *dp = new DanceParticle( dv );
    dpVector.push_back( dp );
    dpMap[dp->DV->hash] = dp;
    
    dp->pos.set( frustumHelp.getRandomPointOnFarPlane() );
    dp->vel.set( 0, 0, -ofRandom( 10.0, 20.0 ) );
    
    if ( shouldZoom ) {
        zoomParticle( dp );
    }
    
}

void DPManager::tweenEnded( int & theId ) {
    
}