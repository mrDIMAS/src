#include "Sparks.h"




void Sparks::Update() {
    if( GetParticleSystemAliveParticles( ps ) <= 0 ) {
        alive = 0;
        FreeSceneNode( ps );
        FreeSoundSource( es );
    }
}

Sparks::Sparks( NodeHandle at, SoundHandle emits ) {
	ParticleSystemProperties psProps;
	psProps.texture = GetTexture( "data/textures/particles/p1.png");
	psProps.type = PS_BOX;
	psProps.speedDeviationMin = Vector3( -0.0015, 0.02, -0.0015 );
	psProps.speedDeviationMax = Vector3( 0.0015, -0.09, 0.0015 );
	psProps.colorBegin = Vector3( 255, 255, 255 );
	psProps.colorEnd = Vector3( 255, 255, 0 );
	psProps.pointSize = 0.025f;
	psProps.boundingBoxMin = Vector3( -0.08, -0.2, -0.08 );
	psProps.boundingBoxMax = Vector3( 0.08, 0.1, 0.08 );
	psProps.particleThickness = 20.5f;
	psProps.autoResurrectDeadParticles = false;
	psProps.useLighting = false;
    ps = CreateParticleSystem( 50, psProps );
    Attach( ps, at );
    AttachSound( emits, ps );
    PlaySoundSource( emits );
    es = emits;
    alive = 1;
}
