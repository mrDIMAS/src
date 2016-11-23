/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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

Animation::Animation( ) :
	mLooped( false ),
	mBeginFrame( 0 ),
	mEndFrame( 0 ),
	mCurrentFrame( 0 ),
	mDuration( 0.0f ),
	mInterpolator( 0.0f ),
	mNextFrame( 0 ),
	mEnabled( false ),
	mDirection( Animation::Direction::Forward ) {

}

Animation::Animation( int theBeginFrame, int theEndFrame, float theDuration, bool theLooped ) :
	mCurrentFrame( theBeginFrame ),
	mNextFrame( theBeginFrame + 1 ),
	mBeginFrame( theBeginFrame ),
	mEndFrame( theEndFrame ),
	mDuration( theDuration ),
	mLooped( theLooped ),
	mEnabled( false ),
	mInterpolator( 0.0f ),
	mDirection( Animation::Direction::Forward ) {

}

Animation::~Animation( ) {

}

void Animation::AddFrameListener( int frameNum, const Delegate & action ) {
	mFrameListenerList[ frameNum ] = AnimationEvent( );
	mFrameListenerList[ frameNum ].Event += action;
}

void Animation::RemoveFrameListeners( int frameNum ) {
	mFrameListenerList[ frameNum ].Event.Clear( );
}

void Animation::Rewind( ) {
	if ( mDirection == Animation::Direction::Forward ) {
		mCurrentFrame = mBeginFrame;
		mNextFrame = mCurrentFrame + 1;
	} else if ( mDirection == Animation::Direction::Reverse ) {
		mCurrentFrame = mEndFrame;
		mNextFrame = mCurrentFrame - 1;
	}
	mInterpolator = 0.0f;
	for ( auto & frameActionPair : mFrameListenerList ) {
		frameActionPair.second.mDone = false;
	}
}

void Animation::Update( float dt ) {
	if ( mEnabled ) {
		if ( mInterpolator >= 1.0f ) {
			// forward play
			if ( mDirection == Animation::Direction::Forward ) {
				++mCurrentFrame;
				if ( mCurrentFrame > mEndFrame ) {
					mCurrentFrame = mBeginFrame;
					mNextFrame = mCurrentFrame + 1;
				} else if ( mCurrentFrame == mEndFrame ) {
					mEnabled = mLooped;
					mNextFrame = mBeginFrame;
					DoFramesActions( );
					for ( auto & frameActionPair : mFrameListenerList ) {
						frameActionPair.second.mDone = false;
					}
				} else {
					mNextFrame = mCurrentFrame + 1;
				}
				// reverse play
			} else if ( mDirection == Animation::Direction::Reverse ) {
				--mCurrentFrame;
				if ( mCurrentFrame < mBeginFrame ) {
					mCurrentFrame = mEndFrame;
					mNextFrame = mCurrentFrame - 1;
				} else if ( mCurrentFrame == mBeginFrame ) {
					mEnabled = mLooped;
					mNextFrame = mEndFrame;
					DoFramesActions( );
					for ( auto & frameActionPair : mFrameListenerList ) {
						frameActionPair.second.mDone = false;
					}
				} else {
					mNextFrame = mCurrentFrame - 1;
				}
			}
			mInterpolator = 0.0f;
		}
		if ( mEnabled ) {
			mInterpolator += dt * ( ( mEndFrame - mBeginFrame ) / mDuration );
			DoFramesActions( );
		}
	}
}

void Animation::DoFramesActions( ) {
	for ( auto & frameActionPair : mFrameListenerList ) {
		if ( !frameActionPair.second.mDone ) {
			if ( mCurrentFrame == frameActionPair.first ) {
				frameActionPair.second.Event( );
				frameActionPair.second.mDone = true;
			}
		}
	}
}

void Animation::SetCurrentFrame( int frame ) {
	mCurrentFrame = frame;
	// range check
	if ( mCurrentFrame >= ( mEndFrame - 1 ) ) {
		mCurrentFrame = ( mEndFrame - 1 );
	}
	if ( mCurrentFrame < mBeginFrame ) {
		mCurrentFrame = mBeginFrame;
	}
}

void Animation::SetFrameInterval( int begin, int end ) {
	mBeginFrame = begin;
	mEndFrame = end;
	// swap if needed
	if ( mBeginFrame > mEndFrame ) {
		std::swap( mBeginFrame, mEndFrame );
	}
	// range check
	if ( mBeginFrame < 0 ) {
		mBeginFrame = 0;
	}
	if ( mEndFrame < 0 ) {
		mEndFrame = 0;
	}
}

int Animation::GetCurrentFrame( ) const {
	return mCurrentFrame;
}

int Animation::GetEndFrame( ) const {
	return mEndFrame;
}

int Animation::GetBeginFrame( ) const {
	return mBeginFrame;
}

int Animation::GetNextFrame( ) const {
	return mNextFrame;
}

void Animation::SetName( const string & newName ) {
	mName = newName;
}

float Animation::GetInterpolator( ) const {
	return mInterpolator;
}

string Animation::GetName( ) const {
	return mName;
}

void Animation::SetDuration( float duration ) {
	mDuration = duration;
}

float Animation::GetDuration( ) const {
	return mDuration;
}

void Animation::SetEnabled( bool state ) {
	mEnabled = state;
}

bool Animation::IsEnabled( ) const {
	return mEnabled;
}

Animation::Direction Animation::GetDirection( ) const {
	return mDirection;
}

void Animation::SetDirection( const Animation::Direction & direction ) {
	mDirection = direction;
}

void Animation::SetInterpolator( float interpolator ) {
	mInterpolator = interpolator;
	if ( mInterpolator < 0 ) {
		mInterpolator = 0;
	}
	if ( mInterpolator > 1.0f ) {
		mInterpolator = 1.0f;
	}
}