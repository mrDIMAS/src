#include "Precompiled.h"

#include "SteamStream.h"

void SteamStream::Update() {
    ruParticleSystemProperties * psProps = ruGetParticleSystemProperties( ps );
    psProps->pointSize = power * 0.15;
    psProps->boundingRadius = power * 0.8;
    ruPlaySound( snd );
    ruSetSoundVolume( snd, power );
}

SteamStream::SteamStream( ruNodeHandle obj, ruVector3 speedMin, ruVector3 speedMax, ruSoundHandle hissSound ) {
    snd = hissSound;
    ruParticleSystemProperties psProps;
    psProps.type = PS_STREAM;
    psProps.speedDeviationMin = speedMin;
    psProps.speedDeviationMax = speedMax;
    psProps.texture = ruGetTexture( "data/textures/particles/p1.png");
    psProps.colorBegin = ruVector3( 255, 255, 255 );
    psProps.colorEnd = ruVector3( 255, 255, 255 );
    psProps.pointSize = 0.5f;
    psProps.particleThickness = 1.5f;
    psProps.boundingRadius = 0.8f;
    psProps.useLighting = true;
    psProps.scaleFactor = 0.05f;
    ps = ruCreateParticleSystem( 60, psProps );
    ruSetNodePosition( ps, ruGetNodePosition( obj ));
    ruAttachSound( snd, ps );
	ruSetRolloffFactor( snd, 5 );
	ruSetSoundReferenceDistance( snd, 0.4 );
	ruSetRoomRolloffFactor( snd, 2.5f );
    power = 1;
}
