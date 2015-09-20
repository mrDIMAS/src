#include "Precompiled.h"

#include "Flashlight.h"
#include "Player.h"

Flashlight::Flashlight() {
	mModel = ruLoadScene( "data/models/hands/arm.scene" );
	mModel.SetDepthHack( 0.155f );

	mLight = ruFindInObjectByName( mModel, "PlayerLight" );
	ruSetLightSpotTexture( mLight, ruGetTexture( "data/textures/generic/spotlight.jpg"));

	mOnSound = ruSound::Load2D( "data/sounds/lighter/open.ogg" );
	mOffSound = ruSound::Load2D( "data/sounds/lighter/close.ogg" );
	mFireSound = ruSound::Load2D( "data/sounds/lighter/fire.ogg" );
	mOutOfChargeSound = ruSound::Load2D( "data/sounds/flashlight/outofcharge.ogg" );

	mOnRange = ruGetLightRange( mLight );

	const float lim = 0.02;
	ruSetLightFloatingLimits( mLight, ruVector3( -lim, -lim, -lim ), ruVector3( lim, lim, lim ));

	mRealRange = mOnRange;
	mRangeDest = mOnRange;

	mMaxCharge = 1.0f;
	mCharge = mMaxCharge;

	mChargeWorkTimeSeconds = 240.0f;
	mShakeCoeff = 0.0f;
	mOn = true;

	mCloseAnim = ruAnimation( 20, 34, 0.9 );
	mCloseAnim.AddFrameListener( 26, ruDelegate::Bind( this, &Flashlight::Close ));
	mCloseAnim.AddFrameListener( 34, ruDelegate::Bind( this, &Flashlight::Hide ));
	mOpenAnim = ruAnimation( 35, 70, 2.8 );
	mOpenAnim.AddFrameListener( 44, ruDelegate::Bind( this, &Flashlight::Open ));
	mOpenAnim.AddFrameListener( 55, ruDelegate::Bind( this, &Flashlight::Fire ));
	mOpenAnim.AddFrameListener( 35, ruDelegate::Bind( this, &Flashlight::Show ));
	mIdleAnim = ruAnimation( 0, 19, 5 );

	ruParticleSystemProperties psProps;
	psProps.type = PS_STREAM;
	psProps.speedDeviationMin = ruVector3( -0.00001f, 0.001f, -0.00001f );
	psProps.speedDeviationMax = ruVector3( 0.000012f, 0.0015f, 0.000012f );
	psProps.texture = ruGetTexture( "data/textures/particles/p1.png" );
	psProps.colorBegin = ruVector3( 0, 55, 244 );
	psProps.colorEnd = ruVector3( 255, 127, 39 );
	psProps.pointSize = 0.0095f;
	psProps.particleThickness = 1.5f;
	psProps.boundingRadius = 0.004f;
	psProps.useLighting = false;
	psProps.depthHack = 0.1551f;
	psProps.scaleFactor = -0.00008f;
	mFire = ruCreateParticleSystem( 60, psProps );
	mFire.Attach( ruFindInObjectByName( mModel, "FirePlace" ));
}

void Flashlight::Update() {
    if( mOn ) {
        mRangeDest = mOnRange;

        //mCharge -= g_dt / mChargeWorkTimeSeconds;

        if( mCharge < 0.1f ) {
            mCharge = 0.1f;
        }
    } else {
        mRangeDest = 0.0f;
    }

    mRealRange += ( mRangeDest - mRealRange ) * 0.15f;
    ruSetLightRange( mLight, mRealRange * mCharge );
    mPosition = mPosition.Lerp( mDestPosition, 0.15f );
    mModel.SetPosition( mPosition + mOffset );

    if( pPlayer->mMoved ) {
        if( pPlayer->mRunning ) {
            mShakeCoeff += 11.5f * g_dt;
        } else {
            mShakeCoeff += 7.5f * g_dt;
        }
        mOffset = ruVector3( cosf( mShakeCoeff * 0.5f ) * 0.005f, sinf( mShakeCoeff ) * 0.01f, 0.0f );
    }

	mOpenAnim.Update();
	mCloseAnim.Update();
	mIdleAnim.Update();
	
	if( mOn && !mCloseAnim.enabled && !mOpenAnim.enabled ) {
		mModel.SetAnimation( &mIdleAnim );
		mIdleAnim.enabled = true;
	}	
}

void Flashlight::Switch() {
    if( mOn ) {
        SwitchOff();
    } else {
        SwitchOn();
    }
}

void Flashlight::SwitchOn() {
	if( !mCloseAnim.enabled && !mOpenAnim.enabled ) {
		if( !mOn ) {
			mOpenAnim.Rewind();
			mModel.SetAnimation( &mOpenAnim );
			mOpenAnim.enabled = true;
			mIdleAnim.enabled = false;
		}
	}
}

void Flashlight::SwitchOff() {
	if( !mOpenAnim.enabled && !mCloseAnim.enabled ) {
		if( mOn ) {		
			mCloseAnim.Rewind();
			mModel.SetAnimation( &mCloseAnim );
			mCloseAnim.enabled = true;
			mIdleAnim.enabled = false;
		}
	}
}

void Flashlight::Attach( ruSceneNode node ) {
    mModel.Attach( node );

    mInitialPosition = mModel.GetPosition();
    mDestPosition = mInitialPosition;
    mPosition = mInitialPosition;
}

void Flashlight::Fuel() {
    mCharge = mMaxCharge;
}

bool Flashlight::GotCharge() {
    return mCharge > 0.0f;
}



void Flashlight::Deserialize( SaveFile & in ) {
    in.ReadFloat( mMaxCharge );
    in.ReadFloat( mCharge );
    in.ReadFloat( mOnRange );
    in.ReadFloat( mRealRange );
    in.ReadFloat( mRangeDest );
    in.ReadBoolean( mOn );
    if( mOn ) {
        SwitchOn();
    } else {
        SwitchOff();
    }
}

void Flashlight::Serialize( SaveFile & out ) {
    out.WriteFloat( mMaxCharge );
    out.WriteFloat( mCharge );
    out.WriteFloat( mOnRange );
    out.WriteFloat( mRealRange );
    out.WriteFloat( mRangeDest );
    out.WriteBoolean( mOn );
}

Flashlight::~Flashlight() {

}

Item * Flashlight::CreateAppropriateItem() {
    return new Item( mModel, Item::Type::Lighter );
}

bool Flashlight::IsBeamContainsPoint( ruVector3 point ) const {
    return ruIsLightSeePoint( mLight, point );
}

float Flashlight::GetCharge() {
    return mCharge;
}

bool Flashlight::IsOn() const {
    return mOn;
}
