#include "Precompiled.h"
#include "LevelArrival2.h"

LevelArrival::LevelArrival(unique_ptr<Game>& game, const unique_ptr<PlayerTransfer>& playerTransfer) : Level(game, playerTransfer) {
	mName = LevelName::Arrival;

	LoadSceneFromFile("data/maps/arrival.scene");
	LoadLocalization("arrival.loc");

	auto renderer = mGame->GetEngine()->GetRenderer();
	auto soundSystem = mGame->GetEngine()->GetSoundSystem();

	// skybox
	mPlayer->mpCamera->mCamera->SetSkybox(
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonUp2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonRight2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonLeft2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonFront2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonBack2048.png")
	);

	// notes
	{
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note1")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note1Desc"), mLocalization.GetString("note1")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note2")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note2Desc"), mLocalization.GetString("note2")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note3")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note3Desc"), mLocalization.GetString("note3")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note4")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note4Desc"), mLocalization.GetString("note4")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note5")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note5Desc"), mLocalization.GetString("note5")); });
	}

	auto factory = mGame->GetEngine()->GetSceneFactory();

	// add doors with clockwise direction
	auto doors = factory->GetTaggedObjects("door");
	for(auto & d : doors) {
		auto door = MakeDoor(d->GetName(), 90);
		if(d->GetName() == "Door007") {
			mDoor = door;
		}
	}

	// add wooden doors with clockwise direction
	auto wdoors = factory->GetTaggedObjects("woodendoor");
	for(auto & d : wdoors) {
		auto door = MakeDoor(d->GetName(), 90, true, "data/sounds/door/woodendoor_open.ogg", "data/sounds/door/woodendoor_close.ogg");
	}

	// add doors with counterclockwise direction
	auto rdoors = mGame->GetEngine()->GetSceneFactory()->GetTaggedObjects("rdoor");
	for(auto & d : rdoors) {
		auto door = MakeDoor(d->GetName(), 90);
		door->SetTurnDirection(Door::TurnDirection::Counterclockwise);
	}

	// hydraulics 
	{
		mHydraButton = AddButton(make_shared<Button>(GetUniqueObject("GateOpenButton"), "Open gates", [this] {mHydraulicsAnimation.SetEnabled(true); }));
		mHydraButton->SetEnabled(false);

		mPumpStartButton = AddButton(make_shared<Button>(GetUniqueObject("PumpStartButton"), "Enable pump",
			[this] {
			mHydraButton->SetEnabled(true);
			mElectricMotorSound->Play();
		}
		));
		mPumpStartButton->SetEnabled(false);

		mHydraulicsAnimation = Animation(0, 40, 15, false);
		mHydraulicsAnimation.AddFrameListener(0, [this] { mGatesSound->Play(); });
		mHydraulicsAnimation.AddFrameListener(20, [this] { mStages["GatesOpen"] = true; });
		auto hydra = factory->GetTaggedObjects("hydraGates");
		for(auto part : hydra) {
			part->SetAnimation(&mHydraulicsAnimation);
		}

		mGatesSound = mGame->GetEngine()->GetSoundSystem()->LoadSound3D("data/sounds/gates.ogg");
		mGatesSound->Attach(GetUniqueObject("LargeGates"));
		mGatesSound->SetReferenceDistance(15);

		mElectricMotorSound = AddSound(soundSystem->LoadSound3D("data/sounds/electricmotor.ogg"));
		mElectricMotorSound->SetLoop(true);
		mElectricMotorSound->Attach(GetUniqueObject("Pump"));
	}

	// power 
	{
		mPowerSwitchAnimation = Animation(0, 40, 1, false);
		mPowerSwitchAnimation.AddFrameListener(20, [this] { mSwitchSound->Play(); mStages["PowerRestored"] = true; });
		mPowerSwitch = factory->GetTaggedObjects("powerSwitch")[0];
		mPowerSwitch->SetAnimation(&mPowerSwitchAnimation);

		mSwitchSound = AddSound(soundSystem->LoadSound3D("data/sounds/lever.ogg"));
		mSwitchSound->Attach(mPowerSwitch);

		mInsertSound = AddSound(soundSystem->LoadSound3D("data/sounds/insert.ogg"));

		// fuses
		auto fuses = factory->GetTaggedObjects("fuseItem");
		for(auto & f : fuses) {
			AddInteractiveObject("Fuse", make_shared<InteractiveObject>(f), [this] { mPlayer->GetInventory()->AddItem(Item::Type::Fuse); });
		}
	}
	
	// lift
	{
		mLiftFallAnimation = Animation(0, 40, 20, false);
		mLiftFallAnimation.AddFrameListener(27, [this] { 
			for(int i = 0; i < 2; ++i) {
				mSparks[i]->Show();
			}
			mScratchSound->Play();
		});
		mLiftFallAnimation.AddFrameListener(36, [this] { mFallSound->Play(); });

		mLift = GetUniqueObject("Lift1");
		mLift->SetAnimation(&mLiftFallAnimation);
				
		// add go up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoUp"), "Go up", [this] { }));
		// add go down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoDown"), "Go down", [this] { mLiftFallAnimation.SetEnabled(true); mLiftSound->Play(); }));
		// add call up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1CallUp"), "Call up", [this] {}));
		// add call down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1CallDown"), "Call down", [this] {}));

		mLiftSound = AddSound(soundSystem->LoadSound3D("data/sounds/elevator_loop.ogg"));
		mLiftSound->Attach(mLift);
		mLiftSound->SetReferenceDistance(20);
	}

	// sparks in lift
	{
		mSparks[0] = dynamic_pointer_cast<IPointLight>(GetUniqueObject("Sparks1"));
		mSparks[1] = dynamic_pointer_cast<IPointLight>(GetUniqueObject("Sparks2"));

		for(int i = 0; i < 2; ++i) {
			mSparks[i]->Hide();
			mSparks[i]->SetDrawFlare(false);

			mSparksPS[i] = factory->CreateParticleSystem(100);
			mSparksPS[i]->SetTexture(mGame->GetEngine()->GetRenderer()->GetTexture("data/textures/particles/p1.png"));
			mSparksPS[i]->SetType(IParticleSystem::Type::Box);
			mSparksPS[i]->SetSpeedDeviation(Vector3(-0.001, 0.01, -0.001), Vector3(0.001, 0.03, 0.001));
			mSparksPS[i]->SetColorRange(Vector3(255, 114, 0), Vector3(255, 114, 0));
			mSparksPS[i]->SetPointSize(0.045f);
			mSparksPS[i]->SetBoundingBox(Vector3(-0.07, 0.0, -0.07), Vector3(0.07, 0.4, 0.07));
			mSparksPS[i]->SetParticleThickness(20.5f);
			mSparksPS[i]->SetAutoResurrection(true);
			mSparksPS[i]->SetLightingEnabled(true);
			mSparksPS[i]->Attach(mSparks[i]);
		}
	}

	// rusted door and crowbar
	{
		mCrowbarAnimation = Animation(0, 40, 2, false);
		mCrowbarAnimation.AddFrameListener(0, [this] { mRustedDoorSound->Play(); });
		mCrowbarAnimation.AddFrameListener(27, [this] { mCrowbarModel->Hide(); });
		mCrowbarModel = GetUniqueObject("CrowbarModel");
		mCrowbarModel->SetAnimation(&mCrowbarAnimation);

		mRustedDoor = GetUniqueObject("RustedDoor");
		mRustedDoor->SetAnimation(&mCrowbarAnimation);
		mRustedDoorSound = AddSound(soundSystem->LoadSound3D("data/sounds/metal_stress1.ogg"));
		mRustedDoorSound->Attach(mCrowbarModel);

		AddInteractiveObject(Item::GetNameByType(Item::Type::Crowbar), make_shared<InteractiveObject>(GetUniqueObject("Crowbar")), [this] { mPlayer->AddItem(Item::Type::Crowbar); });
	}

	// oil canister
	{
		AddItemPlace(mCanisterPlace = make_shared<ItemPlace>(GetUniqueObject("CanisterPlace"), Item::Type::OilCanister));
		AddInteractiveObject("Oil canister", make_shared<InteractiveObject>(GetUniqueObject("CanisterItem")), [this] { mPlayer->GetInventory()->AddItem(Item::Type::OilCanister); });

		mCanisterAnimation = Animation(0, 40, 3, false);
		mCanisterAnimation.AddFrameListener(0, [this] { mOilSound->Play(); });
		mCanisterAnimation.AddFrameListener(mCanisterAnimation.GetEndFrame() - 1,
			[this] {
			mCanisterModel->Hide();
			mOilSound->Stop();
		});
		mCanisterModel = GetUniqueObject("CanisterModel");
		mCanisterModel->SetAnimation(&mCanisterAnimation);

		mOilSound = AddSound(soundSystem->LoadSound3D("data/sounds/oil.ogg"));
		mOilSound->Attach(mCanisterModel);
	}
		
	
	MakeKeypad("Keypad3", "Keypad3Key0", "Keypad3Key1", "Keypad3Key2", "Keypad3Key3", "Keypad3Key4", "Keypad3Key5", "Keypad3Key6", "Keypad3Key7", "Keypad3Key8", "Keypad3Key9", "Keypad3KeyCancel", mDoor, "5418");

	mLights[0] = GetUniqueObject("HLight1");
	mLights[1] = GetUniqueObject("HLight2");

	for(int i = 0; i < 6; ++i) {
		mFusesModels[i] = GetUniqueObject(StringBuilder("FuseModel") << i + 1);
		AddItemPlace(mFusesPlaces[i] = make_shared<ItemPlace>(GetUniqueObject(StringBuilder("FusePlace") << i + 1), Item::Type::Fuse));
	}
	
	mMusic = AddSound(soundSystem->LoadMusic("data/music/arrival.ogg"));

	// zones
	{
		AddZone(make_shared<Zone>(GetUniqueObject("ObjNeedEnterMine"), [this] { mPlayer->GetHUD()->SetObjective(mLocalization.GetString("ObjNeedEnterMine")); }));
		AddZone(make_shared<Zone>(GetUniqueObject("ObjNeedFigureOutWhyDontOpen"), [this] { mPlayer->GetHUD()->SetObjective(mLocalization.GetString("ObjNeedFigureOutWhyDontOpen")); }));
		AddZone(make_shared<Zone>(GetUniqueObject("ObjNeedFindCode"), [this] { mPlayer->GetHUD()->SetObjective(mLocalization.GetString("ObjNeedFindCode")); }));
		AddZone(make_shared<Zone>(GetUniqueObject("ObjNeedFindCrowbar"), [this] { mPlayer->GetHUD()->SetObjective(mLocalization.GetString("ObjNeedFindCrowbar")); }));
		AddZone(make_shared<Zone>(GetUniqueObject("ObjNeedFindFuses"), [this] { mPlayer->GetHUD()->SetObjective(mLocalization.GetString("ObjNeedFindFuses")); }));
		AddZone(make_shared<Zone>(GetUniqueObject("ObjNeedRefillHydra"), [this] { if(!mStages["HydraulicsRefilled"]) mPlayer->GetHUD()->SetObjective(mLocalization.GetString("ObjNeedRefillHydra"));}));
	}

	mWindSound = AddSound(soundSystem->LoadSound2D("data/sounds/wind.ogg"));
	mWindSound->SetVolume(0.15f);
	mWindSound->Play();
	mWindSound->SetLoop(true);

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());

	mStages["PowerRestored"] = false;
	mStages["HydraulicsRefilled"] = false;
	mStages["GatesOpen"] = false;

	dynamic_pointer_cast<IPointLight>(GetUniqueObject("Omni001"))->SetDrawFlare(false);
	dynamic_pointer_cast<IPointLight>(GetUniqueObject("Omni003"))->SetDrawFlare(false);
	dynamic_pointer_cast<IPointLight>(GetUniqueObject("Omni004"))->SetDrawFlare(false);

	soundSystem->SetReverbPreset(ReverbPreset::Arena);

	mZoneNewLevelLoad = GetUniqueObject("LoadNextLevelZone");

	mPlayer->AddItem(Item::Type::Lighter);

	mScratchSound = AddSound(soundSystem->LoadSound2D("data/sounds/lift_scratch_concrete.ogg"));
	mFallSound = AddSound(soundSystem->LoadSound2D("data/sounds/fall.ogg"));
}

LevelArrival::~LevelArrival() {

}

void LevelArrival::DoScenario() {
	mMusic->Play();

	mHydraulicsAnimation.Update();
	mPowerSwitchAnimation.Update();
	mCanisterAnimation.Update();
	mCrowbarAnimation.Update();
	mLiftFallAnimation.Update();

	mGame->GetEngine()->GetRenderer()->SetAmbientColor(Vector3(0.05, 0.05, 0.05));

	int placedCount = 0;
	for(int i = 0; i < 6; ++i) {
		if(mPlayer->GetInventory()->GetItemSelectedForUse()) {
			if(mPlayer->mNearestPickedNode == mFusesPlaces[i]->mObject) {
				mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->mLocalization.GetString("putItem"));
				if(mGame->GetEngine()->GetInput()->IsKeyHit(mPlayer->mKeyUse)) {
					if(mFusesPlaces[i]->PlaceItem(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType())) {
						mFusesModels[i]->Show();
						mFusesPlaces[i]->mObject->Hide();
						mFusesPlaces[i]->SetPlaceType(Item::Type::Unknown);
						mInsertSound->SetPosition(mFusesPlaces[i]->mObject->GetPosition());
						mInsertSound->Play();
					}
				}
			}
		}

		if(mFusesPlaces[i]->GetPlaceType() == Item::Type::Unknown) {
			placedCount++;
		}
	}

	bool enoughFuses = placedCount >= 6;

	if(!mStages["PowerRestored"]) {
		if(mPlayer->mNearestPickedNode == mPowerSwitch) {
			mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->mLocalization.GetString("putItem"));
			if(mPlayer->IsUseButtonHit() && enoughFuses) {
				mPowerSwitchAnimation.SetEnabled(true);
			}
		}
	}
	
	if(mPlayer->mNearestPickedNode == mCanisterPlace->mObject) {
		if(mPlayer->GetInventory()->GetItemSelectedForUse()) {
			if(!mStages["HydraulicsRefilled"]) {
				if(mPlayer->IsUseButtonHit()) {
					if(mCanisterPlace->PlaceItem(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType())) {
						mCanisterPlace->mObject->Hide();
						mStages["HydraulicsRefilled"] = true;
						mCanisterAnimation.SetEnabled(true);
						mCanisterModel->Show();						
					}
				}
			}
		}
	}

	if(mPlayer->mNearestPickedNode == mRustedDoor) {
		if(mPlayer->GetInventory()->GetItemSelectedForUse()) {
			if(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType() == Item::Type::Crowbar) {
				if(mPlayer->IsUseButtonHit()) {
					mPlayer->GetInventory()->ResetSelectedForUse();
					mPlayer->GetInventory()->RemoveItem(Item::Type::Crowbar, 1);
					mCrowbarAnimation.SetEnabled(true);
					mCrowbarModel->Show();
				}
			}
		}
	}

	if(mStages["HydraulicsRefilled"]) {
		if(mStages["PowerRestored"]) {
			mPumpStartButton->SetEnabled(true);
		}
	}

	if(mStages["GatesOpen"]) {
		mHydraButton->SetEnabled(false);
	}

	if(mStages["PowerRestored"]) {
		mLights[0]->Show();
		mLights[1]->Show();		
	} else {
		mLights[0]->Hide();
		mLights[1]->Hide();
	}

	if(mPlayer->IsInsideZone(mZoneNewLevelLoad)) {
		mGame->LoadLevel(LevelName::Mine);
	}
}

void LevelArrival::Show() {
	Level::Show();
}

void LevelArrival::Hide() {
	Level::Hide();
}

void LevelArrival::OnSerialize(SaveFile & out) {
	out & mPowerSwitchAnimation;
	out & mHydraulicsAnimation;
	out & mCanisterAnimation;
	out & mCrowbarAnimation;
	out & mLiftFallAnimation;
}
