#include "Flashlight.h"
#include "Player.h"

void Flashlight::Update()
{
    if( on )
    {
        rangeDest = onRange;

        charge -= g_dt / chargeWorkTimeSeconds;

        if( charge < 0.1f )
            charge = 0.1f;
    }
    else
        rangeDest = 0.0f;

    realRange += ( rangeDest - realRange ) * 0.15f;
    ruSetLightRange( light, realRange * charge );
    position = position.Lerp( destPosition, 0.15f );
    ruSetNodePosition( model, position + offset );

    if( pPlayer->mMoved )
    {
        if( pPlayer->mRunning )
            bobArg += 11.5f * g_dt;
        else
            bobArg += 7.5f * g_dt;
        offset = ruVector3( cosf( bobArg * 0.5f ) * 0.02f, sinf( bobArg ) * 0.02f, 0.0f );
    }
}

void Flashlight::Switch()
{
    if( on )
        SwitchOff();
    else
        SwitchOn();
}

void Flashlight::SwitchOn()
{
    if( !on )
    {
        ruPlaySound( onSound );

        on = true;

        destPosition = initialPosition;
    }
}

void Flashlight::SwitchOff()
{
    if( on )
    {
        ruPlaySound( offSound );

        on = false;

        destPosition = ruVector3( -1.0f, -1.0f, -1.0f );
    }
}

void Flashlight::Attach( ruNodeHandle node )
{
    ::ruAttachNode( model, node );

    initialPosition = ruGetNodePosition( model );
    destPosition = initialPosition;
    position = initialPosition;
}

void Flashlight::Fuel()
{
    charge = maxCharge;
}

bool Flashlight::GotCharge()
{
    return charge > 0.0f;
}

Flashlight::Flashlight()
{
    model = ruLoadScene( "data/models/hands/arm.scene" );
    ruSetNodeDepthHack( model, 0.1f );

    light = ruFindInObjectByName( model, "PlayerLight" );
    ruSetLightSpotTexture( light, ruGetTexture( "data/textures/generic/spotlight.jpg"));

    onSound = ruLoadSound2D( "data/sounds/flashlight/on.ogg" );
    offSound = ruLoadSound2D( "data/sounds/flashlight/off.ogg" );
    outOfChargeSound = ruLoadSound2D( "data/sounds/flashlight/outofcharge.ogg" );

    onRange = ruGetLightRange( light );

    realRange = onRange;
    rangeDest = onRange;

    maxCharge = 1.0f;
    charge = maxCharge;

    chargeWorkTimeSeconds = 120.0f;
    bobArg = 0.0f;
    on = true;
}

void Flashlight::DeserializeWith( TextFileStream & in )
{
    in.ReadFloat( maxCharge );
    in.ReadFloat( charge );
    in.ReadFloat( onRange );
    in.ReadFloat( realRange );
    in.ReadFloat( rangeDest );
    in.ReadBoolean( on );
    if( on )
        SwitchOn();
    else
        SwitchOff();
}

void Flashlight::SerializeWith( TextFileStream & out )
{
    out.WriteFloat( maxCharge );
    out.WriteFloat( charge );
    out.WriteFloat( onRange );
    out.WriteFloat( realRange );
    out.WriteFloat( rangeDest );
    out.WriteBoolean( on );
}
