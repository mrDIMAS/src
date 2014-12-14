#include "Flashlight.h"
#include "Player.h"

void Flashlight::Update()
{
    if( mOn )
    {
        mRangeDest = mOnRange;

        mCharge -= g_dt / mChargeWorkTimeSeconds;

        if( mCharge < 0.1f )
            mCharge = 0.1f;
    }
    else
        mRangeDest = 0.0f;

    mRealRange += ( mRangeDest - mRealRange ) * 0.15f;
    ruSetLightRange( mLight, mRealRange * mCharge );
    mPosition = mPosition.Lerp( mDestPosition, 0.15f );
    ruSetNodePosition( mModel, mPosition + mOffset );

    if( pPlayer->mMoved )
    {
        if( pPlayer->mRunning )
            mShakeCoeff += 11.5f * g_dt;
        else
            mShakeCoeff += 7.5f * g_dt;
        mOffset = ruVector3( cosf( mShakeCoeff * 0.5f ) * 0.02f, sinf( mShakeCoeff ) * 0.02f, 0.0f );
    }
}

void Flashlight::Switch()
{
    if( mOn )
        SwitchOff();
    else
        SwitchOn();
}

void Flashlight::SwitchOn()
{
    if( !mOn )
    {
        ruPlaySound( mOnSound );

        mOn = true;

        mDestPosition = mInitialPosition;
    }
}

void Flashlight::SwitchOff()
{
    if( mOn )
    {
        ruPlaySound( mOffSound );

        mOn = false;

        mDestPosition = ruVector3( -1.0f, -1.0f, -1.0f );
    }
}

void Flashlight::Attach( ruNodeHandle node )
{
    ::ruAttachNode( mModel, node );

    mInitialPosition = ruGetNodePosition( mModel );
    mDestPosition = mInitialPosition;
    mPosition = mInitialPosition;
}

void Flashlight::Fuel()
{
    mCharge = mMaxCharge;
}

bool Flashlight::GotCharge()
{
    return mCharge > 0.0f;
}

Flashlight::Flashlight()
{
    mModel = ruLoadScene( "data/models/hands/arm.scene" );
    ruSetNodeDepthHack( mModel, 0.1f );

    mLight = ruFindInObjectByName( mModel, "PlayerLight" );
    ruSetLightSpotTexture( mLight, ruGetTexture( "data/textures/generic/spotlight.jpg"));

    mOnSound = ruLoadSound2D( "data/sounds/flashlight/on.ogg" );
    mOffSound = ruLoadSound2D( "data/sounds/flashlight/off.ogg" );
    mOutOfChargeSound = ruLoadSound2D( "data/sounds/flashlight/outofcharge.ogg" );

    mOnRange = ruGetLightRange( mLight );

    mRealRange = mOnRange;
    mRangeDest = mOnRange;

    mMaxCharge = 1.0f;
    mCharge = mMaxCharge;

    mChargeWorkTimeSeconds = 240.0f;
    mShakeCoeff = 0.0f;
    mOn = true;
}

void Flashlight::DeserializeWith( TextFileStream & in )
{
    in.ReadFloat( mMaxCharge );
    in.ReadFloat( mCharge );
    in.ReadFloat( mOnRange );
    in.ReadFloat( mRealRange );
    in.ReadFloat( mRangeDest );
    in.ReadBoolean( mOn );
    if( mOn )
        SwitchOn();
    else
        SwitchOff();
}

void Flashlight::SerializeWith( TextFileStream & out )
{
    out.WriteFloat( mMaxCharge );
    out.WriteFloat( mCharge );
    out.WriteFloat( mOnRange );
    out.WriteFloat( mRealRange );
    out.WriteFloat( mRangeDest );
    out.WriteBoolean( mOn );
}

Flashlight::~Flashlight()
{

}

Item * Flashlight::CreateAppropriateItem()
{
	return new Item( mModel, Item::Type::Flashlight );
}

bool Flashlight::IsBeamContainsPoint( ruVector3 point ) const
{
	return ruIsLightSeePoint( mLight, point );
}

float Flashlight::GetCharge()
{
	return mCharge;
}

bool Flashlight::IsOn() const
{
	return mOn;
}
