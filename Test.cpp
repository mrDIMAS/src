/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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
void main() {
	try {
		auto engine = IEngine::Create(0, 0, 0, 0);

		IPointLight::SetPointDefaultTexture(ICubeTexture::Request("data/textures/generic/pointCube.dds"));
		ISpotLight::SetSpotDefaultTexture(ITexture::Request("data/textures/generic/spotlight.jpg"));

		shared_ptr<ISceneNode> cameraPivot = engine->GetSceneFactory()->CreateSceneNode();
		cameraPivot->SetCapsuleBody(6, 2);
		cameraPivot->SetAngularFactor(Vector3(0, 0, 0));
		cameraPivot->SetPosition(Vector3(0, 5, 0));

		shared_ptr<ICamera> camera = engine->GetSceneFactory()->CreateCamera(60);
		camera->Attach(cameraPivot);

		camera->SetSkybox(
			ITexture::Request("data/textures/skyboxes/test/red_sky_u.jpg"),
			ITexture::Request("data/textures/skyboxes/test/red_sky_l.jpg"),
			ITexture::Request("data/textures/skyboxes/test/red_sky_r.jpg"),
			ITexture::Request("data/textures/skyboxes/test/red_sky_f.jpg"),
			ITexture::Request("data/textures/skyboxes/test/red_sky_b.jpg")
		);

		camera->SetPosition(Vector3(0, 6, 0));

		shared_ptr<ISceneNode> node = engine->GetSceneFactory()->LoadScene("data/newFormat.scene");
		shared_ptr<ILight> lit = dynamic_pointer_cast<ILight>(engine->GetSceneFactory()->FindByName("Omni002"));
		node->FindChild("Box021")->SetBlurAmount(1.0f);

		float pitchTo = 0, yawTo = 0;
		float pitch = 0, yaw = 0;
		int cameraNum = 0;

		shared_ptr<IFont> font = engine->CreateBitmapFont(12, "data/fonts/font5.ttf");
		shared_ptr<IFont> font2 = engine->CreateBitmapFont(16, "data/fonts/font5.ttf");
		shared_ptr<IFont> font3 = engine->CreateBitmapFont(20, "data/fonts/font5.ttf");

		int counter = 0;
		int fps = 0;


		shared_ptr<IParticleSystem> streamParticleEmitter = engine->GetSceneFactory()->CreateParticleSystem(256);
		streamParticleEmitter->SetPosition(Vector3(10, 0, 10));
		streamParticleEmitter->SetTexture(ITexture::Request("data/textures/particles/p1.png"));
		streamParticleEmitter->SetType(IParticleSystem::Type::Stream);
		streamParticleEmitter->SetSpeedDeviation(Vector3(-0.01, 0.0, -0.01), Vector3(0.01, 0.8, 0.01));
		streamParticleEmitter->SetBoundingRadius(50);
		streamParticleEmitter->SetLightingEnabled(true);

		shared_ptr<ITimer> timer = ITimer::Create();
		shared_ptr<ITimer> perfTimer = ITimer::Create();

		int perfTime = 0;

		engine->GetRenderer()->SetCursor(ITexture::Request("data/gui/cursor.tga"), 32, 32);

		shared_ptr<IGUIScene> guiScene = engine->CreateGUIScene();

		guiScene->SetOpacity(0.5f);

		shared_ptr<IText> fpsText = guiScene->CreateText(u8"Русский текст тоже рисуется! Thisissuperduperlongwordtocrashwordwrap. Some super long text to test word wrapping. It must work for fuck sake! ", 0, 0, 100, 100, font, Vector3(255, 255, 255), TextAlignment::Left, 150);
		//shared_ptr<IText> fpsText = guiScene->CreateText("The Mine", 0, 0, 100, 100, font, Vector3(255, 255, 255), TextAlignment::Left, 150);

		shared_ptr<IRect> testrect = guiScene->CreateRect(100, 100, 200, 200, ITexture::Request("data/gui/inventory/items/detonator.png"));
		shared_ptr<IButton> testButton = guiScene->CreateButton(10, 30, 128, 32, ITexture::Request("data/gui/menu/button.tga"), u8"Русский текст", font, Vector3(255, 255, 255), TextAlignment::Center);
		shared_ptr<IButton> testButton2 = guiScene->CreateButton(0, 100, 128, 32, ITexture::Request("data/gui/menu/button.tga"), "Test", font, Vector3(255, 255, 255), TextAlignment::Center);
		testButton->Attach(testrect);
		testButton2->Attach(testrect);

		shared_ptr<IGUIScene> anotherGUIScene =  engine->CreateGUIScene();
		shared_ptr<IButton> testButton233 = anotherGUIScene->CreateButton(300, 100, 128, 32, ITexture::Request("data/gui/menu/button.tga"), "Test", font, Vector3(255, 255, 255), TextAlignment::Center);


		engine->GetRenderer()->SetSpotLightShadowsEnabled(false);
		engine->GetRenderer()->SetPointLightShadowsEnabled(false);
		engine->GetRenderer()->SetHDREnabled(false);
		engine->GetRenderer()->SetFXAAEnabled(false);
		engine->GetRenderer()->SetParallaxEnabled(false);

		shared_ptr<ISceneNode> cube = engine->GetSceneFactory()->LoadScene("data/cube.scene");

		shared_ptr<ISound> snd = engine->GetSoundSystem()->LoadMusic("data/music/rf.ogg");
		snd->SetVolume(0.1);


		/*
		shared_ptr<ISceneNode> ripper = ISceneNode::LoadFromFile("data/models/ripper/ripper0.scene");
		Animation anim = Animation(0, 85, 3, true);
		anim.SetEnabled(true);
		ripper->SetAnimation(&anim);
		ripper->SetBlurAmount(1.0f);

		auto dummyTest = ISceneNode::LoadFromFile("data/models/character/character.scene");
		auto dummyAnim = Animation(82, 90, 0.9, true);
		dummyAnim.SetDirection(Animation::Direction::Reverse);
		dummyAnim.SetEnabled(true);
		dummyTest->SetAnimation(&dummyAnim);
		dummyTest->SetPosition(Vector3(1, 0, -3));
		*/

		engine->GetRenderer()->SetAmbientColor(Vector3(.01, .01, .01));

		auto bone008 = node->FindChild("Bone011");
		auto bone001 = node->FindChild("Bone004");
		bone001->Detach();

		auto fog = engine->GetSceneFactory()->CreateFog(Vector3(-100, -1, -100), Vector3(100, 5, 100), Vector3(0.7, 0.7, 0.9), 0.1);
		fog->SetPosition(Vector3(0, 0, -20));

		while(!engine->GetInput()->IsKeyDown(IInput::Key::Esc)) {
			//idleAnim.Update();
			engine->GetInput()->Update();

			//anim.Update();
			//dummyAnim.Update();



			if(engine->GetInput()->IsMouseHit(IInput::MouseButton::Right)) {
				//IEngine::SetHDREnabled(!IEngine::IsHDREnabled());
				engine->GetRenderer()->SetSpotLightShadowsEnabled(engine->GetRenderer()->IsSpotLightShadowsEnabled());
			}

			Vector3 speed;

			pitchTo += engine->GetInput()->GetMouseYSpeed() / 2.0;
			yawTo += -engine->GetInput()->GetMouseXSpeed() / 2.0;

			pitch = pitch + (pitchTo - pitch) * 0.2f;
			yaw = yaw + (yawTo - yaw) * 0.2f;

			Quaternion pitchRotation(Vector3(1, 0, 0), pitch);
			Quaternion yawRotation(Vector3(0, 1, 0), yaw);

			Vector3 look = cameraPivot->GetLookVector() * 0.1f;
			Vector3 right = cameraPivot->GetRightVector() * 0.1f;

			engine->GetPhysics()->Update(1.0f / 60.0f, 10, 1.0f / 60.0f);

			if(engine->GetInput()->IsKeyHit(IInput::Key::Num1)) {
				testrect->SetVisible(false);
				engine->GetRenderer()->SetFXAAEnabled(true);
			}
			if(engine->GetInput()->IsKeyHit(IInput::Key::Num2)) {
				testrect->SetVisible(true);
				engine->GetRenderer()->SetFXAAEnabled(false);
			}
			if(engine->GetInput()->IsKeyHit(IInput::Key::Num3)) {
				node.reset();
			}

			if(engine->GetInput()->IsMouseHit(IInput::MouseButton::Left)) {
				shared_ptr<ISceneNode> newCube = engine->GetSceneFactory()->CreateSceneNodeDuplicate(cube);
				//newCube->Attach( camera );
				newCube->SetPosition(Vector3(0, 0, 1));

				engine->GetRenderer()->SetSpotLightShadowsEnabled(!engine->GetRenderer()->IsSpotLightShadowsEnabled());
			}

			if(engine->GetInput()->IsKeyDown(IInput::Key::W)) {
				speed = speed + look;
			}
			if(engine->GetInput()->IsKeyDown(IInput::Key::S)) {
				speed = speed - look;
			}
			if(engine->GetInput()->IsKeyDown(IInput::Key::A)) {
				speed = speed + right;
			}
			if(engine->GetInput()->IsKeyDown(IInput::Key::D)) {
				speed = speed - right;
			}

			if(engine->GetInput()->IsKeyHit(IInput::Key::Q)) {
				cameraNum = 1 - cameraNum;

				/*
				if( cameraNum ) {
					testCamera->SetActive();
				} else {
					camera->SetActive();
				}*/
			}

			cameraPivot->Move(speed * Vector3(500, 1, 500));
			camera->SetRotation(pitchRotation);
			cameraPivot->SetRotation(yawRotation);
			//ripper.SetPosition( cameraPivot.GetPosition() );

			counter++;
			if(timer->GetElapsedTimeInSeconds() >= 1.0f) {

				timer->Restart();
				fps = counter;
				counter = 0;
			}


			fpsText->SetText(
				StringBuilder("DIPs: ") << engine->GetRenderer()->GetDIPs() <<
				"\nTC: " << engine->GetRenderer()->GetTextureUsedPerFrame() <<
				"\nFPS: " << fps <<
				"\nSC: " << engine->GetRenderer()->GetShaderUsedPerFrame() <<
				"\nRT: " << engine->GetRenderer()->GetRenderedTriangles()
			);

			fpsText->SetVisible(true);


			perfTimer->Restart();
			engine->GetRenderer()->RenderWorld(); // fixed FPS
			engine->GetRenderer()->UpdateWorld();
			perfTime = perfTimer->GetElapsedTimeInMilliSeconds();
		}
	} catch(std::runtime_error & err) {
		MessageBoxA(0, err.what(), "Error", MB_ICONERROR);
	}
}

#endif