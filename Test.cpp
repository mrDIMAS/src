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

/* shared_ptr\<.*\> .* */
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

    shared_ptr<ruSceneNode> cameraPivot = ruSceneNode::Create();
	cameraPivot->SetCapsuleBody( 6, 2 );
	cameraPivot->SetAngularFactor( ruVector3( 0, 0, 0 ));
    cameraPivot->SetPosition( ruVector3( 0, 5, 0 ));

    //shared_ptr<ruCamera> testCamera = ruCamera::Create( 90 );
    //testCamera->SetPosition( ruVector3( 10, 50, -100 ));

    shared_ptr<ruCamera> camera = ruCamera::Create( 60 );
    camera->Attach( cameraPivot );

    camera->SetSkybox(  
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_u.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_l.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_r.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_f.jpg" ), 
		ruTexture::Request( "data/textures/skyboxes/test/red_sky_b.jpg" )
	);

    camera->SetPosition( ruVector3( 0, 6, 0 ));

	shared_ptr<ruSceneNode> node = ruSceneNode::LoadFromFile( "data/newFormat.scene" );
	shared_ptr<ruLight> lit = dynamic_pointer_cast<ruLight>( ruSceneNode::FindByName( "Omni002" ));
	node->FindChild( "Box021" )->SetBlurAmount( 1.0f );

	float pitchTo = 0, yawTo = 0;
    float pitch = 0, yaw = 0;
    int cameraNum = 0;

    shared_ptr<ruFont> font = ruFont::LoadFromFile( 12, "data/fonts/font1.otf" );
    shared_ptr<ruFont> font2 = ruFont::LoadFromFile( 16, "data/fonts/font1.otf" );
    shared_ptr<ruFont> font3 = ruFont::LoadFromFile( 20, "data/fonts/font1.otf" );

    int counter = 0;
    int fps = 0;

	
	shared_ptr<ruParticleSystem> streamParticleEmitter = ruParticleSystem::Create( 256 );
	streamParticleEmitter->SetPosition( ruVector3( 10, 0, 10 ));
    streamParticleEmitter->SetTexture( ruTexture::Request( "data/textures/particles/p1.png" ));
    streamParticleEmitter->SetType( ruParticleSystem::Type::Stream );
    streamParticleEmitter->SetSpeedDeviation( ruVector3( -0.01, 0.0, -0.01 ), ruVector3(  0.01, 0.8,  0.01 ));
    streamParticleEmitter->SetBoundingRadius( 50 );
	streamParticleEmitter->SetLightingEnabled( true );
	
    shared_ptr<ruTimer> timer = ruTimer::Create();
    shared_ptr<ruTimer> perfTimer = ruTimer::Create();

    int perfTime=0;

    ruEngine::SetCursorSettings( ruTexture::Request( "data/gui/cursor.tga" ), 32, 32 );

	shared_ptr<ruGUIScene> guiScene = ruGUIScene::Create();

	guiScene->SetOpacity(0.5f);

    shared_ptr<ruText> fpsText = guiScene->CreateText( "Thisissuperduperlongwordtocrashwordwrap. Some super long text to test word wrapping. It must work for fuck sake!", 0, 0, 100, 100, font, ruVector3( 255, 255, 255 ), ruTextAlignment::Left, 150 );
	//shared_ptr<ruText> fpsText = guiScene->CreateText("The Mine", 0, 0, 100, 100, font, ruVector3(255, 255, 255), ruTextAlignment::Left, 150);

	shared_ptr<ruRect> testrect = guiScene->CreateRect( 100, 100, 200, 200, ruTexture::Request( "data/gui/inventory/items/detonator.png" ));
	shared_ptr<ruButton> testButton = guiScene->CreateButton( 10, 30, 128, 32, ruTexture::Request( "data/gui/menu/button.tga" ), "Test", font, ruVector3( 255, 255, 255 ), ruTextAlignment::Center );
	shared_ptr<ruButton> testButton2 = guiScene->CreateButton( 0, 100, 128, 32, ruTexture::Request( "data/gui/menu/button.tga" ), "Test", font, ruVector3( 255, 255, 255 ), ruTextAlignment::Center );
	testButton->Attach( testrect );
	testButton2->Attach( testrect );

	shared_ptr<ruGUIScene> anotherGUIScene = ruGUIScene::Create();
	shared_ptr<ruButton> testButton233 = anotherGUIScene->CreateButton(300, 100, 128, 32, ruTexture::Request("data/gui/menu/button.tga"), "Test", font, ruVector3(255, 255, 255), ruTextAlignment::Center);

	ruEngine::SetAmbientColor( ruVector3( 0.05, 0.05, 0.05 ));
	ruEngine::EnableSpotLightShadows( false );
	ruEngine::EnablePointLightShadows( true );
	ruEngine::SetHDREnabled( false );
	ruEngine::SetFXAAEnabled( false );
	ruEngine::SetParallaxEnabled( true );

	shared_ptr<ruSceneNode> cube = ruSceneNode::LoadFromFile( "data/cube.scene" );
	
	shared_ptr<ruSound> snd = ruSound::LoadMusic( "data/music/rf.ogg" );
	snd->SetVolume( 0.1 );

	shared_ptr<ruSceneNode> ripper = ruSceneNode::LoadFromFile( "data/models/ripper/ripper0.scene" );
	ruAnimation anim = ruAnimation( 0, 85, 8, true );
	anim.SetEnabled( true );
	ripper->SetAnimation( &anim );
	ripper->SetBlurAmount( 1.0f );

	for( int i = 0; i < ruPointLight::GetCount(); i++ ) {
		ruPointLight::Get( i )->SetGreyscaleFactor( 1.0f );
	}

	ruEngine::SetAmbientColor( ruVector3( .1, .1, .1 ));

    while( !ruInput::IsKeyDown( ruInput::Key::Esc )) {
        //idleAnim.Update();
        ruInput::Update();

		anim.Update();
		if( ruInput::IsMouseHit( ruInput::MouseButton::Right )) {
            ruEngine::SetHDREnabled( !ruEngine::IsHDREnabled() );
        }
		
		if( ruInput::IsKeyHit( ruInput::Key::T )) {
			ruEngine::ChangeVideomode( 2560, 1440, 0, 1 );
		}
		if( ruInput::IsKeyHit( ruInput::Key::Y )) {
			ruEngine::ChangeVideomode( 1920, 1080, 0, 1 );
		}
        ruVector3 speed;

        pitchTo += ruInput::GetMouseYSpeed() / 2.0;
        yawTo += -ruInput::GetMouseXSpeed() / 2.0;

        pitch = pitch + ( pitchTo - pitch ) * 0.2f;
        yaw = yaw + ( yawTo - yaw ) * 0.2f;

        ruQuaternion pitchRotation( ruVector3( 1, 0, 0 ), pitch );
        ruQuaternion yawRotation( ruVector3( 0, 1, 0 ), yaw );

        ruVector3 look = cameraPivot->GetLookVector() * 0.1f;
        ruVector3 right = cameraPivot->GetRightVector() * 0.1f;
		 
        ruPhysics::Update( 1.0f / 60.0f, 10, 1.0f / 60.0f );

		if( ruInput::IsKeyHit( ruInput::Key::Num1 )) {
			testrect->SetVisible( false );
			ruEngine::SetFXAAEnabled( true );
		}
		if( ruInput::IsKeyHit( ruInput::Key::Num2 )) {
			testrect->SetVisible( true );
			ruEngine::SetFXAAEnabled( false );
		}
		if( ruInput::IsKeyHit( ruInput::Key::Num3 )) {
			node.reset();
		}

		if( ruInput::IsMouseHit(  ruInput::MouseButton::Left )) {
			shared_ptr<ruSceneNode> newCube = ruSceneNode::Duplicate( cube );
			//newCube->Attach( camera );
			newCube->SetPosition( ruVector3( 0, 0, 1 ));

			ruEngine::EnableSpotLightShadows( !ruEngine::IsSpotLightShadowsEnabled() );
		}

        if( ruInput::IsKeyDown( ruInput::Key::W )) {
            speed = speed + look;
        }
        if( ruInput::IsKeyDown( ruInput::Key::S )) {
            speed = speed - look;
        }
        if( ruInput::IsKeyDown( ruInput::Key::A )) {
            speed = speed + right;
        }
        if( ruInput::IsKeyDown( ruInput::Key::D )) {
            speed = speed - right;
        }

        if( ruInput::IsKeyHit( ruInput::Key::Q )) {
            cameraNum = 1 - cameraNum;

			/*
            if( cameraNum ) {
                testCamera->SetActive();
            } else {
                camera->SetActive();
            }*/
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

		/*
        fpsText->SetText( 
			StringBuilder( "DIPs: " ) << ruEngine::GetDIPs() <<
			"\nTC: " << ruEngine::GetTextureUsedPerFrame() <<
			"\nFPS: " << fps <<
			"\nSC: " << ruEngine::GetShaderCountChangedPerFrame() <<
			"\nRT: " << ruEngine::GetRenderedTriangles()
		);
		*/
       	fpsText->SetVisible( true );


        perfTimer->Restart();
        ruEngine::RenderWorld( ); // fixed FPS
		ruEngine::UpdateWorld();
        perfTime = perfTimer->GetElapsedTimeInMilliSeconds();
    }

    ruEngine::Free();
}

#endif