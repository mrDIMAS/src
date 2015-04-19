#pragma once

#include "Game.h"
#include "Door.h"
#include "Player.h"

class Keypad {
private:
	ruNodeHandle mKeypad;
	ruNodeHandle mKeys[10];
	ruNodeHandle mKeyCancel;
	Door * mDoorToUnlock;
	string mCurrentCode;
	string mCodeToUnlock;
	bool mKeyState[10];
	ruVector3 mKeysInitialPosition[10];
	ruVector3 mKeysPressedOffsets[10];
	ruSoundHandle mButtonPushSound;
	ruSoundHandle mButtonPopSound;

	void Reset() {
		for( int i = 0; i < 10; i++ ) {
			mKeyState[i] = false;
			mCurrentCode.clear();
			ruSetNodePosition( mKeys[i], mKeysInitialPosition[i] );
		}
		ruPlaySound( mButtonPopSound );
	}
public:
	explicit Keypad( ruNodeHandle keypad, ruNodeHandle key0, ruNodeHandle key1, ruNodeHandle key2, ruNodeHandle key3, 
		ruNodeHandle key4, ruNodeHandle key5, ruNodeHandle key6, ruNodeHandle key7, ruNodeHandle key8, ruNodeHandle key9,
		ruNodeHandle keyCancel, Door * doorToUnlock, string codeToUnlock ) {

			mKeypad = keypad;
			mKeys[0] = key0;
			mKeys[1] = key1;
			mKeys[2] = key2;
			mKeys[3] = key3;
			mKeys[4] = key4;
			mKeys[5] = key5;
			mKeys[6] = key6;
			mKeys[7] = key7;
			mKeys[8] = key8;
			mKeys[9] = key9;
			mKeyCancel = keyCancel;

			mDoorToUnlock = doorToUnlock;
			mCodeToUnlock = codeToUnlock;

			for( int i = 0; i < 10; i++ ) {
				mKeysInitialPosition[i] = ruGetNodePosition( mKeys[i] );
				ruVector3 min = ruGetNodeAABBMin( mKeys[i] );
				ruVector3 max = ruGetNodeAABBMax( mKeys[i] );
				ruVector3 size = (max - min) / 2;
				ruVector3 right = ruGetNodeRightVector( mKeys[i]);
				mKeysPressedOffsets[i] = right * (size / 2);
			}

			mDoorToUnlock->SetLocked( true );

			Reset();

			mButtonPushSound = ruLoadSound3D( "data/sounds/button_push.ogg" );
			mButtonPopSound = ruLoadSound3D( "data/sounds/button_pop.ogg" );

			ruSetSoundPosition( mButtonPushSound, ruGetNodePosition( mKeypad ));
			ruSetSoundPosition( mButtonPopSound, ruGetNodePosition( mKeypad ));
	}

	void Update() {
		for( int i = 0; i < 10; i++ ) {
			if( pPlayer->mNearestPickedNode == mKeys[i] ) {
				if( !mKeyState[i] ) {
					pPlayer->SetActionText( StringBuilder() << i << pPlayer->mLocalization.GetString( "pressButton" ) );
					if( ruIsKeyHit( pPlayer->mKeyUse ) ) {
						mCurrentCode += to_string( i );
						mKeyState[i] = true;
						ruPlaySound( mButtonPushSound );
						if( mCurrentCode.size() == 4 ) {
							if( mCurrentCode == mCodeToUnlock ) {
								mDoorToUnlock->SetLocked( false );
								mDoorToUnlock->Open();								
							} 
							Reset();						
						} else {
							ruSetNodePosition( mKeys[i], mKeysInitialPosition[i] + mKeysPressedOffsets[i] );
						}
					}
				}
			}
		}

		if( pPlayer->mNearestPickedNode == mKeyCancel ) {
			pPlayer->SetActionText( pPlayer->mLocalization.GetString( "resetButtons" ) );
			if( ruIsKeyHit( pPlayer->mKeyUse ) ) {
				Reset();
			}
		}
	}
};