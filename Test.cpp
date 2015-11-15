#include "Precompiled.h"

#ifdef _TEST

#include "RutheniumAPI.h"

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

    ruEngine::Create( 0, 0, 0, 0 );

    ruSetLightPointDefaultTexture( ruGetCubeTexture( "data/textures/generic/pointCube.dds" ));
    ruSetLightSpotDefaultTexture( ruGetTexture( "data/textures/generic/spotlight.jpg" ));

    ruSceneNode cameraPivot = ruSceneNode::Create();
	cameraPivot.SetCapsuleBody( 6, 2 );
	cameraPivot.SetAngularFactor( ruVector3( 0, 0, 0 ));
    cameraPivot.SetPosition( ruVector3( 0, 5, 0 ));

    ruSceneNode testCamera = ruCreateCamera( 90 );
    testCamera.SetPosition( ruVector3( 10, 50, -100 ));

    ruSceneNode camera = ruCreateCamera( 60 );
    camera.Attach( cameraPivot );

    ruSetCameraSkybox( camera, 
		ruGetTexture( "data/textures/skyboxes/test/red_sky_u.jpg" ),
		ruGetTexture( "data/textures/skyboxes/test/red_sky_l.jpg" ),
		ruGetTexture( "data/textures/skyboxes/test/red_sky_r.jpg" ),
		ruGetTexture( "data/textures/skyboxes/test/red_sky_f.jpg" ), 
		ruGetTexture( "data/textures/skyboxes/test/red_sky_b.jpg" ));

    camera.SetPosition( ruVector3( 0, 6, 0 ));

    //int node = LoadScene( "data/maps/release/arrival/arrival.scene" );
    //ruNodeHandle node = ruLoadScene( "data/maps/release/arrival/arrival.scene");//ruLoadScene( "data/newFormat.scene" );
	ruSceneNode node = ruSceneNode::LoadFromFile( "data/newFormat.scene" );
	//ruSetNodePosition( cameraPivot, ruGetNodePosition( ruFindInObjectByName( node, "PlayerPosition")));
    //int node = LoadScene( "data/maps/testingChamber/testingChamber.scene" );

    // int node = LoadScene( "data/maps/release/1/1.scene" );

    float pitchTo = 0, yawTo = 0;
    float pitch = 0, yaw = 0;
    int cameraNum = 0;

    ruFontHandle font = ruCreateGUIFont( 12, "data/fonts/font1.otf" );
    ruFontHandle font2 = ruCreateGUIFont( 16, "data/fonts/font1.otf" );
    ruFontHandle font3 = ruCreateGUIFont( 20, "data/fonts/font1.otf" );

    int counter = 0;
    int fps = 0;



    ruParticleSystemProperties streamParticleEmitterProps;
    streamParticleEmitterProps.texture = ruGetTexture( "data/textures/particles/p1.png" );
    streamParticleEmitterProps.type = PS_STREAM;
    streamParticleEmitterProps.speedDeviationMin = ruVector3( -0.01, 0.0, -0.01 );
    streamParticleEmitterProps.speedDeviationMax = ruVector3(  0.01, 0.8,  0.01 );
    streamParticleEmitterProps.boundingRadius = 50;
	streamParticleEmitterProps.useLighting = true;

    ruSceneNode streamParticleEmitter = ruCreateParticleSystem( 256, streamParticleEmitterProps );

    ruTimerHandle timer = ruCreateTimer();

    ruTimerHandle perfTimer = ruCreateTimer();
    int perfTime=0;

    ruEngine::EnableSpotLightShadows( false );
    

    ruEngine::SetCursorSettings( ruGetTexture( "data/gui/cursor.png" ), 32, 32 );
    ruTextHandle fpsText = ruCreateGUIText( "Test text", 0, 0, 100, 100, font, ruVector3( 255, 255, 255 ), 0, 150 );
    
	ruRectHandle testrect = ruCreateGUIRect( 100, 100, 200, 200, ruGetTexture( "data/gui/inventory/items/detonator.png" ));
	ruButtonHandle testButton = ruCreateGUIButton( 10, 30, 128, 32, ruGetTexture( "data/gui/menubutton.png" ), "Test", font, ruVector3( 255, 255, 255 ), 1 );
	ruButtonHandle testButton2 = ruCreateGUIButton( 0, 100, 128, 32, ruGetTexture( "data/gui/menubutton.png" ), "Test", font, ruVector3( 255, 255, 255 ), 1 );
	ruAttachGUINode( testButton, testrect );
	ruAttachGUINode( testButton2, testrect );
	//ruNodeHandle testScene = ruLoadScene( "data/test.scene" );
	//ruNodeHandle testScene = ruLoadScene( "data/maps/release/mine/mine.scene" );
	ruEngine::SetAmbientColor( ruVector3( 0.05, 0.05, 0.05 ));
	ruEngine::SetHDREnabled( false );
	ruEngine::DisableFXAA();

	ruSceneNode cube = ruSceneNode::LoadFromFile( "data/cube.scene" );
	
	ruSound snd = ruSound::LoadMusic( "data/music/rf.ogg" );
	snd.SetVolume( 0.1 );

    while( !ruIsKeyDown( KEY_Esc )) {
        //idleAnim.Update();
        ruInputUpdate();

        if( ruIsMouseHit( MB_Right )) {
            ruEngine::SetHDREnabled( !ruEngine::IsHDREnabled() );
        }
		
		if( ruIsKeyHit( KEY_T )) {
			ruEngine::ChangeVideomode( 2560, 1440, 0, 1 );
		}
		if( ruIsKeyHit( KEY_Y )) {
			ruEngine::ChangeVideomode( 1920, 1080, 0, 1 );
		}
        ruVector3 speed;

        pitchTo += ruGetMouseYSpeed() / 2.0;
        yawTo += -ruGetMouseXSpeed() / 2.0;

        pitch = pitch + ( pitchTo - pitch ) * 0.2f;
        yaw = yaw + ( yawTo - yaw ) * 0.2f;

        ruQuaternion pitchRotation( ruVector3( 1, 0, 0 ), pitch );
        ruQuaternion yawRotation( ruVector3( 0, 1, 0 ), yaw );

        ruVector3 look = cameraPivot.GetLookVector();
        ruVector3 right = cameraPivot.GetRightVector();
		 
        //SetPosition( Omni09, GetPosition( camera ));
        //ruSetNodePosition( streamParticleEmitter, ruGetNodePosition( cameraPivot ));
        //DrawGUIRect( 0, 0, 200, 200, 0 );
        //    DrawGUIText( "TEST", 200, 200, 100, 100, font, Vector3( 255, 0, 0 ) );

        ruUpdatePhysics( 1.0f / 60.0f, 10, 1.0f / 60.0f );

		if( ruIsKeyHit( KEY_1 )) {
			ruSetGUINodeVisible( testrect, false );
			ruEngine::EnableFXAA();
		}
		if( ruIsKeyHit( KEY_2 )) {
			ruSetGUINodeVisible( testrect, true );
			ruEngine::DisableFXAA();
		}
		if( ruIsKeyHit( KEY_3 )) {
			node.Free();
		}

		if( ruIsMouseHit( MB_Left )) {
			ruSceneNode newCube = ruSceneNode::Duplicate( cube );
			newCube.Attach( camera );
			newCube.SetPosition( ruVector3( 0, 0, 1 ));
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

        cameraPivot.Move( speed * ruVector3( 100, 1, 100 ));
        camera.SetRotation( pitchRotation );
        cameraPivot.SetRotation( yawRotation );

        counter++;

        if( ruGetElapsedTimeInSeconds( timer ) >= 1 ) {
            ruRestartTimer( timer );
            fps = counter;
            counter = 0;
        }

        char buf[ 128 ];
        sprintf( buf, "DIPs: %d TC: %d FPS: %d Available Vid Mem, Mb: %i HDR: %i\n", ruEngine::GetDIPs(), ruEngine::GetTextureUsedPerFrame(), fps, ruEngine::GetAvailableTextureMemory() / ( 1024 * 1024 ), (int)ruEngine::IsHDREnabled()  );
        ruSetGUINodeText( fpsText, buf );
        
        if( ruIsButtonPressed( testButton )) {
        	ruSetGUINodeVisible( fpsText, true );
        } else {
        	ruSetGUINodeVisible( fpsText, false );
        }

        ruRestartTimer( perfTimer );
        ruEngine::RenderWorld( ); // fixed FPS
		ruEngine::UpdateWorld();
        perfTime=ruGetElapsedTimeInMilliSeconds( perfTimer );
    }

    ruEngine::Free();
}

#endif