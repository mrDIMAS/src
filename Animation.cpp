/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"
#include "RutheniumAPI.h"

#include "SceneNode.h"

ruAnimation::ruAnimation() {
    mLooped = false;
    mBeginFrame = 0;
    mEndFrame = 0;
    mCurrentFrame = 0;
    mDuration = 0.0f;
    mInterpolator = 0.0f;
    mNextFrame = 0;
	mEnabled = false;
}

ruAnimation::ruAnimation( int theBeginFrame, int theEndFrame, float theDuration, bool theLooped ) {
    mCurrentFrame = theBeginFrame;
    mNextFrame = mCurrentFrame + 1;
    mBeginFrame = theBeginFrame;
    mEndFrame = theEndFrame;
    mDuration = theDuration;
    mLooped = theLooped;
	mEnabled = false;
    mInterpolator = 0.0f;
}

ruAnimation::~ruAnimation() {

}

void ruAnimation::AddFrameListener( int frameNum, const ruDelegate & action ) {
	if( frameNum >= 0 || frameNum < mEndFrame ) {
		mFrameListenerList[ frameNum ] = AnimationEvent();
		mFrameListenerList[ frameNum ].Event.AddListener( action );
	}
}

void ruAnimation::Rewind() {
	mCurrentFrame = mBeginFrame;
	mNextFrame = mBeginFrame + 1;
	mInterpolator = 0.0f;
	for( auto & frameActionPair : mFrameListenerList ) {
		frameActionPair.second.mState = false;
	}
}

void ruAnimation::Update( float dt ) {
	if( mEnabled ) {
		if ( mInterpolator >= 1.0f ) {
			mCurrentFrame++;
			// get next frame number
			if( mCurrentFrame > mEndFrame ) {
				mCurrentFrame = mBeginFrame;
				mNextFrame = mCurrentFrame + 1;
			} else if( mCurrentFrame == mEndFrame ) { 
				if ( !mLooped ) {
					mEnabled = false;
				}
				mNextFrame = mBeginFrame;
				for( auto & frameActionPair : mFrameListenerList ) {
					frameActionPair.second.mState = false;
				}
			} else {
				mNextFrame = mCurrentFrame + 1;
			}	
			mInterpolator = 0.0f;
		}

		if( mEnabled ) {
			mInterpolator += dt * ( ( mEndFrame - mBeginFrame ) / mDuration );
		}

		for( auto & frameActionPair : mFrameListenerList ) {
			if( !frameActionPair.second.mState ) {
				if( mCurrentFrame == frameActionPair.first ) {
					frameActionPair.second.Event.DoActions();
					frameActionPair.second.mState = true;
				}
			}
		}
	}
}

void ruAnimation::SetCurrentFrame( int frame ) {
    mCurrentFrame = frame;
    // range check
    if( mCurrentFrame >= ( mEndFrame - 1 ) ) {
        mCurrentFrame  = ( mEndFrame - 1 );
    }
    if( mCurrentFrame < mBeginFrame ) {
        mCurrentFrame = mBeginFrame;
    }
}

void ruAnimation::SetFrameInterval( int begin, int end ) {
    mBeginFrame = begin;
    mEndFrame = end;
    // swap if needed
    if( mBeginFrame > mEndFrame ) {
        int temp = mBeginFrame;
        mBeginFrame = mEndFrame;
        mEndFrame = temp;
    }
    // range check
    if( mBeginFrame < 0 ) {
        mBeginFrame = 0;
    }
    if( mEndFrame < 0 ) {
        mEndFrame = 0;
    }
}

int ruAnimation::GetCurrentFrame() const {
	return mCurrentFrame;
}
int ruAnimation::GetEndFrame() const {
	return mEndFrame;
}
int ruAnimation::GetBeginFrame() const {
	return mBeginFrame;
}
int ruAnimation::GetNextFrame() const {
	return mNextFrame;
}
void ruAnimation::SetName( const string & newName ) {
	mName = newName;
}
float ruAnimation::GetInterpolator() const {
	return mInterpolator;
}
string ruAnimation::GetName( ) const {
	return mName;
}
void ruAnimation::SetDuration( float duration ) {
	mDuration = duration;
}
float ruAnimation::GetDuration( ) const {
	return mDuration;
}
void ruAnimation::SetEnabled( bool state ) {
	mEnabled = state;
}
bool ruAnimation::IsEnabled() const {
	return mEnabled;
}