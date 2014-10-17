#include "Fire.h"




Fire::~Fire() {
    FreeSceneNode( light );
    FreeSceneNode( particleSystem );
}

Fire::Fire( float size, float height ) {
    particleSystem = CreateParticleSystem( 64, GetTexture( "data/textures/particles/p1.png"), PS_STREAM );
    SetParticleSystemSpeedDeviation( particleSystem, Vector3( -0.005, 0.08, -0.005 ), Vector3( 0.005, 0.02, 0.005 ) );
    SetParticleSystemBox( particleSystem, Vector3( -size, 0, -size ), Vector3( size, height, size ) );
    SetParticleSystemColors( particleSystem, Vector3( 220, 70, 0 ),  Vector3( 255, 255, 0 ) );
    SetParticleSystemPointSize( particleSystem, 0.6 );
    SetParticleSystemThickness( particleSystem, 1 );
    SetParticleSystemRadius( particleSystem, 0.8 );
    SetParticleSystemParticleScaling( particleSystem, -0.005 );

    light = CreateLight( LT_POINT );
    SetLightRange( light, size * 4 );
    Attach( light, particleSystem );
}
