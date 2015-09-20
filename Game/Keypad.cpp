#include "Precompiled.h"

#include "Keypad.h"

void Keypad::Update()
{
	for( int i = 0; i < 10; i++ ) {
		if( pPlayer->mNearestPickedNode == mKeys[i] ) {
			if( !mKeyState[i] ) {
				pPlayer->SetActionText( StringBuilder() << i << pPlayer->mLocalization.GetString( "pressButton" ) );
				if( ruIsKeyHit( pPlayer->mKeyUse ) ) {
					mCurrentCode += to_string( i );
					mKeyState[i] = true;
					mButtonPushSound.Play();
					if( mCurrentCode.size() == 4 ) {
						if( mCurrentCode == mCodeToUnlock ) {
							mDoorToUnlock->SetLocked( false );
							mDoorToUnlock->Open();								
						} 
						Reset();						
					} else {
						mKeys[i].SetPosition( mKeysInitialPosition[i] + mKeysPressedOffsets[i] );
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

Keypad::Keypad( ruSceneNode keypad, ruSceneNode key0, ruSceneNode key1, ruSceneNode key2, ruSceneNode key3, ruSceneNode key4, ruSceneNode key5, ruSceneNode key6, ruSceneNode key7, ruSceneNode key8, ruSceneNode key9, ruSceneNode keyCancel, Door * doorToUnlock, string codeToUnlock )
{
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
		mKeysInitialPosition[i] = mKeys[i].GetPosition();
		ruVector3 min = mKeys[i].GetAABBMin();
		ruVector3 max = mKeys[i].GetAABBMax();
		ruVector3 size = (max - min) / 2;
		ruVector3 right = mKeys[i].GetRightVector();
		mKeysPressedOffsets[i] = right * (size / 2);
	}

	mDoorToUnlock->SetLocked( true );

	Reset();

	mButtonPushSound = ruSound::Load3D( "data/sounds/button_push.ogg" );
	mButtonPopSound = ruSound::Load3D( "data/sounds/button_pop.ogg" );

	mButtonPushSound.SetPosition( mKeypad.GetPosition() );
	mButtonPopSound.SetPosition( mKeypad.GetPosition() );
}

void Keypad::Reset()
{
	for( int i = 0; i < 10; i++ ) {
		mKeyState[i] = false;
		mCurrentCode.clear();
		mKeys[i].SetPosition( mKeysInitialPosition[i] );
	}
	mButtonPopSound.Play();
}
