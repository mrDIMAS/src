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

	ruSceneNode mGate;
	float mGateHeight;
	ruSceneNode mButtonOpen[2];
	ruSceneNode mButtonClose[2];
	ruVector3 mInitialPosition;
	State mState;
	float mGateYOffset;
	ruSound mBeginSound;
	ruSound mIdleSound;
	ruSound mEndSound;
	ruSound mButtonSound;
	
	explicit Gate( ruSceneNode gate, ruSceneNode buttonOpen, ruSceneNode buttonClose, ruSceneNode buttonOpen2, ruSceneNode buttonClose2  ) {
		mGate = gate;
		mGateHeight = ( mGate.GetAABBMax() - mGate.GetAABBMin() ).y * 0.9f;
		mButtonClose[0] = buttonClose;
		mButtonOpen[0] = buttonOpen;
		mButtonClose[1] = buttonClose2;
		mButtonOpen[1] = buttonOpen2;
		mGateYOffset = 0.0f;
		mInitialPosition = mGate.GetPosition();
		mState = State::Closed;

		mBeginSound = ruSound::Load3D( "data/sounds/door_open_start.ogg" );
		mIdleSound = ruSound::Load3D( "data/sounds/door_open_idle.ogg" );
		mEndSound = ruSound::Load3D( "data/sounds/door_open_end.ogg" );
		mButtonSound = ruSound::Load3D( "data/sounds/button.ogg" );

		mBeginSound.SetPosition( mGate.GetPosition() );
		mIdleSound.SetPosition( mGate.GetPosition() );
		mEndSound.SetPosition( mGate.GetPosition() );
	}

	void OnEndMoving() {
		mIdleSound.Pause();
		mEndSound.Play();;
	}

	void Update() {
		if( pPlayer->mNearestPickedNode == mButtonOpen[0] || pPlayer->mNearestPickedNode == mButtonOpen[1] ) {
			if( !( mState == State::Closing || mState == State::Opening )) {
				pPlayer->SetActionText( "Открыть" );
				if( ruIsKeyHit( pPlayer->mKeyUse )) {
					mButtonSound.SetPosition( pPlayer->mNearestPickedNode.GetPosition() );
					mButtonSound.Play();
					if( mState != State::Opened ) {
						mState = State::Opening;
						mBeginSound.Play();
					}
				}
			}
		}
		if( pPlayer->mNearestPickedNode == mButtonClose[0] || pPlayer->mNearestPickedNode == mButtonClose[1] ) {
			if( !( mState == State::Closing || mState == State::Opening )) {
				pPlayer->SetActionText( "Закрыть" );
				if( ruIsKeyHit( pPlayer->mKeyUse )) {	
					mButtonSound.SetPosition( pPlayer->mNearestPickedNode.GetPosition() );
					mButtonSound.Play();
					if( mState != State::Closed ) {
						mState = State::Closing;
						mBeginSound.Play();
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
			if( !mBeginSound.IsPlaying() ) {
				mIdleSound.Play();;
			}
		}
		mGate.SetPosition( mInitialPosition + ruVector3( 0, mGateYOffset, 0 ));
	}
};