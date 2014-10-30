
#ifdef _TEST

#include "Engine.h"

#include <Windows.h>
#include <iostream>
using namespace std;

void main( ) {
    Vector3 v1( 10, 20, 30 );
    Vector3 v2( 30, 20, 10 );

    Vector3 v3 = v1 - v2;

    v3 = v1 + v2;
    v3 *= Vector3( 10, 10, 10 );

    v3 = 10 * v1 ;

    CreateRenderer( 0, 0, 0 );

    SetPointDefaultTexture( GetCubeTexture( "data/textures/generic/pointCube.dds" ));
    SetSpotDefaultTexture( GetTexture( "data/textures/generic/spotlight.jpg" ));

    NodeHandle cameraPivot = CreateSceneNode();
    SetCapsuleBody( cameraPivot, 6, 2 );
    SetAngularFactor( cameraPivot, Vector3( 0, 0, 0 ));
    SetPosition( cameraPivot, Vector3( 0, 5, 0 ));

    NodeHandle testCamera = CreateCamera( 90 );
    SetPosition( testCamera, Vector3( 10, 50, -100 ));

    NodeHandle camera = CreateCamera( 60 );
    Attach( camera, cameraPivot );
    SetSkybox( camera, "data/textures/skyboxes/test/red_sky");
    SetPosition( camera, Vector3( 0, 6, 0 ));

    //int node = LoadScene( "data/maps/release/arrival/arrival.scene" );
    NodeHandle node = LoadScene( "data/newFormat.scene" );

    //int node = LoadScene( "data/maps/testingChamber/testingChamber.scene" );

// int node = LoadScene( "data/maps/release/1/1.scene" );

    float pitchTo = 0, yawTo = 0;
    float pitch = 0, yaw = 0;
    int cameraNum = 0;

    FontHandle font = CreateGUIFont( 16, "Arial", 1, 0 );

    //int ripper = LoadScene( "data/models/ripper/ripper.scene" );
    /*
    int particleSystem = CreateParticleSystem( 1000, GetTexture( "data/textures/particles/p1.png" ), PS_BOX );
    SetParticleSystemSpeedDeviation( particleSystem, Vector3( -0.1, 0.0, -0.1 ),  Vector3(  0.1, 0.4,  0.1 ) );
    SetParticleSystemBox( particleSystem, Vector3( -100, 0, -100 ), Vector3( 100, 100, 100 ) );
    SetParticleSystemColors( particleSystem, Vector3( 0, 255, 0 ),  Vector3( 0, 0, 255 ) );
    SetParticleSystemPointSize( particleSystem, 10 );*/
    //EnableParticleSystemLighting( particleSystem );

// SetAmbientColor( Vector3( 1, 1,1 ));

// SetLightColor( FindByName( "Fspot003" ), Vector3( 255, 0, 0 ));


    int counter = 0;
    int fps = 0;

    //int Omni09 = FindByName( "Omni09" );
    //SetLightRange( Omni09, 20 );

    //DisableFXAA();
    /*
    int steamPS = CreateParticleSystem( 35, GetTexture( "data/textures/particles/p1.png"), PS_STREAM );
    SetParticleSystemSpeedDeviation( steamPS, Vector3( -0.0015, 0.08, -0.0015 ), Vector3( 0.0015, 0.2, 0.0015 ) );
    SetParticleSystemRadius( steamPS, 0.45f );
    SetParticleSystemColors( steamPS, Vector3( 255, 255, 255 ),  Vector3( 255, 255, 255 ) );
    SetParticleSystemPointSize( steamPS, 0.15 );
    SetParticleSystemThickness( steamPS, 1.5 );
    SetParticleSystemAutoResurrect( steamPS, true );
    Attach( steamPS, cameraPivot );*/


// int dummy = LoadScene( "data/models/ripper/ripper.scene" );

//  NodeHandle rl = FindInObjectByName( dummy, "RightLeg" );

    TimerHandle timer = CreateTimer();

    TimerHandle perfTimer = CreateTimer();
    int perfTime=0;
    while( !mi::KeyDown( mi::Esc )) {
        if( mi::KeyHit( mi::E ))
            //if( !Animating( node ))
        {
            Animate(node, 0.05, 2 );
        }


        mi::Update();


        Vector3 speed;

        pitchTo += mi::MouseYSpeed() / 2.0;
        yawTo += -mi::MouseXSpeed() / 2.0;

        pitch = pitch + ( pitchTo - pitch ) * 0.2f;
        yaw = yaw + ( yawTo - yaw ) * 0.2f;

        Quaternion pitchRotation( Vector3( 1, 0, 0 ), pitch );
        Quaternion yawRotation( Vector3( 0, 1, 0 ), yaw );

        Vector3 look = GetLookVector( cameraPivot );
        Vector3 right = GetRightVector( cameraPivot );

        //SetPosition( Omni09, GetPosition( camera ));

        //DrawGUIRect( 0, 0, 200, 200, 0 );
//    DrawGUIText( "TEST", 200, 200, 100, 100, font, Vector3( 255, 0, 0 ) );

        if( mi::KeyDown( mi::W )) {
            speed = speed + look;
        }
        if( mi::KeyDown( mi::S )) {
            speed = speed - look;
        }
        if( mi::KeyDown( mi::A )) {
            speed = speed + right;
        }
        if( mi::KeyDown( mi::D )) {
            speed = speed - right;
        }

        if( mi::KeyHit( mi::Q )) {
            cameraNum = 1 - cameraNum;

            if( cameraNum ) {
                SetCamera( testCamera );
            } else {
                SetCamera( camera );
            }
        }

        //if( Animating( ripper ))
        //  SetPosition( ripper, GetPosition( ripper ) + Vector3( 0, 0, 0.5 ));

        //if( mi::KeyHit( mi::R ))
        //{
        //  Animate( ripper, 0.08, 1 );
        //}

        // speed = VectorAdd( speed, Vector3( 0, -9.81, 0 ));

        /*
        if( !Animating( dummy ))
        {
          Animate( dummy, 0.08, 1 );
        }
        else
        {
          //SetPosition( dummy, GetPosition( dummy ) - Vector3( 0, 0, 0.05 ));
          Move( dummy, Vector3( 0, 0, -0.05 ) );
        }
        */
        Move( cameraPivot, speed * Vector3( 100, 1, 100 ));
        SetRotation( camera, pitchRotation );
        SetRotation( cameraPivot, yawRotation );

        counter++;

        if( GetElapsedTimeInSeconds( timer ) >= 1 ) {
            RestartTimer( timer );
            fps = counter;
            counter = 0;
        }
        char buf[ 128 ];
        sprintf( buf, "DIPs: %d\nTC: %d\nFPS: %d\nAvailable Vid Mem, Mb: %i", DIPs(), TextureUsedPerFrame(), fps, GetAvailableTextureMemory() / ( 1024 * 1024 )  );
        DrawGUIText( buf, 0, 0, 200, 100, font, Vector3( 255, 0, 255 ), 0, 100 );
        RestartTimer( perfTimer );
        RenderWorld();
        perfTime=GetElapsedTimeInMilliSeconds( perfTimer );
    }

    FreeRenderer();
}

#endif