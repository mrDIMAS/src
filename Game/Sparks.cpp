#include "Sparks.h"




void Sparks::Update() {
    if( GetParticleSystemAliveParticles( ps ) <= 0 ) {
        alive = 0;
        FreeSceneNode( ps );
        FreeSoundSource( es );
    }
}

Sparks::Sparks( NodeHandle at, SoundHandle emits ) {
    ps = CreateParticleSystem( 50, GetTexture( "data/textures/particles/p1.png"));
    SetParticleSystemSpeedDeviation( ps, Vector3( -0.0015, 0.02, -0.0015 ), Vector3( 0.0015, -0.09, 0.0015 ) );
    SetParticleSystemBox( ps, Vector3( -0.08, -0.2, -0.08 ), Vector3( 0.08, 0.1, 0.08 ) );
    SetParticleSystemColors( ps, Vector3( 255, 255, 0 ),  Vector3( 255, 255, 0 ) );
    SetParticleSystemPointSize( ps, 0.025 );
    SetParticleSystemThickness( ps, 20.5 );
    DisableParticleSystemLighting( ps );
    SetParticleSystemAutoResurrect( ps, 0 );
    Attach( ps, at );
    AttachSound( emits, ps );
    PlaySoundSource( emits );
    es = emits;
    alive = 1;
}
