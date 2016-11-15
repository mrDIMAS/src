#include "Precompiled.h"

#include "LevelArrival.h"
#include "Player.h"
#include "LevelMine.h"


LevelArrival::LevelArrival(const unique_ptr<PlayerTransfer> & playerTransfer) : Level(playerTransfer), mChangeLevel(false) {
	mName = LevelName::Arrival;

	mPlayer->mYaw.SetTarget(180.0f);

	// Load localization
	LoadLocalization("arrival.loc");

	// Load main scene
	LoadSceneFromFile("data/maps/arrival.scene");

	//////////////////////////////////////////////////////////////////////////
	// Find and create all sheets
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note1")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note1Desc"), mLocalization.GetString("note1")); });

	//////////////////////////////////////////////////////////////////////////
	// Find zones
	mLiftStopZone = GetUniqueObject("LiftStopZone");

	//////////////////////////////////////////////////////////////////////////
	// Find entities

	mLiftLamp = std::dynamic_pointer_cast<ruLight>(GetUniqueObject("LiftLamp"));
	mLiftLamp->Hide();

	mTutorialZone1 = GetUniqueObject("TutorialZone1");
	mTutorialZone2 = GetUniqueObject("TutorialZone2");
	mTutorialZone3 = GetUniqueObject("TutorialZone3");
	mTutorialZone4 = GetUniqueObject("TutorialZone4");
	mTutorialZone5 = GetUniqueObject("TutorialZone5");
	mTutorialZone6 = GetUniqueObject("TutorialZone6");
	mTutorialZone7 = GetUniqueObject("TutorialZone7");
	mTutorialZone8 = GetUniqueObject("TutorialZone8");

	mHalt = GetUniqueObject("Halt");

	//////////////////////////////////////////////////////////////////////////
	// Player noticements
	mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective1"));

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition() + ruVector3(0, 1, 0));

	//////////////////////////////////////////////////////////////////////////
	// Load sounds

	AddSound(mPowerDownSound = ruSound::Load2D("data/sounds/powerdown.ogg"));
	mPowerDownSound->SetVolume(0.7);

	AddSound(mMetalWhineSound = ruSound::Load2D("data/sounds/metal_whining.ogg"));
	mMetalWhineSound->SetVolume(0.95);

	AddSound(mMetalStressSound = ruSound::Load2D("data/sounds/metal_stress1.ogg"));
	AddSound(mLiftFallSound = ruSound::Load2D("data/sounds/lift_fall.ogg"));
	mLiftFallSound->SetVolume(1.25f);

	AddSound(mWindSound = ruSound::Load2D("data/sounds/wind.ogg"));
	mWindSound->SetVolume(0.5f);
	mWindSound->Play();

	// construct lift
	{
		mLift = AddLift("Lift1", "Lift1Source", "Lift1Dest", "Lift1FrontDoorLeft", "Lift1FrontDoorRight", "Lift1BackDoorLeft", "Lift1BackDoorRight");
		// add go up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoUp"), "Go up", [this] { mLift->GoUp(); }));
		// add go down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoDown"), "Go down", [this] { mLift->GoDown(); }));
		// add call button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoUpFloor1"), "Call", [this] { mLift->GoUp(); }));
	}

	ruSound::SetAudioReverb(15);

	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/forest/forestambient1.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/forest/forestambient2.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/forest/forestambient3.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/forest/forestambient4.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/forest/forestambient5.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/forest/forestambient6.ogg"));

	AddSound(mWoodHitSound = ruSound::Load2D("data/sounds/woodhit.ogg"));

	mPlayer->mpCamera->mCamera->SetSkybox(
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonUp2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonRight2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonLeft2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonFront2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonBack2048.png")
	);

	mStages["LiftCrashed"] = false;

	// add light switches
	vector<shared_ptr<ruLight>> lights;
	lights.push_back(std::dynamic_pointer_cast<ruLight>(GetUniqueObject("ClockHouseLight1")));
	lights.push_back(std::dynamic_pointer_cast<ruLight>(GetUniqueObject("ClockHouseLight2")));
	AddLightSwitch(shared_ptr<LightSwitch>(new LightSwitch(GetUniqueObject("LightSwitch1"), lights, false)));
	
	lights.clear();
	lights.push_back(std::dynamic_pointer_cast<ruLight>(GetUniqueObject("EntranceLight1")));
	lights.push_back(std::dynamic_pointer_cast<ruLight>(GetUniqueObject("EntranceLight2")));
	AddLightSwitch(shared_ptr<LightSwitch>(new LightSwitch(GetUniqueObject("LightSwitch2"), lights, false)));

	lights.clear();
	lights.push_back(std::dynamic_pointer_cast<ruLight>(GetUniqueObject("RoadLight2")));
	AddLightSwitch(shared_ptr<LightSwitch>(new LightSwitch(GetUniqueObject("LightSwitch3"), lights, false)));

	lights.clear();
	lights.push_back(std::dynamic_pointer_cast<ruLight>(GetUniqueObject("MineLight1")));
	lights.push_back(std::dynamic_pointer_cast<ruLight>(GetUniqueObject("MineLight2")));
	AddLightSwitch(shared_ptr<LightSwitch>(new LightSwitch(GetUniqueObject("LightSwitch4"), lights, false)));

	// every action series disabled by default
	mLiftCrashSeries.AddAction(0.0f, [this] { ActLiftCrash_PowerDown(); });
	mLiftCrashSeries.AddAction(mPowerDownSound->GetLength(), [this] { ActLiftCrash_AfterPowerDown(); });
	mLiftCrashSeries.AddAction(mMetalStressSound->GetLength(), [this] { ActLiftCrash_AfterFirstStressSound(); });
	mLiftCrashSeries.AddAction(mLiftFallSound->GetLength(), [this] { ActLiftCrash_AfterFalldown(); });

	AddInteractiveObject(Item::GetNameByType(Item::Type::Crowbar), make_shared<InteractiveObject>(GetUniqueObject("Crowbar")), [this] {mPlayer->AddItem(Item::Type::Crowbar); });

	AddDoor("MetalDoor1", 90.0f);

	mPlayer->GetInventory()->AddItem(Item::Type::Lighter);


	auto fogMesh = GetUniqueObject("Fog");
	mFog = ruFog::Create(fogMesh->GetAABBMin(), fogMesh->GetAABBMax(), ruVector3(0.5, 0.5, 0.7), 0.2);
	mFog->SetPosition(fogMesh->GetPosition());
	mFog->SetSpeed(ruVector3(0.0005, 0, 0.0005));
	mFog->Attach(mScene);

	DoneInitialization();
}

LevelArrival::~LevelArrival() {

}

void LevelArrival::Show() {
	Level::Show();
}

void LevelArrival::Hide() {
	Level::Hide();
}

void LevelArrival::DoScenario() {
	mLiftCrashSeries.Perform();

	mLift->Update();

	// force disable hdr
	//ruEngine::SetHDREnabled(false);

	if (mPlayer->IsInsideZone(mTutorialZone1)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls"));
	}
	if (mPlayer->IsInsideZone(mTutorialZone2)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls2"));
	}
	if (mPlayer->IsInsideZone(mTutorialZone3)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls3"));
	}
	if (mPlayer->IsInsideZone(mTutorialZone4)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls4"));
	}
	if (mPlayer->IsInsideZone(mTutorialZone5)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls5"));
	}
	if (mPlayer->IsInsideZone(mTutorialZone6)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls6"));
	}
	if (mPlayer->IsInsideZone(mTutorialZone7)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls7"));
	}
	if (mPlayer->IsInsideZone(mTutorialZone8)) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("tutorialControls8"));
	}

	auto itemForUse = mPlayer->GetInventory()->GetItemSelectedForUse();
	if (itemForUse) {
		if (ruInput::IsKeyHit(mPlayer->mKeyUse)) {
			if (itemForUse->GetType() == Item::Type::Crowbar) {
				auto planks = ruSceneNode::GetTaggedObjects("WoodenPlank");
				for (auto plank : planks) {
					if (plank->IsFrozen()) {
						if (mPlayer->mNearestPickedNode == plank) {
							plank->Unfreeze();
							mWoodHitSound->SetPosition(plank->GetPosition());
							mWoodHitSound->Play();
						}
					}
				}
			}
		}
	}

	if (!mStages["LiftCrashed"]) {
		ruEngine::SetAmbientColor(ruVector3(0.05, 0.05, 0.05));
		PlayAmbientSounds();
		if (mPlayer->IsInsideZone(mLiftStopZone)) {
			mLift->SetPaused(true);
			mLiftLamp->SetRange(0.01f);
			mStages["LiftCrashed"] = true;
			mLiftCrashSeries.SetEnabled(true);
		}
	} else {
		// fully dark
		mPlayer->TurnOffFakeLight();
		ruEngine::SetAmbientColor(ruVector3(0.0, 0.0, 0.0));
		for (int i = 0; i < ruPointLight::GetCount(); ++i) {
			ruPointLight::Get(i)->Hide();
		}
		for (int i = 0; i < ruSpotLight::GetCount(); ++i) {
			ruSpotLight::Get(i)->Hide();
		}
	}

	if (mPlayer->DistanceTo(mHalt) < 4) {
		mHalt->Unfreeze();
	}

	if (mChangeLevel) {
		Level::Change(LevelName::Mine);
	}
}

void LevelArrival::OnSerialize(SaveFile & s) {
	mLiftCrashSeries.Serialize(s);
}

// ACTIONS
void LevelArrival::ActLiftCrash_PowerDown() {
	mPowerDownSound->Play();
	mWindSound->Pause();
}

void LevelArrival::ActLiftCrash_AfterPowerDown() {
	mMetalStressSound->Play();
	mLift->SetEngineSoundEnabled(false);
	mLift->SetPaused(false);
	mLift->SetSpeedMultiplier(0.24f);
}

void LevelArrival::ActLiftCrash_AfterFirstStressSound() {
	mLiftFallSound->Play();
	mPlayer->LockFlashlight(true);
}

void LevelArrival::ActLiftCrash_AfterFalldown() {
	mChangeLevel = true;
	mPlayer->LockFlashlight(false);
	mPlayer->SetHealth(20);
}
