#include "Precompiled.h"

#include "SteamStream.h"

void SteamStream::Update() {
    ruParticleSystemProperties * psProps = ruGetParticleSystemProperties( ps );
    psProps->pointSize = power * 0.15;
    psProps->boundingRadius = power * 0.8;
    snd.Play();
    snd.SetVolume( power );
}

SteamStream::SteamStream( ruSceneNode obj, ruVector3 speedMin, ruVector3 speedMax, ruSound hissSound ) {
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
    ps.SetPosition( obj.GetPosition() );
    snd.Attach( ps );
	snd.SetRolloffFactor( 5 );
	snd.SetReferenceDistance( 0.4 );
	snd.SetRoomRolloffFactor( 2.5f );
    power = 1;
}
