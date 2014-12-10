#include "Sparks.h"




void Sparks::Update()
{
    if( ruGetParticleSystemAliveParticles( ps ) <= 0 ) {
        alive = 0;
        ruFreeSceneNode( ps );
        ruFreeSound( es );
    }
}

Sparks::Sparks( ruNodeHandle at, ruSoundHandle emits )
{
    ruParticleSystemProperties psProps;
    psProps.texture = ruGetTexture( "data/textures/particles/p1.png");
    psProps.type = PS_BOX;
    psProps.speedDeviationMin = ruVector3( -0.0015, 0.02, -0.0015 );
    psProps.speedDeviationMax = ruVector3( 0.0015, -0.09, 0.0015 );
    psProps.colorBegin = ruVector3( 255, 255, 255 );
    psProps.colorEnd = ruVector3( 255, 255, 0 );
    psProps.pointSize = 0.025f;
    psProps.boundingBoxMin = ruVector3( -0.08, -0.2, -0.08 );
    psProps.boundingBoxMax = ruVector3( 0.08, 0.1, 0.08 );
    psProps.particleThickness = 20.5f;
    psProps.autoResurrectDeadParticles = false;
    psProps.useLighting = false;
    ps = ruCreateParticleSystem( 50, psProps );
    ruAttachNode( ps, at );
    ruAttachSound( emits, ps );
    ruPlaySound( emits );
    es = emits;
    alive = 1;
}
