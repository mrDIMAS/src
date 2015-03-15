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
	}

	void Update() {
		if( pPlayer->mNearestPickedNode == mButtonOpen[0] ||
			pPlayer->mNearestPickedNode == mButtonOpen[1] ) {
			pPlayer->SetActionText( "Открыть" );
			if( ruIsKeyHit( pPlayer->mKeyUse )) {
				mState = State::Opening;
			}
		}
		if( pPlayer->mNearestPickedNode == mButtonClose[0] ||
			pPlayer->mNearestPickedNode == mButtonClose[1] ) {
			pPlayer->SetActionText( "Закрыть" );
			if( ruIsKeyHit( pPlayer->mKeyUse )) {
				mState = State::Closing;
			}
		}
		if( mState == State::Closing ) {
			mGateYOffset += 0.015f;
			if( mGateYOffset > 0.0f ) {
				mGateYOffset = 0.0f;
				mState = State::Closed;
			}
		}
		if( mState == State::Opening ) {
			mGateYOffset -= 0.015f;
			if( mGateYOffset < -mGateHeight ) {
				mGateYOffset = -mGateHeight;
				mState = State::Opened;
			}
		}
		ruSetNodePosition( mGate, mInitialPosition + ruVector3( 0, mGateYOffset, 0 ));
	}
};