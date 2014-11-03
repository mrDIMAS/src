#include "SteamStream.h"




void SteamStream::Update() {
    SetParticleSystemPointSize( ps, power * 0.15 );
    SetParticleSystemRadius( ps, power * 0.8 );
    PlaySoundSource( snd );
    SetVolume( snd, power );
}

SteamStream::SteamStream( NodeHandle obj, Vector3 speedMin, Vector3 speedMax, SoundHandle hissSound ) {
    snd = hissSound;
    ps = CreateParticleSystem( 60, GetTexture( "data/textures/particles/p1.png"), PS_STREAM );
    SetParticleSystemSpeedDeviation( ps, speedMin, speedMax );
    SetParticleSystemColors( ps, Vector3( 255, 255, 255 ),  Vector3( 255, 255, 255 ) );
    SetParticleSystemPointSize( ps, 0.5 );
    SetParticleSystemThickness( ps, 1.5 );
    SetParticleSystemRadius( ps, 0.8 );
    DisableParticleSystemLighting( ps );
    SetParticleSystemParticleScaling( ps, 0.05 );
    //Attach( ps, obj );
    SetPosition( ps, GetPosition( obj ));
    AttachSound( snd, ps );
	SetSoundReferenceDistance( snd, 15.0f );
	SetRolloffFactor( snd, 50.0f );
    power = 1;
}
