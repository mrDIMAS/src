#include "Precompiled.h"
#include "RutheniumAPI.h"

#include "SceneNode.h"

ruAnimation::ruAnimation() {
    looped = false;
    beginFrame = 0;
    endFrame = 0;
    currentFrame = 0;
    duration = 0.0f;
    interpolator = 0.0f;
    nextFrame = 0;
	enabled = false;
}

ruAnimation::ruAnimation( int theBeginFrame, int theEndFrame, float theDuration, bool theLooped ) {
    currentFrame = theBeginFrame;
    nextFrame = currentFrame + 1;
    beginFrame = theBeginFrame;
    endFrame = theEndFrame;
    duration = theDuration;
    looped = theLooped;
	enabled = false;
    interpolator = 0.0f;
}

void ruAnimation::AddFrameListener( int frameNum, const ruDelegate & action ) {
	if( frameNum >= 0 || frameNum < endFrame ) {
		mFrameListenerList[ frameNum ] = AnimationEvent();
		mFrameListenerList[ frameNum ].Event.AddListener( action );
	}
}

void ruAnimation::Rewind() {
	currentFrame = beginFrame;
	nextFrame = beginFrame + 1;
	interpolator = 0.0f;
	for( auto & frameActionPair : mFrameListenerList ) {
		frameActionPair.second.mState = false;
	}
}

void ruAnimation::Update( float dt ) {
	if( enabled ) {
		if ( interpolator >= 1.0f ) {
			currentFrame++;
			// get next frame number
			if( currentFrame > endFrame ) {
				currentFrame = beginFrame;
				nextFrame = currentFrame + 1;
			} else if( currentFrame == endFrame ) { //
				if ( !looped ) {
					enabled = false;
				}
				nextFrame = beginFrame;

				for( auto & frameActionPair : mFrameListenerList ) {
					frameActionPair.second.mState = false;
				}
			} else {
				nextFrame = currentFrame + 1;
			}
			interpolator = 0.0f;
		}

		interpolator += dt * ( ( endFrame - beginFrame ) / duration );

		for( auto & frameActionPair : mFrameListenerList ) {
			if( !frameActionPair.second.mState ) {
				if( currentFrame == frameActionPair.first ) {
					frameActionPair.second.Event.DoActions();
					frameActionPair.second.mState = true;
				}
			}
		}
	}
}

void ruAnimation::SetCurrentFrame( int frame ) {
    currentFrame = frame;
    // range check
    if( currentFrame >= ( endFrame - 1 ) ) {
        currentFrame  = ( endFrame - 1 );
    }
    if( currentFrame < beginFrame ) {
        currentFrame = beginFrame;
    }
}

void ruAnimation::SetFrameInterval( int begin, int end ) {
    beginFrame = begin;
    endFrame = end;
    // swap if needed
    if( beginFrame > endFrame ) {
        int temp = beginFrame;
        beginFrame = endFrame;
        endFrame = temp;
    }
    // range check
    if( beginFrame < 0 ) {
        beginFrame = 0;
    }
    if( endFrame < 0 ) {
        endFrame = 0;
    }
}