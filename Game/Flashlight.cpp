#include "Flashlight.h"
#include "Player.h"

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
    ruSetNodePosition( mModel, mPosition + mOffset );

    if( pPlayer->mMoved ) {
        if( pPlayer->mRunning ) {
            mShakeCoeff += 11.5f * g_dt;
        } else {
            mShakeCoeff += 7.5f * g_dt;
        }
        mOffset = ruVector3( cosf( mShakeCoeff * 0.5f ) * 0.005f, sinf( mShakeCoeff ) * 0.01f, 0.0f );
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
    if( !mOn ) {
        ruPlaySound( mOnSound );

        mOn = true;

        mDestPosition = mInitialPosition;
    }
}

void Flashlight::SwitchOff() {
    if( mOn ) {
        ruPlaySound( mOffSound );

        mOn = false;

        mDestPosition = ruVector3( -1.0f, -1.0f, -1.0f );
    }
}

void Flashlight::Attach( ruNodeHandle node ) {
    ruAttachNode( mModel, node );

    mInitialPosition = ruGetNodePosition( mModel );
    mDestPosition = mInitialPosition;
    mPosition = mInitialPosition;
}

void Flashlight::Fuel() {
    mCharge = mMaxCharge;
}

bool Flashlight::GotCharge() {
    return mCharge > 0.0f;
}

Flashlight::Flashlight() {
    mModel = ruLoadScene( "data/models/hands/arm.scene" );
    ruSetNodeDepthHack( mModel, 0.155f );

    mLight = ruFindInObjectByName( mModel, "PlayerLight" );
    ruSetLightSpotTexture( mLight, ruGetTexture( "data/textures/generic/spotlight.jpg"));

    mOnSound = ruLoadSound2D( "data/sounds/flashlight/on.ogg" );
    mOffSound = ruLoadSound2D( "data/sounds/flashlight/off.ogg" );
    mOutOfChargeSound = ruLoadSound2D( "data/sounds/flashlight/outofcharge.ogg" );

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
	ruAttachNode( mFire, ruFindInObjectByName( mModel, "FirePlace" ));
}

void Flashlight::DeserializeWith( TextFileStream & in ) {
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

void Flashlight::SerializeWith( TextFileStream & out ) {
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
    return new Item( mModel, Item::Type::Flashlight );
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
