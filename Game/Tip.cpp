#include "Precompiled.h"

#include "Tip.h"

/*
void Tip::Deserialize( SaveFile & in ) {
    mAlpha.Deserialize( in );
    in.ReadInteger( mX );
    mY.Deserialize( in );
    in.ReadInteger( mWidth );
    in.ReadInteger( mHeight );
}

void Tip::Serialize( SaveFile & out ) {
    mAlpha.Serialize( out );
    out.WriteInteger( mX );
    mY.Serialize( out );
    out.WriteInteger( mWidth );
    out.WriteInteger( mHeight );
}
*/
void Tip::AnimateAndDraw() {
    mGUIText->SetPosition( mX, mY );
	mGUIText->SetAlpha( mAlpha );
    if( mTimer->GetElapsedTimeInSeconds() > 1.5 ) {
        mAlpha.SetTarget( mAlpha.GetMin() );
        mAlpha.ChaseTarget( 0.1 );
        mY.SetTarget( mY.GetMax() );
        mY.ChaseTarget( 0.1 );
    };
}

void Tip::SetNewText( string text ) {
    mTimer->Restart();
    mAlpha.Set( 255.0f );
	mGUIText->SetText( text );
    mY.SetMax( ruEngine::GetResolutionHeight() - mHeight );
    mY.SetMin( ruEngine::GetResolutionHeight() / 2 + mHeight );
    mY.Set( mY.GetMin());
}

Tip::Tip() : mAlpha( 255.0f, 0.0f, 255.0f ) {
    mTimer = ruTimer::Create();
    mWidth = 256;
    mHeight = 32;
    mX = ruEngine::GetResolutionWidth() / 2 - mWidth / 2;
    mGUIText = ruText::Create( " ", 0, 0, mWidth, mHeight, pGUIProp->mFont, ruVector3( 255, 0, 0 ), ruTextAlignment::Center, mAlpha );
	SetNewText( " " );
}

void Tip::SetVisible( bool state ) {
	mGUIText->SetVisible( state );
}

Tip::~Tip() {

}
