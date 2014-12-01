
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

	FontHandle font = CreateGUIFont( 12, "data/fonts/font1.otf", 1, 0 );

	int counter = 0;
	int fps = 0;

	/*
	// Particle system test
	ParticleSystemProperties boxParticleEmitterProps;
	boxParticleEmitterProps.texture = GetTexture( "data/textures/particles/p1.png" );
	boxParticleEmitterProps.type = PS_BOX;
	boxParticleEmitterProps.useLighting = true;
	boxParticleEmitterProps.autoResurrectDeadParticles = true;
	boxParticleEmitterProps.speedDeviationMin = Vector3( -0.05, 0.0, -0.05 );
	boxParticleEmitterProps.speedDeviationMax = Vector3(  0.05, 0.05,  0.05 );
	boxParticleEmitterProps.boundingBoxMin = Vector3( -5, 0, -5 );
	boxParticleEmitterProps.boundingBoxMax = Vector3( 5, 5, 5 );
	boxParticleEmitterProps.colorBegin = Vector3( 255, 0, 0 );
	boxParticleEmitterProps.colorEnd = Vector3( 0, 255, 0 );
	
	NodeHandle boxParticleEmitter = CreateParticleSystem( 1024, boxParticleEmitterProps );
	*/
	
	ParticleSystemProperties streamParticleEmitterProps;
	streamParticleEmitterProps.texture = GetTexture( "data/textures/particles/p1.png" );
	streamParticleEmitterProps.type = PS_STREAM;
	streamParticleEmitterProps.speedDeviationMin = Vector3( -0.01, 0.0, -0.01 );
	streamParticleEmitterProps.speedDeviationMax = Vector3(  0.01, 0.8,  0.01 );
	streamParticleEmitterProps.boundingRadius = 50;

	NodeHandle streamParticleEmitter = CreateParticleSystem( 256, streamParticleEmitterProps );
	
	// Animation test
	//NodeHandle dummy = LoadScene( "data/models/ripper/ripper.scene" );
	//Animation idleAnim = Animation( 0, GetTotalAnimationFrameCount( dummy ), 3.0f, false );	
	//SetAnimation( dummy, &idleAnim );

	TimerHandle timer = CreateTimer();

	TimerHandle perfTimer = CreateTimer();
	int perfTime=0;

	EnablePointLightShadows( false );
	EnableSpotLightShadows( true );
	while( !IsKeyDown( KEY_Esc )) {

		//idleAnim.Update();
		InputUpdate();

		if( IsMouseHit( MB_Right )) {
			SetHDREnabled( !IsHDREnabled() );
		}
		Vector3 speed;

		pitchTo += GetMouseYSpeed() / 2.0;
		yawTo += -GetMouseXSpeed() / 2.0;

		pitch = pitch + ( pitchTo - pitch ) * 0.2f;
		yaw = yaw + ( yawTo - yaw ) * 0.2f;

		Quaternion pitchRotation( Vector3( 1, 0, 0 ), pitch );
		Quaternion yawRotation( Vector3( 0, 1, 0 ), yaw );

		Vector3 look = GetLookVector( cameraPivot );
		Vector3 right = GetRightVector( cameraPivot );

		//SetPosition( Omni09, GetPosition( camera ));

		//DrawGUIRect( 0, 0, 200, 200, 0 );
		//    DrawGUIText( "TEST", 200, 200, 100, 100, font, Vector3( 255, 0, 0 ) );

		if( IsKeyDown( KEY_W )) {
			speed = speed + look;
		}
		if( IsKeyDown( KEY_S )) {
			speed = speed - look;
		}
		if( IsKeyDown( KEY_A )) {
			speed = speed + right;
		}
		if( IsKeyDown( KEY_D )) {
			speed = speed - right;
		}

		if( IsKeyHit( KEY_Q )) {
			cameraNum = 1 - cameraNum;

			if( cameraNum ) {
				SetCamera( testCamera );
			} else {
				SetCamera( camera );
			}
		}

		//if( !IsAnimationEnabled( dummy )) {
		//	SetAnimationEnabled( dummy, true );
		//}

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
		//sprintf( buf, "DIPs: %d TC: %d FPS: %d Available Vid Mem, Mb: %i HDR: %i\n", DIPs(), TextureUsedPerFrame(), fps, GetAvailableTextureMemory() / ( 1024 * 1024 ), (int)IsHDREnabled()  );
		//DrawGUIText( buf, 0, 0, 200, 500, font, Vector3( 255, 0, 255 ), 0, 100 );
		DrawGUIText( "Это текст с переносом \tслов и он работает отлично( или нет )", 0, 0, 200, 500, font, Vector3( 255, 0, 255 ), 0, 100 );
		RestartTimer( perfTimer );
		RenderWorld( 1.0f / 60.0f ); // fixed FPS
		perfTime=GetElapsedTimeInMilliSeconds( perfTimer );
	}

	FreeRenderer();
}

#endif