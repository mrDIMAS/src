#include "SteamStream.h"

void SteamStream::Update() {
	ParticleSystemProperties * psProps = GetParticleSystemProperties( ps );
    psProps->pointSize = power * 0.15;
    psProps->boundingRadius = power * 0.8;
    PlaySoundSource( snd );
    SetVolume( snd, power );
}

SteamStream::SteamStream( NodeHandle obj, Vector3 speedMin, Vector3 speedMax, SoundHandle hissSound ) {
    snd = hissSound;
	ParticleSystemProperties psProps;
	psProps.type = PS_STREAM;
	psProps.speedDeviationMin = speedMin;
	psProps.speedDeviationMax = speedMax;
	psProps.texture = GetTexture( "data/textures/particles/p1.png");
	psProps.colorBegin = Vector3( 255, 255, 255 );
	psProps.colorEnd = Vector3( 255, 255, 255 );
	psProps.pointSize = 0.5f;
	psProps.particleThickness = 1.5f;
	psProps.boundingRadius = 0.8f;
	psProps.useLighting = false;
	psProps.scaleFactor = 0.05f;
    ps = CreateParticleSystem( 60, psProps );
    SetPosition( ps, GetPosition( obj ));
    AttachSound( snd, ps );
	SetSoundReferenceDistance( snd, 15.0f );
	SetRolloffFactor( snd, 50.0f );
    power = 1;
}
