/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

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

    ruPointLight::SetPointDefaultTexture( ruCubeTexture::Request( "data/textures/generic/pointCube.dds" ));
    ruSpotLight::SetSpotDefaultTexture( ruTexture::Request( "data/textures/generic/spotlight.jpg" ));

    ruSceneNode * cameraPivot = ruSceneNode::Create();
	cameraPivot->SetCapsuleBody( 6, 2 );
	cameraPivot->SetAngularFactor( ruVector3( 0, 0, 0 ));
    cameraPivot->SetPosition( ruVector3( 0, 5, 0 ));

    ruCamera * testCamera = ruCamera::Create( 90 );
    testCamera->SetPosition( ruVector3( 10, 50, -100 ));

    ruCamera * camera = ruCamera::Create( 60 );
    camera->Attach( cameraPivot );

    camera->SetSkybox(  
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_u.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_l.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_r.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_f.jpg" ), 
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_b.jpg" )
	);

    camera->SetPosition( ruVector3( 0, 6, 0 ));

	ruSceneNode * node = ruSceneNode::LoadFromFile( "data/newFormat.scene" );
	ruLight * lit = dynamic_cast<ruLight*>( ruSceneNode::FindByName( "Omni002" ));
	node->FindChild( "Box021" )->SetBlurAmount( 1.0f );

	float pitchTo = 0, yawTo = 0;
    float pitch = 0, yaw = 0;
    int cameraNum = 0;

    ruFont * font = ruFont::LoadFromFile( 12, "data/fonts/font1.otf" );
    ruFont * font2 = ruFont::LoadFromFile( 16, "data/fonts/font1.otf" );
    ruFont * font3 = ruFont::LoadFromFile( 20, "data/fonts/font1.otf" );

    int counter = 0;
    int fps = 0;

	ruParticleSystem * streamParticleEmitter = ruParticleSystem::Create( 256 );
	streamParticleEmitter->SetPosition( ruVector3( 10, 0, 10 ));
    streamParticleEmitter->SetTexture( ruTexture::Request( "data/textures/particles/p1.png" ));
    streamParticleEmitter->SetType( ruParticleSystem::Type::Stream );
    streamParticleEmitter->SetSpeedDeviation( ruVector3( -0.01, 0.0, -0.01 ), ruVector3(  0.01, 0.8,  0.01 ));
    streamParticleEmitter->SetBoundingRadius( 50 );
	streamParticleEmitter->SetLightingEnabled( true );

    ruTimer * timer = ruTimer::Create();
    ruTimer * perfTimer = ruTimer::Create();

    int perfTime=0;

    ruEngine::EnableSpotLightShadows( false );
    

    ruEngine::SetCursorSettings( ruTexture::Request( "data/gui/cursor.png" ), 32, 32 );
    ruText * fpsText = ruText::Create( "Test text", 0, 0, 100, 100, font, ruVector3( 255, 255, 255 ), ruTextAlignment::Left, 150 );

	ruRect * testrect = ruRect::Create( 100, 100, 200, 200, ruTexture::Request( "data/gui/inventory/items/detonator.png" ));
	ruButton * testButton = ruButton::Create( 10, 30, 128, 32, ruTexture::Request( "data/gui/menu/button.tga" ), "Test", font, ruVector3( 255, 255, 255 ), ruTextAlignment::Center );
	ruButton * testButton2 = ruButton::Create( 0, 100, 128, 32, ruTexture::Request( "data/gui/menu/button.tga" ), "Test", font, ruVector3( 255, 255, 255 ), ruTextAlignment::Center );
	testButton->Attach( testrect );
	testButton2->Attach( testrect );

	ruEngine::SetAmbientColor( ruVector3( 0.05, 0.05, 0.05 ));
	ruEngine::SetHDREnabled( false );
	ruEngine::SetFXAAEnabled( false );

	ruSceneNode * cube = ruSceneNode::LoadFromFile( "data/cube.scene" );
	
	ruSound snd = ruSound::LoadMusic( "data/music/rf.ogg" );
	snd.SetVolume( 0.1 );

	ruSceneNode * ripper = ruSceneNode::LoadFromFile( "data/models/ripper/ripper0.scene" );
	ruAnimation anim = ruAnimation( 0, 85, 8, true );
	anim.enabled = true;
	ripper->SetAnimation( &anim );
	ripper->SetBlurAmount( 1.0f );

    while( !ruIsKeyDown( KEY_Esc )) {
        //idleAnim.Update();
        ruInputUpdate();

		anim.Update();
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

        ruVector3 look = cameraPivot->GetLookVector() * 0.1f;
        ruVector3 right = cameraPivot->GetRightVector() * 0.1f;
		 
        ruPhysics::Update( 1.0f / 60.0f, 10, 1.0f / 60.0f );

		if( ruIsKeyHit( KEY_1 )) {
			testrect->SetVisible( false );
			ruEngine::SetFXAAEnabled( true );
		}
		if( ruIsKeyHit( KEY_2 )) {
			testrect->SetVisible( true );
			ruEngine::SetFXAAEnabled( false );
		}
		if( ruIsKeyHit( KEY_3 )) {
			node->Free();
		}

		if( ruIsMouseHit( MB_Left )) {
			ruSceneNode * newCube = ruSceneNode::Duplicate( cube );
			newCube->Attach( camera );
			newCube->SetPosition( ruVector3( 0, 0, 1 ));
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
                testCamera->SetActive();
            } else {
                camera->SetActive();
            }
        }

        cameraPivot->Move( speed * ruVector3( 100, 1, 100 ));
        camera->SetRotation( pitchRotation );
        cameraPivot->SetRotation( yawRotation );

		//ripper.SetPosition( cameraPivot.GetPosition() );

        counter++;

        if( timer->GetElapsedTimeInSeconds() >= 1.0f ) {
            timer->Restart();
            fps = counter;
            counter = 0;
        }

        char buf[ 128 ];
        sprintf( buf, "DIPs: %d TC: %d FPS: %d Available Vid Mem, Mb: %i HDR: %i\n", ruEngine::GetDIPs(), ruEngine::GetTextureUsedPerFrame(), fps, ruEngine::GetAvailableTextureMemory() / ( 1024 * 1024 ), (int)ruEngine::IsHDREnabled()  );
        fpsText->SetText( buf );
        
        if( testButton->IsPressed() ) {
        	fpsText->SetVisible( true );
        } else {
        	fpsText->SetVisible( false );
        }

        perfTimer->Restart();
        ruEngine::RenderWorld( ); // fixed FPS
		ruEngine::UpdateWorld();
        perfTime = perfTimer->GetElapsedTimeInMilliSeconds();
    }

    ruEngine::Free();
}

#endif