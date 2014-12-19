
#ifdef _TEST

#include "Engine.h"

#include <Windows.h>
#include <iostream>
using namespace std;

void main( ) {
    ruVector3 v1( 10, 20, 30 );
    ruVector3 v2( 30, 20, 10 );

    ruVector3 v3 = v1 - v2;

    v3 = v1 + v2;
    v3 *= ruVector3( 10, 10, 10 );

    v3 = 10 * v1 ;

    ruCreateRenderer( 0, 0, 0, 0 );

    ruSetLightPointDefaultTexture( ruGetCubeTexture( "data/textures/generic/pointCube.dds" ));
    ruSetLightSpotDefaultTexture( ruGetTexture( "data/textures/generic/spotlight.jpg" ));

    ruNodeHandle cameraPivot = ruCreateSceneNode();
    ruSetCapsuleBody( cameraPivot, 6, 2 );
    ruSetAngularFactor( cameraPivot, ruVector3( 0, 0, 0 ));
    ruSetNodePosition( cameraPivot, ruVector3( 0, 5, 0 ));

    ruNodeHandle testCamera = ruCreateCamera( 90 );
    ruSetNodePosition( testCamera, ruVector3( 10, 50, -100 ));

    ruNodeHandle camera = ruCreateCamera( 60 );
    ruAttachNode( camera, cameraPivot );
    ruSetCameraSkybox( camera, "data/textures/skyboxes/test/red_sky");
    ruSetNodePosition( camera, ruVector3( 0, 6, 0 ));

    //int node = LoadScene( "data/maps/release/arrival/arrival.scene" );
    ruNodeHandle node = ruLoadScene( "data/newFormat.scene" );

    //int node = LoadScene( "data/maps/testingChamber/testingChamber.scene" );

    // int node = LoadScene( "data/maps/release/1/1.scene" );

    float pitchTo = 0, yawTo = 0;
    float pitch = 0, yaw = 0;
    int cameraNum = 0;

    ruFontHandle font = ruCreateGUIFont( 12, "data/fonts/font1.otf", 1, 0 );
    ruFontHandle font2 = ruCreateGUIFont( 16, "data/fonts/font1.otf", 1, 0 );
    ruFontHandle font3 = ruCreateGUIFont( 20, "data/fonts/font1.otf", 1, 0 );

    int counter = 0;
    int fps = 0;



    ruParticleSystemProperties streamParticleEmitterProps;
    streamParticleEmitterProps.texture = ruGetTexture( "data/textures/particles/p1.png" );
    streamParticleEmitterProps.type = PS_STREAM;
    streamParticleEmitterProps.speedDeviationMin = ruVector3( -0.01, 0.0, -0.01 );
    streamParticleEmitterProps.speedDeviationMax = ruVector3(  0.01, 0.8,  0.01 );
    streamParticleEmitterProps.boundingRadius = 50;

    ruNodeHandle streamParticleEmitter = ruCreateParticleSystem( 256, streamParticleEmitterProps );

    ruTimerHandle timer = ruCreateTimer();

    ruTimerHandle perfTimer = ruCreateTimer();
    int perfTime=0;

    ruEnablePointLightShadows( false );
    ruEnableSpotLightShadows( false );
    ruDisableFXAA();

    ruSetCursorSettings( ruGetTexture( "data/gui/cursor.png" ), 32, 32 );
    ruTextHandle fpsText = ruCreateGUIText( "Test text", 0, 0, 100, 100, font, ruVector3( 255, 255, 255 ), 0, 150 );
    ruButtonHandle testButton = ruCreateGUIButton( 200, 200, 128, 32, ruGetTexture( "data/gui/button.png" ), "Test", font, ruVector3( 255, 255, 255 ), 1 );
	ruRectHandle testrect = ruCreateGUIRect( 0, 0, 200, 200, ruGetTexture( "data/gui/inventory/items/detonator.png" ));
    while( !ruIsKeyDown( KEY_Esc )) {
        //idleAnim.Update();
        ruInputUpdate();

        if( ruIsMouseHit( MB_Right )) {
            ruSetHDREnabled( !ruIsHDREnabled() );
        }

        ruVector3 speed;

        pitchTo += ruGetMouseYSpeed() / 2.0;
        yawTo += -ruGetMouseXSpeed() / 2.0;

        pitch = pitch + ( pitchTo - pitch ) * 0.2f;
        yaw = yaw + ( yawTo - yaw ) * 0.2f;

        ruQuaternion pitchRotation( ruVector3( 1, 0, 0 ), pitch );
        ruQuaternion yawRotation( ruVector3( 0, 1, 0 ), yaw );

        ruVector3 look = ruGetNodeLookVector( cameraPivot );
        ruVector3 right = ruGetNodeRightVector( cameraPivot );

        //SetPosition( Omni09, GetPosition( camera ));
        //ruSetNodePosition( streamParticleEmitter, ruGetNodePosition( cameraPivot ));
        //DrawGUIRect( 0, 0, 200, 200, 0 );
        //    DrawGUIText( "TEST", 200, 200, 100, 100, font, Vector3( 255, 0, 0 ) );

        ruUpdatePhysics( 1.0f / 60.0f, 10, 1.0f / 60.0f );

        if( ruIsKeyHit( KEY_1 )) {
            ruSetRenderQuality( 0 );
        }
        if( ruIsKeyHit( KEY_2 )) {
            ruSetRenderQuality( 1 );
        }
        if( ruIsKeyDown( KEY_W )) {
            speed = speed + look;
        }
        if( ruIsKeyDown( KEY_S )) {
            speed = speed - look;
        }
        if( ruIsKeyDown( KEY_A )) {
            speed = speed + right;
        }
        if( ruIsKeyDown( KEY_D )) {
            speed = speed - right;
        }

        if( ruIsKeyHit( KEY_Q )) {
            cameraNum = 1 - cameraNum;

            if( cameraNum ) {
                ruSetActiveCamera( testCamera );
            } else {
                ruSetActiveCamera( camera );
            }
        }

        //if( !IsAnimationEnabled( dummy )) {
        //	SetAnimationEnabled( dummy, true );
        //}

        ruMoveNode( cameraPivot, speed * ruVector3( 100, 1, 100 ));
        ruSetNodeRotation( camera, pitchRotation );
        ruSetNodeRotation( cameraPivot, yawRotation );

        counter++;

        if( ruGetElapsedTimeInSeconds( timer ) >= 1 ) {
            ruRestartTimer( timer );
            fps = counter;
            counter = 0;
        }
        char buf[ 128 ];
        //ruDrawGUIRect( 400, 400, 34, 34, ruTextureHandle::Empty() );
        sprintf( buf, "DIPs: %d TC: %d FPS: %d Available Vid Mem, Mb: %i HDR: %i\n", ruDIPs(), ruTextureUsedPerFrame(), fps, ruGetAvailableTextureMemory() / ( 1024 * 1024 ), (int)ruIsHDREnabled()  );
        ruSetGUINodeText( fpsText, buf );
        /*
        if( ruIsButtonPressed( testButton ))
        {
        	ruSetGUINodeVisible( fpsText, true );
        }
        else
        {
        	ruSetGUINodeVisible( fpsText, false );
        }*/
        // ruDrawGUIText( buf, 0, 0, 200, 500, font, ruVector3( 255, 0, 255 ), 0, 100 );
        //DrawGUIText( "Это текст с переносом \tслов и он работает отлично( или нет )", 0, 0, 200, 500, font, Vector3( 255, 0, 255 ), 0, 100 );
        //ruDrawGUIText( "Простой текст", 200, 0, 100, 100, font2, ruVector3( 255, 0, 255 ), 0, 100 );
        //ruDrawGUIText( "Простой текст", 400, 0, 100, 100, font3, ruVector3( 255, 0, 255 ), 0, 100 );

        ruRestartTimer( perfTimer );
        ruRenderWorld( 1.0f / 60.0f ); // fixed FPS
        perfTime=ruGetElapsedTimeInMilliSeconds( perfTimer );
    }

    ruFreeRenderer();
}

#endif