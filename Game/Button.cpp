#include "Precompiled.h"
#include "Button.h"
#include "Player.h"
#include "Utils.h"

void Button::Update() {
	if( pPlayer->mNearestPickedNode == mNode ) {
		pPlayer->SetActionText( StringBuilder() << ruInput::GetKeyName( pPlayer->mKeyUse ) << "Запустить генератор" );
		if( ruInput::IsKeyHit( pPlayer->mKeyUse )) {
			OnPush.DoActions();
			mPushSound.Play();
			mPush = true;
		}
		if( ruInput::IsKeyDown( pPlayer->mKeyUse )) {
			mNode->SetPosition( mInitialPosition - mAxis * mSize / 2.0f );
		}
	} 
	if( !ruInput::IsKeyDown( pPlayer->mKeyUse ) ) {
		mNode->SetPosition( mInitialPosition );
	}
	if( mPush ) {
		mPopSound.Play();
		mPush = false;
	}
}

Button::Button( shared_ptr<ruSceneNode> node, const ruVector3 & axis, ruSound pushSound, ruSound popSound ) 
	: mNode( node ), mAxis( axis ), mPush( false ), mPushSound( pushSound ), mPopSound( popSound ) {
	mInitialPosition = mNode->GetPosition();
	mSize = (mNode->GetAABBMax() - mNode->GetAABBMin()).Abs();
	mPushSound.Attach( mNode );
	mPopSound.Attach( mNode );
}
