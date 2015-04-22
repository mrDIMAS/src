#include "Precompiled.h"

#include "Tip.h"

void Tip::Deserialize( TextFileStream & in ) {
    mAlpha.Deserialize( in );
    in.ReadInteger( mX );
    mY.Deserialize( in );
    in.ReadInteger( mWidth );
    in.ReadInteger( mHeight );
}

void Tip::Serialize( TextFileStream & out ) {
    mAlpha.Serialize( out );
    out.WriteInteger( mX );
    mY.Serialize( out );
    out.WriteInteger( mWidth );
    out.WriteInteger( mHeight );
}

void Tip::AnimateAndDraw() {
    ruSetGUINodePosition( mGUIText, mX, mY );
	ruSetGUINodeAlpha( mGUIText, mAlpha );
    if( ruGetElapsedTimeInSeconds( mTimer ) > 1.5 ) {
        mAlpha.SetTarget( mAlpha.GetMin() );
        mAlpha.ChaseTarget( 0.1 );
        mY.SetTarget( mY.GetMax() );
        mY.ChaseTarget( 0.1 );
    };
}

void Tip::SetNewText( string text ) {
    ruRestartTimer( mTimer );
    mAlpha.Set( 255.0f );
	ruSetGUINodeText( mGUIText, text );
    mY.SetMax( ruGetResolutionHeight() - mHeight );
    mY.SetMin( ruGetResolutionHeight() / 2 + mHeight );
    mY.Set( mY.GetMin());
}

Tip::Tip() : mAlpha( 255.0f, 0.0f, 255.0f ) {
    mTimer = ruCreateTimer();
    mWidth = 256;
    mHeight = 32;
    mX = ruGetResolutionWidth() / 2 - mWidth / 2;
    mGUIText = ruCreateGUIText( " ", 0, 0, mWidth, mHeight, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 , mAlpha );
	SetNewText( " " );
}

void Tip::SetVisible( bool state ) {
	ruSetGUINodeVisible( mGUIText, state );
}

Tip::~Tip() {
	ruFreeGUINode( mGUIText );
}
