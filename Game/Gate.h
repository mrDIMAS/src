#pragma once

#include "Game.h"
#include "Player.h"

class Gate {
public:
	enum class State {
		Opening,
		Closing,
		Opened,
		Closed,
	};

	ruNodeHandle mGate;
	float mGateHeight;
	ruNodeHandle mButtonOpen[2];
	ruNodeHandle mButtonClose[2];
	ruVector3 mInitialPosition;
	State mState;
	float mGateYOffset;
	ruSoundHandle mBeginSound;
	ruSoundHandle mIdleSound;
	ruSoundHandle mEndSound;
	ruSoundHandle mButtonSound;
	
	explicit Gate( ruNodeHandle gate, ruNodeHandle buttonOpen, ruNodeHandle buttonClose, ruNodeHandle buttonOpen2, ruNodeHandle buttonClose2  ) {
		mGate = gate;
		mGateHeight = (ruGetNodeAABBMax( mGate ) - ruGetNodeAABBMin( mGate )).y * 0.9f;
		mButtonClose[0] = buttonClose;
		mButtonOpen[0] = buttonOpen;
		mButtonClose[1] = buttonClose2;
		mButtonOpen[1] = buttonOpen2;
		mGateYOffset = 0.0f;
		mInitialPosition = ruGetNodePosition( mGate );
		mState = State::Closed;

		mBeginSound = ruLoadSound3D( "data/sounds/door_open_start.ogg" );
		mIdleSound = ruLoadSound3D( "data/sounds/door_open_idle.ogg" );
		mEndSound = ruLoadSound3D( "data/sounds/door_open_end.ogg" );
		mButtonSound = ruLoadSound3D( "data/sounds/button.ogg" );

		ruSetSoundPosition( mBeginSound, ruGetNodePosition( mGate ) );
		ruSetSoundPosition( mIdleSound, ruGetNodePosition( mGate ) );
		ruSetSoundPosition( mEndSound, ruGetNodePosition( mGate ) );
	}

	void OnEndMoving() {
		ruPauseSound( mIdleSound );
		ruPlaySound( mEndSound );
	}

	void Update() {
		if( pPlayer->mNearestPickedNode == mButtonOpen[0] || pPlayer->mNearestPickedNode == mButtonOpen[1] ) {
			if( !( mState == State::Closing || mState == State::Opening )) {
				pPlayer->SetActionText( "�������" );
				if( ruIsKeyHit( pPlayer->mKeyUse )) {
					ruSetSoundPosition( mButtonSound, ruGetNodePosition( pPlayer->mNearestPickedNode ));
					ruPlaySound( mButtonSound );
					if( mState != State::Opened ) {
						mState = State::Opening;
						ruPlaySound( mBeginSound );
					}
				}
			}
		}
		if( pPlayer->mNearestPickedNode == mButtonClose[0] || pPlayer->mNearestPickedNode == mButtonClose[1] ) {
			if( !( mState == State::Closing || mState == State::Opening )) {
				pPlayer->SetActionText( "�������" );
				if( ruIsKeyHit( pPlayer->mKeyUse )) {	
					ruSetSoundPosition( mButtonSound, ruGetNodePosition( pPlayer->mNearestPickedNode ));
					ruPlaySound( mButtonSound );
					if( mState != State::Closed ) {
						mState = State::Closing;
						ruPlaySound( mBeginSound );
					}
				}
			}
		}
		if( mState == State::Closing ) {
			mGateYOffset += 0.015f;
			if( mGateYOffset > 0.0f ) {
				mGateYOffset = 0.0f;
				mState = State::Closed;
				OnEndMoving();
			}
		}
		if( mState == State::Opening ) {
			mGateYOffset -= 0.015f;
			if( mGateYOffset < -mGateHeight ) {
				mGateYOffset = -mGateHeight;
				mState = State::Opened;
				OnEndMoving();
			}
		}

		if( mState == State::Opening || mState == State::Closing ) {
			if( !ruIsSoundPlaying( mBeginSound )) {
				ruPlaySound( mIdleSound );
			}
		}
		ruSetNodePosition( mGate, mInitialPosition + ruVector3( 0, mGateYOffset, 0 ));
	}
};