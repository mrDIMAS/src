#include "Precompiled.h"

#include "LevelResearchFacility.h"
#include "Player.h"
#include "Door.h"
#include "Utils.h"

LevelResearchFacility::LevelResearchFacility(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer) :
	Level(game, playerTransfer) {
	mpPowerSparks = nullptr;

	mSteamDisabled = false;

	mName = LevelName::ResearchFacility;

	LoadSceneFromFile("data/maps/researchfacility.scene");
	LoadLocalization("rf.loc");

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());
	
	auto soundSystem = mGame->GetEngine()->GetSoundSystem();

	AddSound(mSteamHissSound = soundSystem->LoadSound3D("data/sounds/steamhiss.ogg"));
	mSteamHissSound->SetRolloffFactor(5);
	mSteamHissSound->SetReferenceDistance(4);
	mSteamHissSound->SetRoomRolloffFactor(2.5f);

	// construct first lift
	{
		mLift1 = AddLift("Lift1", "Lift1Source", "Lift1Dest", "Lift1FrontDoor1", "Lift1FrontDoor2", "Lift1BackDoor1", "Lift1BackDoor2");
		// add go up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoUp"), "Go up", [this] { mLift1->GoUp(); }));
		// add go down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoDown"), "Go down", [this] { mLift1->GoDown(); }));
		// add call up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1CallUp"), "Call up", [this] { mLift1->GoUp(); }));
		// add call down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1CallDown"), "Call down", [this] { mLift1->GoDown(); }));
	}

	// construct second lift
	{
		mLift2 = AddLift("Lift2", "Lift2Source", "Lift2Dest", "Lift2FrontDoor1", "Lift2FrontDoor2", "Lift2BackDoor1", "Lift2BackDoor2");
		// add go up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift2GoUp"), "Go up", [this] { mLift2->GoUp(); }));
		// add go down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift2GoDown"), "Go down", [this] { mLift2->GoDown(); }));
		// add call up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift2CallUp"), "Call up", [this] { mLift2->GoUp(); }));
		// add call down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift2CallDown"), "Call down", [this] { mLift2->GoDown(); }));
	}

	mpFan1 = make_shared<Ventilator>(GetUniqueObject("Fan"), 15, Vector3(0, 1, 0), soundSystem->LoadSound3D("data/sounds/fan.ogg"));
	mpFan2 = make_shared<Ventilator>(GetUniqueObject("Fan2"), 15, Vector3(0, 1, 0), soundSystem->LoadSound3D("data/sounds/fan.ogg"));

	// create notes
	{
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note1")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note1Desc"), mLocalization.GetString("note1")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note2")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note2Desc"), mLocalization.GetString("note2")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note3")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note3Desc"), mLocalization.GetString("note3")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note4")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note4Desc"), mLocalization.GetString("note4")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note5")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note5Desc"), mLocalization.GetString("note5")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note6")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note6Desc"), mLocalization.GetString("note6")); });
		AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note7")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note7Desc"), mLocalization.GetString("note7")); });
	}

	AddSound(mLeverSound = soundSystem->LoadSound3D("data/sounds/lever.ogg"));

	AddValve(mpSteamValve = make_shared<Valve>(GetUniqueObject("SteamValve"), Vector3(0, 1, 0)));
	shared_ptr<ISound> steamHis = soundSystem->LoadSound3D("data/sounds/steamhiss_loop.ogg");
	steamHis->SetRolloffFactor(5);
	steamHis->SetReferenceDistance(4);
	steamHis->SetRoomRolloffFactor(2.5f);
	AddSound(steamHis);
	mpExtemeSteam = make_unique<SteamStream>(GetUniqueObject("ExtremeSteam"), Vector3(-0.0015, -0.1, -0.0015), Vector3(0.0015, -0.45, 0.0015), steamHis);

	mGame->GetEngine()->GetRenderer()->SetAmbientColor(Vector3(0, 0, 0));

	mDoorUnderFloor = GetUniqueObject("DoorUnderFloor");

	mExtremeSteamBlock = GetUniqueObject("ExtremeSteamBlock");
	mZoneExtremeSteamHurt = GetUniqueObject("ExtremeSteamHurtZone");

	mThermiteSmall = GetUniqueObject("ThermiteSmall");
	mThermiteBig = GetUniqueObject("ThermiteBig");

	mRadioHurtZone = GetUniqueObject("RadioHurtZone");

	mMeshLock = GetUniqueObject("MeshLock");
	mMeshLockAnimation = Animation(0, 30, 2);
	mMeshLock->SetAnimation(&mMeshLockAnimation);

	mThermitePlace = GetUniqueObject("ThermitePlace");

	mMeshToSewers = GetUniqueObject("MeshToSewers");
	mMeshAnimation = Animation(0, 30, 2);
	mMeshToSewers->SetAnimation(&mMeshAnimation);

	// create zones	
	AddZone(make_shared<Zone>(GetUniqueObject("ObjectiveNeedPassThroughMesh"), [this] { OnPlayerEnterNeedPassThroughMeshZone(); }));
	AddZone(make_shared<Zone>(GetUniqueObject("ZoneEnemySpawn"), [this] { OnPlayerEnterSpawnEnemyZone(); }));
	AddZone(make_shared<Zone>(GetUniqueObject("SteamActivateZone"), [this] { OnPlayerEnterSteamActivateZone(); }));
	AddZone(make_shared<Zone>(GetUniqueObject("ObjectiveRestorePower"), [this] { OnPlayerEnterRestorePowerZone(); }));
	AddZone(make_shared<Zone>(GetUniqueObject("ObjectiveExaminePlace"), [this] { OnPlayerEnterExaminePlaceZone(); }));
	AddZone(make_shared<Zone>(GetUniqueObject("ZoneRemovePathBlockingMesh"), [this] { OnPlayerEnterRemovePathBlockingMeshZone(); }));
	AddZone(make_shared<Zone>(GetUniqueObject("ObjectiveNeedCrowbar"), [this] { OnPlayerEnterNeedCrowbarZone(); }));
	AddZone(make_shared<Zone>(GetUniqueObject("DisableSteamZone"), [this] { OnPlayerEnterDisableSteamZone(); }));

	CreatePowerUpSequence();

	// create music
	{
		AddSound(mMusic = soundSystem->LoadMusic("data/music/rf.ogg"));
		mMusic->SetVolume(0.75f);
	}

	// create ladders
	{
		AddLadder("LadderBegin", "LadderEnd", "LadderEnter", "LadderBeginLeavePoint", "LadderEndLeavePoint");
		AddLadder("Ladder3Begin", "Ladder3End", "Ladder3Enter", "Ladder3BeginLeavePoint", "Ladder3EndLeavePoint");
	}

	// create doors 
	{
		AddDoor("Door9", 90.0f);
		AddDoor("Door10", 90.0f);
		AddDoor("Door11", 90.0f);
		AddDoor("Door12", 90.0f);
		AddDoor("Door13", 90.0f);
		AddDoor("Door14", 90.0f);
		AddDoor("Door15", 90.0f);
		AddDoor("Door16", 90.0f);
		AddDoor("Door17", 90.0f);
		AddDoor("Door18", 90.0f);
		AddDoor("Door19", 90.0f);
		mKeypad3DoorToUnlock = AddDoor("Door4", 90.0f);
		mKeypad1DoorToUnlock = AddDoor("Door5", 90.0f);
		mKeypad2DoorToUnlock = AddDoor("Door8", 90.0f);
		mLabDoorToUnlock = AddDoor("LabDoor", 90);
		mColliderDoorToUnlock = AddDoor("DoorToCollider", 90);
		mLockedDoor = AddDoor("LockedDoor", 90);
		mLockedDoor->SetLocked(true);
	}

	mThermiteItemPlace = make_unique<ItemPlace>(mThermitePlace, Item::Type::AluminumPowder);

	AutoCreateDoorsByNamePattern("Door?([[:digit:]]+)");

	mPowerLamp = std::dynamic_pointer_cast<IPointLight>(GetUniqueObject("PowerLamp"));
	mPowerLeverSnd = GetUniqueObject("PowerLeverSnd");
	mSmallSteamPosition = GetUniqueObject("RFSteamPos");
	mZoneNewLevelLoad = GetUniqueObject("NewLevelLoadZone");

	mStages["EnterSteamActivateZone"] = false;
	mStages["EnterObjectiveRestorePowerZone"] = false;
	mStages["EnterObjectiveExaminePlace"] = false;
	mStages["EnterObjectiveNeedCrowbar"] = false;
	mStages["EnterObjectiveNeedOpenDoorOntoFloor"] = false;
	mStages["DoorUnderFloorOpen"] = false;
	mStages["NeedPassThroughMesh"] = false;
	mStages["PassedThroughBlockingMesh"] = false;
	mStages["EnemySpawned"] = false;
	mStages["EnterDisableSteamZone"] = false;

	AddInteractiveObject(Item::GetNameByType(Item::Type::Crowbar), make_shared<InteractiveObject>(GetUniqueObject("Crowbar")), [this] { mPlayer->AddItem(Item::Type::Crowbar); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::FerrumOxide), make_shared<InteractiveObject>(GetUniqueObject("FerrumOxide")), [this] { mPlayer->AddItem(Item::Type::FerrumOxide); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::AluminumPowder), make_shared<InteractiveObject>(GetUniqueObject("AluminumPowder")), [this] { mPlayer->AddItem(Item::Type::AluminumPowder); });

	mKeypad1 = AddKeypad("Keypad1", "Keypad1Key0", "Keypad1Key1", "Keypad1Key2", "Keypad1Key3", "Keypad1Key4", "Keypad1Key5", "Keypad1Key6", "Keypad1Key7", "Keypad1Key8", "Keypad1Key9", "Keypad1KeyCancel", mKeypad1DoorToUnlock, "3065");
	mKeypad2 = AddKeypad("Keypad2", "Keypad2Key0", "Keypad2Key1", "Keypad2Key2", "Keypad2Key3", "Keypad2Key4", "Keypad2Key5", "Keypad2Key6", "Keypad2Key7", "Keypad2Key8", "Keypad2Key9", "Keypad2KeyCancel", mKeypad2DoorToUnlock, "6497");
	mKeypad3 = AddKeypad("Keypad3", "Keypad3Key0", "Keypad3Key1", "Keypad3Key2", "Keypad3Key3", "Keypad3Key4", "Keypad3Key5", "Keypad3Key6", "Keypad3Key7", "Keypad3Key8", "Keypad3Key9", "Keypad3KeyCancel", mKeypad3DoorToUnlock, "1487");
	mLabKeypad = AddKeypad("Keypad4", "Keypad4Key0", "Keypad4Key1", "Keypad4Key2", "Keypad4Key3", "Keypad4Key4", "Keypad4Key5", "Keypad4Key6", "Keypad4Key7", "Keypad4Key8", "Keypad4Key9", "Keypad4KeyCancel", mLabDoorToUnlock, "8279");
	mColliderKeypad = AddKeypad("Keypad5", "Keypad5Key0", "Keypad5Key1", "Keypad5Key2", "Keypad5Key3", "Keypad5Key4", "Keypad5Key5", "Keypad5Key6", "Keypad5Key7", "Keypad5Key8", "Keypad5Key9", "Keypad5KeyCancel", mColliderDoorToUnlock, "1598");

	soundSystem->SetReverbPreset(ReverbPreset::Stonecorridor);

	mEnemySpawnPosition = GetUniqueObject("EnemyPosition");

	mSteamPS = nullptr;

	auto fogMesh = GetUniqueObject("Fog");
	mFog = mGame->GetEngine()->GetSceneFactory()->CreateFog(fogMesh->GetAABBMin(), fogMesh->GetAABBMax(), Vector3(0.0, 0.5, 1), 0.6);
	mFog->SetPosition(fogMesh->GetPosition());
	mFog->SetSpeed(Vector3(0.0002, 0, 0.0002));
	mFog->Attach(mScene);

	dynamic_pointer_cast<IPointLight>(GetUniqueObject("RadioLight1"))->SetDrawFlare(false);
	dynamic_pointer_cast<IPointLight>(GetUniqueObject("RadioLight2"))->SetDrawFlare(false);

	DoneInitialization();
}

void LevelResearchFacility::CreateEnemy() {
	const char * ways[] = {
		"WayA", "WayB", "WayC", "WayD", "WayE", "WayF", "WayG",
		"WayH", "WayI", "WayJ", "WayK"
	};
	Path p;
	for(auto w : ways) {
		p += Path(mScene, w);
	}

	p.Get("WayA028")->AddEdge(p.Get("WayB1"));
	p.Get("WayB024")->AddEdge(p.Get("WayC1"));
	p.Get("WayB021")->AddEdge(p.Get("WayD1"));
	p.Get("WayA020")->AddEdge(p.Get("WayE1"));
	p.Get("WayA020")->AddEdge(p.Get("WayF1"));
	p.Get("WayA013")->AddEdge(p.Get("WayG1"));
	p.Get("WayA013")->AddEdge(p.Get("WayH1"));
	p.Get("WayH005")->AddEdge(p.Get("WayI1"));
	p.Get("WayA033")->AddEdge(p.Get("WayJ1"));
	p.Get("WayA037")->AddEdge(p.Get("WayK1"));

	vector<shared_ptr<GraphVertex>> patrolPoints = {
		p.Get("WayD009"), p.Get("WayC009"), p.Get("WayB024"),
		p.Get("WayB024"), p.Get("WayA072"), p.Get("WayA1"),
		p.Get("WayJ007"), p.Get("WayK010"), p.Get("WayE006"),
		p.Get("WayF005"), p.Get("WayG007"), p.Get("WayI004"),
		p.Get("WayH018")
	};

	mEnemy = make_unique<Enemy>(mGame, p.mVertexList, patrolPoints);
	mEnemy->SetPosition(mEnemySpawnPosition->GetPosition());
}

LevelResearchFacility::~LevelResearchFacility() {

}

void LevelResearchFacility::Show() {
	Level::Show();

	mMusic->Play();
}

void LevelResearchFacility::Hide() {
	Level::Hide();

	mMusic->Pause();
}

void LevelResearchFacility::DoScenario() {
	mMusic->Play();

	mMeshAnimation.Update();
	mMeshLockAnimation.Update();

	mGame->GetEngine()->GetRenderer()->SetAmbientColor(Vector3(0.01, 0.01, 0.01));

	if(mPowerOn) {
		mpFan1->DoTurn();
		mpFan2->DoTurn();
		mLift1->SetLocked(false);
	} else {
		mLift1->SetLocked(true);
	}

	mLift1->Update();
	mLift2->Update();

	if(mEnemy) {
		mEnemy->Think();
	}

	if(!mStages["DoorUnderFloorOpen"]) {
		if(mPlayer->mNearestPickedNode) {
			if(mPlayer->mNearestPickedNode == mDoorUnderFloor) {
				if(mPlayer->GetInventory()->GetItemSelectedForUse()) {
					if(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType() == Item::Type::Crowbar) {
						mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->GetLocalization()->GetString("openDoor"));
						if(mGame->GetEngine()->GetInput()->IsKeyHit(mPlayer->mKeyUse)) {
							mPlayer->GetInventory()->ResetSelectedForUse();
							mDoorUnderFloor->SetRotation(Quaternion(0, 0, -110));
							mStages["DoorUnderFloorOpen"] = true;
						}
					}
				}
			}
		}
	}

	if(mPlayer->IsInsideZone(mRadioHurtZone)) {
		mPlayer->Damage(0.05, false);
		mPlayer->GetHUD()->SetAction(IInput::Key::None, mLocalization.GetString("radioactive"));
	}

	mpSteamValve->Update();
	mpExtemeSteam->Update();
	mpExtemeSteam->SetPower(1.0f - mpSteamValve->GetClosedCoeffecient());

	UpdatePowerupSequence();
	UpdateThermiteSequence();

	if(mPowerOn && mpPowerSparks) {
		mpPowerSparks->Update();

		if(!mpPowerSparks->IsAlive()) {
			mpPowerSparks.reset();
		}
	}

	if(mSteamHissSound->IsPlaying() && mSteamPS) {
		static float steamParticleSize = 0.15f;

		mSteamPS->SetPointSize(steamParticleSize);

		if(steamParticleSize > 0) {
			steamParticleSize -= 0.0005f;
		} else {
			mSteamPS.reset();
		}
	}

	if(mSteamDisabled) {
		mExtremeSteamBlock->SetPosition(Vector3(1000, 1000, 1000));
		mSteamPS.reset();
	} else {
		if(mPlayer->IsInsideZone(mZoneExtremeSteamHurt)) {
			mPlayer->Damage(0.6);
		}
	}

	if(mpSteamValve->IsDone()) {
		mSteamDisabled = true;
	}

	if(mPlayer->IsInsideZone(mZoneNewLevelLoad)) {
		mGame->LoadLevel(LevelName::Sewers);
	}
}

void LevelResearchFacility::UpdateThermiteSequence() {
	if(mPlayer->GetInventory()->GetItemSelectedForUse()) {
		if(mThermiteItemPlace->IsPickedByPlayer()) {
			if(mGame->GetEngine()->GetInput()->IsKeyHit(mPlayer->mKeyUse)) {
				bool placed = mThermiteItemPlace->PlaceItem(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType());
				if(placed) {
					if(mThermiteItemPlace->GetPlaceType() == Item::Type::AluminumPowder) {
						mThermiteSmall->Show();
						mThermiteItemPlace->SetPlaceType(Item::Type::FerrumOxide);
					} else if(mThermiteItemPlace->GetPlaceType() == Item::Type::FerrumOxide) {
						mThermiteBig->Show();
						mThermiteItemPlace->SetPlaceType(Item::Type::Lighter);
					} else if(mThermiteItemPlace->GetPlaceType() == Item::Type::Lighter) {
						mMeshLockAnimation.SetEnabled(true);
						mMeshAnimation.SetEnabled(true);

						mThermiteSmall->Hide();
						mThermiteBig->Hide();

						mBurnSound = mGame->GetEngine()->GetSoundSystem()->LoadSound3D("data/sounds/burn.ogg");
						mBurnSound->SetPosition(mThermiteSmall->GetPosition());
						mBurnSound->Play();

						mThermiteItemPlace->SetPlaceType(Item::Type::Unknown);

						mThermitePS = mGame->GetEngine()->GetSceneFactory()->CreateParticleSystem(150);
						mThermitePS->SetPosition(mThermiteSmall->GetPosition());
						mThermitePS->SetTexture(mGame->GetEngine()->GetRenderer()->GetTexture("data/textures/particles/p1.png"));
						mThermitePS->SetType(IParticleSystem::Type::Box);
						mThermitePS->SetSpeedDeviation(Vector3(-0.001, 0.001, -0.001), Vector3(0.001, 0.009, 0.001));
						mThermitePS->SetColorRange(Vector3(255, 255, 255), Vector3(255, 255, 255));
						mThermitePS->SetPointSize(0.045f);
						mThermitePS->SetBoundingBox(Vector3(-0.2, 0.0, -0.2), Vector3(0.2, 0.4, 0.2));
						mThermitePS->SetParticleThickness(20.5f);
						mThermitePS->SetAutoResurrection(false);
						mThermitePS->SetLightingEnabled(true);
					}
				}
			}
			mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->GetLocalization()->GetString("placeReactive"));
		}
	}
}

void LevelResearchFacility::UpdatePowerupSequence() {
	if(fuseInsertedCount < 3) {
		fuseInsertedCount = 0;

		for(int iFuse = 0; iFuse < 3; iFuse++) {
			shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFuse];
			if(pFuse->GetPlaceType() == Item::Type::Unknown) {
				fuseInsertedCount++;
			}
		}
	}

	if(mPlayer->GetInventory()->GetItemSelectedForUse()) {
		for(int iFuse = 0; iFuse < 3; iFuse++) {
			shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFuse];
			if(pFuse->IsPickedByPlayer()) {
				mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->GetLocalization()->GetString("insertFuse"));
			}
		}

		if(mGame->GetEngine()->GetInput()->IsKeyHit(mPlayer->mKeyUse)) {
			for(int iFusePlace = 0; iFusePlace < 3; iFusePlace++) {
				shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFusePlace];

				if(pFuse->IsPickedByPlayer()) {
					bool placed = pFuse->PlaceItem(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType());

					if(placed) {
						fuseModel[iFusePlace]->Show();
						pFuse->SetPlaceType(Item::Type::Unknown);
					}
				}
			}
		}
	}

	if(fuseInsertedCount >= 3) {
		if(mPlayer->mNearestPickedNode == powerLever) {
			mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->mLocalization.GetString("powerUp"));

			if(mGame->GetEngine()->GetInput()->IsKeyHit(mPlayer->mKeyUse) && !mPowerOn) {


				mPowerLamp->SetColor(Vector3(0, 255, 0));

				mLeverSound->Play();

				mpPowerSparks = make_unique<Sparks>(mPowerLeverSnd, mGame->GetEngine()->GetSoundSystem()->LoadSound3D("data/sounds/sparks.ogg"));

				mPowerLeverOnModel->Show();
				mPowerLeverOffModel->Hide();

				mPowerOn = true;

				mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveTryToFindExit"));
			}
		}
	}
}

void LevelResearchFacility::CreatePowerUpSequence() {
	AddInteractiveObject(Item::GetNameByType(Item::Type::Fuse), make_shared<InteractiveObject>(GetUniqueObject("Fuse1")), [this] {mPlayer->AddItem(Item::Type::Fuse); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Fuse), make_shared<InteractiveObject>(GetUniqueObject("Fuse2")), [this] {mPlayer->AddItem(Item::Type::Fuse); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Fuse), make_shared<InteractiveObject>(GetUniqueObject("Fuse3")), [this] {mPlayer->AddItem(Item::Type::Fuse); });

	AddItemPlace(mFusePlaceList[0] = make_shared<ItemPlace>(GetUniqueObject("FusePlace1"), Item::Type::Fuse));
	AddItemPlace(mFusePlaceList[1] = make_shared<ItemPlace>(GetUniqueObject("FusePlace2"), Item::Type::Fuse));
	AddItemPlace(mFusePlaceList[2] = make_shared<ItemPlace>(GetUniqueObject("FusePlace3"), Item::Type::Fuse));

	fuseModel[0] = GetUniqueObject("FuseModel1");
	fuseModel[1] = GetUniqueObject("FuseModel2");
	fuseModel[2] = GetUniqueObject("FuseModel3");

	mPowerLeverOnModel = GetUniqueObject("PowerSwitchOnModel");
	mPowerLeverOffModel = GetUniqueObject("PowerSwitchOffModel");
	powerLever = GetUniqueObject("PowerLever");

	fuseInsertedCount = 0;

	mPowerOn = false;
}

void LevelResearchFacility::OnSerialize(SaveFile & s) {
	auto enemyPresented = mEnemy != nullptr;
	s & enemyPresented;

	if(enemyPresented) {
		if(s.IsLoading()) {
			CreateEnemy();
		}
		auto epos = mEnemy->GetBody()->GetPosition();
		s & epos;
		mEnemy->GetBody()->SetPosition(epos);
	}
	s & mMeshAnimation;
	s & mMeshLockAnimation;
	s & mSteamDisabled;
}

void LevelResearchFacility::OnCrowbarPickup() {
	if(!mStages["EnterObjectiveNeedOpenDoorOntoFloor"]) {
		mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveNeedOpenDoorOntoFloor"));
		mStages["EnterObjectiveNeedOpenDoorOntoFloor"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterNeedCrowbarZone() {
	if(!mStages["EnterObjectiveNeedCrowbar"]) {
		mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveNeedCrowbar"));
		mStages["EnterObjectiveNeedCrowbar"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterRemovePathBlockingMeshZone() {
	if(!mStages["PassedThroughBlockingMesh"]) {
		mLockedDoor->SetLocked(false);
		mStages["PassedThroughBlockingMesh"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterExaminePlaceZone() {
	if(!mStages["EnterObjectiveExaminePlace"]) {
		mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveExaminePlace"));
		mStages["EnterObjectiveExaminePlace"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterRestorePowerZone() {
	if(!mStages["EnterObjectiveRestorePowerZone"]) {
		mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveRestorePower"));
		mStages["EnterObjectiveRestorePowerZone"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterSteamActivateZone() {
	if(!mStages["EnterSteamActivateZone"]) {
		mSteamPS = mGame->GetEngine()->GetSceneFactory()->CreateParticleSystem(35);
		mSteamPS->SetPosition(mSmallSteamPosition->GetPosition());
		mSteamPS->SetTexture(mGame->GetEngine()->GetRenderer()->GetTexture("data/textures/particles/p1.png"));
		mSteamPS->SetType(IParticleSystem::Type::Stream);
		mSteamPS->SetSpeedDeviation(Vector3(-0.0015, 0.08, -0.0015), Vector3(0.0015, 0.2, 0.0015));
		mSteamPS->SetBoundingRadius(0.4f);
		mSteamPS->SetColorRange(Vector3(255, 255, 255), Vector3(255, 255, 255));
		mSteamPS->SetPointSize(0.15f);
		mSteamPS->SetParticleThickness(1.5f);
		mSteamPS->SetLightingEnabled(true);

		mSteamHissSound->Attach(mSteamPS);
		mSteamHissSound->Play();
		mStages["EnterSteamActivateZone"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterSpawnEnemyZone() {
	if(!mStages["EnemySpawned"]) {
		CreateEnemy();
		mStages["EnemySpawned"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterNeedPassThroughMeshZone() {

}
