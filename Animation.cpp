#include "Precompiled.h"
#include "RutheniumAPI.h"

#include "SceneNode.h"

ruAnimation::ruAnimation() {
    looped = false;
    beginFrame = 0;
    endFrame = 0;
    currentFrame = 0;
    animSpeed = 0.0f;
    interpolator = 0.0f;
    nextFrame = 0;
}

ruAnimation::ruAnimation( int theBeginFrame, int theEndFrame, float theAnimSpeed, bool theLooped ) {
    currentFrame = theBeginFrame;
    nextFrame = currentFrame + 1;
    beginFrame = theBeginFrame;
    endFrame = theEndFrame;
    animSpeed = theAnimSpeed;
    looped = theLooped;
    interpolator = 0.0f;
}

void ruAnimation::Update() {
    if ( interpolator >= 1.0f ) {
        currentFrame++;
        // get next frame number
        if( currentFrame > endFrame ) {
            currentFrame = beginFrame;
            nextFrame = currentFrame + 1;
        } else if( currentFrame == endFrame ) { //
            if ( !looped ) {
                // find all nodes, which use this animation and disable it's animationEnabled flag
                for( auto node : SceneNode::msNodeList ) {
                    if( node->mCurrentAnimation == this ) {
                        node->mAnimationEnabled = false;
                    }
                }
            }
            nextFrame = beginFrame;
        } else {
            nextFrame = currentFrame + 1;
        }
        interpolator = 0.0f;
    }
    interpolator += animSpeed;
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