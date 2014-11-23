#include "Engine.h"
#include "Common.h"
#include "SceneNode.h"

Animation::Animation() {
	looped = false;
	beginFrame = 0;
	endFrame = 0;
	currentFrame = 0;
	timeSeconds = 0.0f;
	interpolator = 0.0f;
	nextFrame = 0;
}

Animation::Animation( int theBeginFrame, int theEndFrame, float theTimeSeconds, bool theLooped ) {
	currentFrame = theBeginFrame;
	nextFrame = currentFrame + 1;
	beginFrame = theBeginFrame;
	endFrame = theEndFrame;
	timeSeconds = theTimeSeconds;
	looped = theLooped;
	interpolator = 0.0f;
}

void Animation::Update( ) {
	if ( interpolator >= 1.0f ) {
		currentFrame++;
		// get next frame number
		if( currentFrame > endFrame ) {
			currentFrame = beginFrame;		
			nextFrame = currentFrame + 1;
		} else if( currentFrame == endFrame ) { // 
			if ( !looped ) {
				// find all nodes, which use this animation and disable it's animationEnabled flag
				for( auto node : g_nodes ) {
					if( node->currentAnimation == this ) {
						node->animationEnabled = false;
					}
				}
			}
			nextFrame = beginFrame;
		} else {
			nextFrame = currentFrame + 1;
		}
		interpolator = 0.0f;
	}
	interpolator += g_dt / timeSeconds;
}

void Animation::SetCurrentFrame( int frame ) {
	currentFrame = frame;
	// range check
	if( currentFrame >= ( endFrame - 1 ) ) {
		currentFrame  = ( endFrame - 1 );
	}
	if( currentFrame < beginFrame ) {
		currentFrame = beginFrame;
	}
}

void Animation::SetFrameInterval( int begin, int end ) {
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