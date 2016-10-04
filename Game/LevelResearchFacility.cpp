#include "Precompiled.h"

#include "LevelResearchFacility.h"
#include "Player.h"
#include "Door.h"
#include "Utils.h"

LevelResearchFacility::LevelResearchFacility(const unique_ptr<PlayerTransfer> & playerTransfer) : Level(playerTransfer) {
	mpPowerSparks = nullptr;

	mSteamDisabled = false;

	mTypeNum = 4;

	LoadSceneFromFile("data/maps/researchfacility.scene");
	LoadLocalization("rf.loc");

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());

	AddSound(mSteamHissSound = ruSound::Load3D("data/sounds/steamhiss.ogg"));
	mSteamHissSound->SetRolloffFactor(5);
	mSteamHissSound->SetReferenceDistance(4);
	mSteamHissSound->SetRoomRolloffFactor(2.5f);

	mLift1 = AddLift("Lift1", "Lift1Screen", "Lift1Source", "Lift1Dest", "Lift1FrontDoor1", "Lift1FrontDoor2", "Lift1BackDoor1", "Lift1BackDoor2");
	mLift2 = AddLift("Lift2", "Lift2Screen", "Lift2Source", "Lift2Dest", "Lift2FrontDoor1", "Lift2FrontDoor2", "Lift2BackDoor1", "Lift2BackDoor2");

	mpFan1 = make_shared<Ventilator>(GetUniqueObject("Fan"), 15, ruVector3(0, 1, 0), ruSound::Load3D("data/sounds/fan.ogg"));
	mpFan2 = make_shared<Ventilator>(GetUniqueObject("Fan2"), 15, ruVector3(0, 1, 0), ruSound::Load3D("data/sounds/fan.ogg"));

	AddSheet("Note1", mLocalization.GetString("note1Desc"), mLocalization.GetString("note1"));
	AddSheet("Note2", mLocalization.GetString("note2Desc"), mLocalization.GetString("note2"));
	AddSheet("Note3", mLocalization.GetString("note3Desc"), mLocalization.GetString("note3"));
	AddSheet("Note4", mLocalization.GetString("note4Desc"), mLocalization.GetString("note4"));
	AddSheet("Note5", mLocalization.GetString("note5Desc"), mLocalization.GetString("note5"));
	AddSheet("Note6", mLocalization.GetString("note6Desc"), mLocalization.GetString("note6"));
	AddSheet("Note7", mLocalization.GetString("note7Desc"), mLocalization.GetString("note7"));

	AddSound(mLeverSound = ruSound::Load3D("data/sounds/lever.ogg"));

	AddValve(mpSteamValve = make_shared<Valve>(GetUniqueObject("SteamValve"), ruVector3(0, 1, 0)));
	shared_ptr<ruSound> steamHis = ruSound::Load3D("data/sounds/steamhiss_loop.ogg");
	steamHis->SetRolloffFactor(5);
	steamHis->SetReferenceDistance(4);
	steamHis->SetRoomRolloffFactor(2.5f);
	AddSound(steamHis);
	mpExtemeSteam = unique_ptr<SteamStream>(new SteamStream(GetUniqueObject("ExtremeSteam"), ruVector3(-0.0015, -0.1, -0.0015), ruVector3(0.0015, -0.45, 0.0015), steamHis));

	ruEngine::SetAmbientColor(ruVector3(0, 0, 0));

	mDoorUnderFloor = GetUniqueObject("DoorUnderFloor");

	mExtremeSteamBlock = GetUniqueObject("ExtremeSteamBlock");
	mZoneExtremeSteamHurt = GetUniqueObject("ExtremeSteamHurtZone");

	mThermiteSmall = GetUniqueObject("ThermiteSmall");
	mThermiteBig = GetUniqueObject("ThermiteBig");

	mRadioHurtZone = GetUniqueObject("RadioHurtZone");

	mMeshLock = GetUniqueObject("MeshLock");
	mMeshLockAnimation = ruAnimation(0, 30, 2);
	mMeshLock->SetAnimation(&mMeshLockAnimation);

	mThermitePlace = GetUniqueObject("ThermitePlace");

	mMeshToSewers = GetUniqueObject("MeshToSewers");
	mMeshAnimation = ruAnimation(0, 30, 2);
	mMeshToSewers->SetAnimation(&mMeshAnimation);

	AddZone(mZoneObjectiveNeedPassThroughMesh = make_shared<Zone>(GetUniqueObject("ObjectiveNeedPassThroughMesh")));
	mZoneObjectiveNeedPassThroughMesh->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelResearchFacility::OnPlayerEnterNeedPassThroughMeshZone));

	AddZone(mZoneEnemySpawn = make_shared<Zone>(GetUniqueObject("ZoneEnemySpawn")));
	mZoneEnemySpawn->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelResearchFacility::OnPlayerEnterSpawnEnemyZone));

	AddZone(mZoneSteamActivate = make_shared<Zone>(GetUniqueObject("SteamActivateZone")));
	mZoneSteamActivate->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelResearchFacility::OnPlayerEnterSteamActivateZone));

	AddZone(mZoneObjectiveRestorePower = make_shared<Zone>(GetUniqueObject("ObjectiveRestorePower")));
	mZoneObjectiveRestorePower->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelResearchFacility::OnPlayerEnterRestorePowerZone));

	AddZone(mZoneExaminePlace = make_shared<Zone>(GetUniqueObject("ObjectiveExaminePlace")));
	mZoneExaminePlace->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelResearchFacility::OnPlayerEnterExaminePlaceZone));

	AddZone(mZoneRemovePathBlockingMesh = make_shared<Zone>(GetUniqueObject("ZoneRemovePathBlockingMesh")));
	mZoneRemovePathBlockingMesh->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelResearchFacility::OnPlayerEnterRemovePathBlockingMeshZone));

	AddZone(mZoneNeedCrowbar = make_shared<Zone>(GetUniqueObject("ObjectiveNeedCrowbar")));
	mZoneNeedCrowbar->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelResearchFacility::OnPlayerEnterNeedCrowbarZone));

	CreatePowerUpSequence();

	AddSound(mMusic = ruSound::LoadMusic("data/music/rf.ogg"));
	mMusic->SetVolume(0.75f);

	AddLadder("LadderBegin", "LadderEnd", "LadderEnter", "LadderBeginLeavePoint", "LadderEndLeavePoint");
	AddLadder("Ladder3Begin", "Ladder3End", "Ladder3Enter", "Ladder3BeginLeavePoint", "Ladder3EndLeavePoint");
	AddLadder("Ladder4Begin", "Ladder4End", "Ladder4Enter", "Ladder4BeginLeavePoint", "Ladder4EndLeavePoint");
	AddLadder("Ladder5Begin", "Ladder5End", "Ladder5Enter", "Ladder5BeginLeavePoint", "Ladder5EndLeavePoint");
	AddLadder("Ladder7Begin", "Ladder7End", "Ladder7Enter", "Ladder7BeginLeavePoint", "Ladder7EndLeavePoint");
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
	AddDoor("Door20", 90.0f);
	AddDoor("Door21", 90.0f);
	AddDoor("EasterEggDoor", 90.0f);
	mKeypad3DoorToUnlock = AddDoor("Door4", 90.0f);
	mKeypad1DoorToUnlock = AddDoor("Door5", 90.0f);
	mKeypad2DoorToUnlock = AddDoor("Door8", 90.0f);
	mLabDoorToUnlock = AddDoor("LabDoor", 90);
	mColliderDoorToUnlock = AddDoor("DoorToCollider", 90);

	mLockedDoor = AddDoor("LockedDoor", 90);
	mLockedDoor->SetLocked(true);

	mThermiteItemPlace = new ItemPlace(mThermitePlace, Item::Type::AluminumPowder);

	AutoCreateDoorsByNamePattern("Door?([[:digit:]]+)");

	mPowerLamp = std::dynamic_pointer_cast<ruPointLight>(GetUniqueObject("PowerLamp"));
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

	AutoCreateBulletsByNamePattern("Bullet?([[:digit:]]+)");

	AddInteractiveObject(Item::GetNameByType(Item::Type::Crowbar), make_shared<InteractiveObject>(GetUniqueObject("Crowbar")), ruDelegate::Bind(this, &LevelResearchFacility::Proxy_GiveCrowbar));

	AddInteractiveObject(Item::GetNameByType(Item::Type::FerrumOxide), make_shared<InteractiveObject>(GetUniqueObject("FerrumOxide")), ruDelegate::Bind(this, &LevelResearchFacility::Proxy_GiveFe2O3));
	AddInteractiveObject(Item::GetNameByType(Item::Type::AluminumPowder), make_shared<InteractiveObject>(GetUniqueObject("AluminumPowder")), ruDelegate::Bind(this, &LevelResearchFacility::Proxy_GiveAl));

	mKeypad1 = AddKeypad("Keypad1", "Keypad1Key0", "Keypad1Key1", "Keypad1Key2", "Keypad1Key3", "Keypad1Key4", "Keypad1Key5", "Keypad1Key6", "Keypad1Key7", "Keypad1Key8", "Keypad1Key9", "Keypad1KeyCancel", mKeypad1DoorToUnlock, "3065");
	mKeypad2 = AddKeypad("Keypad2", "Keypad2Key0", "Keypad2Key1", "Keypad2Key2", "Keypad2Key3", "Keypad2Key4", "Keypad2Key5", "Keypad2Key6", "Keypad2Key7", "Keypad2Key8", "Keypad2Key9", "Keypad2KeyCancel", mKeypad2DoorToUnlock, "6497");
	mKeypad3 = AddKeypad("Keypad3", "Keypad3Key0", "Keypad3Key1", "Keypad3Key2", "Keypad3Key3", "Keypad3Key4", "Keypad3Key5", "Keypad3Key6", "Keypad3Key7", "Keypad3Key8", "Keypad3Key9", "Keypad3KeyCancel", mKeypad3DoorToUnlock, "1487");
	mLabKeypad = AddKeypad("Keypad4", "Keypad4Key0", "Keypad4Key1", "Keypad4Key2", "Keypad4Key3", "Keypad4Key4", "Keypad4Key5", "Keypad4Key6", "Keypad4Key7", "Keypad4Key8", "Keypad4Key9", "Keypad4KeyCancel", mLabDoorToUnlock, "8279");
	mColliderKeypad = AddKeypad("Keypad5", "Keypad5Key0", "Keypad5Key1", "Keypad5Key2", "Keypad5Key3", "Keypad5Key4", "Keypad5Key5", "Keypad5Key6", "Keypad5Key7", "Keypad5Key8", "Keypad5Key9", "Keypad5KeyCancel", mColliderDoorToUnlock, "1598");

	ruSound::SetAudioReverb(10);

	mEnemySpawnPosition = GetUniqueObject("EnemyPosition");

	mSteamPS = nullptr;

	DoneInitialization();
}

void LevelResearchFacility::CreateEnemy() {
	// create paths
	Path pathStraight; BuildPath(pathStraight, "PathStraight");
	Path pathCircle; BuildPath(pathCircle, "PathCircle");
	Path pathRoomA; BuildPath(pathRoomA, "PathRoomA");
	Path pathRoomB;	BuildPath(pathRoomB, "PathRoomB");
	Path pathRoomC;	BuildPath(pathRoomC, "PathRoomC");
	Path pathRoomD;	BuildPath(pathRoomD, "PathRoomD");
	Path pathToBasement; BuildPath(pathToBasement, "PathToBasement");

	// add edges
	pathStraight.mVertexList[7]->AddEdge(pathCircle.mVertexList[0]);
	pathStraight.mVertexList[5]->AddEdge(pathToBasement.mVertexList[0]);
	pathStraight.mVertexList[2]->AddEdge(pathRoomA.mVertexList[0]);
	pathStraight.mVertexList[2]->AddEdge(pathRoomB.mVertexList[0]);
	pathStraight.mVertexList[3]->AddEdge(pathRoomC.mVertexList[0]);
	pathStraight.mVertexList[3]->AddEdge(pathRoomD.mVertexList[0]);

	// concatenate paths
	vector<GraphVertex*> allPaths;
	allPaths.insert(allPaths.end(), pathStraight.mVertexList.begin(), pathStraight.mVertexList.end());
	allPaths.insert(allPaths.end(), pathToBasement.mVertexList.begin(), pathToBasement.mVertexList.end());
	allPaths.insert(allPaths.end(), pathCircle.mVertexList.begin(), pathCircle.mVertexList.end());
	allPaths.insert(allPaths.end(), pathRoomA.mVertexList.begin(), pathRoomA.mVertexList.end());
	allPaths.insert(allPaths.end(), pathRoomB.mVertexList.begin(), pathRoomB.mVertexList.end());
	allPaths.insert(allPaths.end(), pathRoomC.mVertexList.begin(), pathRoomC.mVertexList.end());
	allPaths.insert(allPaths.end(), pathRoomD.mVertexList.begin(), pathRoomD.mVertexList.end());

	// create patrol paths
	vector< GraphVertex* > patrolPoints;

	patrolPoints.push_back(pathToBasement.mVertexList.front());
	patrolPoints.push_back(pathToBasement.mVertexList.back());

	patrolPoints.push_back(pathStraight.mVertexList.front());
	patrolPoints.push_back(pathStraight.mVertexList.back());

	patrolPoints.push_back(pathCircle.mVertexList.front());
	patrolPoints.push_back(pathCircle.mVertexList.back());

	patrolPoints.push_back(pathRoomA.mVertexList.front());
	patrolPoints.push_back(pathRoomA.mVertexList.back());

	patrolPoints.push_back(pathRoomB.mVertexList.front());
	patrolPoints.push_back(pathRoomB.mVertexList.back());

	patrolPoints.push_back(pathRoomC.mVertexList.front());
	patrolPoints.push_back(pathRoomC.mVertexList.back());

	patrolPoints.push_back(pathRoomD.mVertexList.front());
	patrolPoints.push_back(pathRoomD.mVertexList.back());

	mEnemy = unique_ptr<Enemy>(new Enemy(allPaths, patrolPoints));
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

	ruEngine::SetAmbientColor(ruVector3(10.0f / 255.0f, 10.0f / 255.0f, 10.0f / 255.0f));

	if (mPowerOn) {
		mpFan1->DoTurn();
		mpFan2->DoTurn();
		mLift1->SetLocked(false);
	} else {
		mLift1->SetLocked(true);
	}

	mLift1->Update();
	mLift2->Update();

	if (mEnemy) {
		mEnemy->Think();
	}

	if (!mStages["DoorUnderFloorOpen"]) {
		if (mPlayer->mNearestPickedNode) {
			if (mPlayer->mNearestPickedNode == mDoorUnderFloor) {
				if (mPlayer->GetInventory()->GetItemSelectedForUse()) {
					if (mPlayer->GetInventory()->GetItemSelectedForUse()->GetType() == Item::Type::Crowbar) {
						mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->GetLocalization()->GetString("openDoor"));
						if (ruInput::IsKeyHit(mPlayer->mKeyUse)) {
							mPlayer->GetInventory()->ResetSelectedForUse();
							mDoorUnderFloor->SetRotation(ruQuaternion(0, 0, -110));
							mStages["DoorUnderFloorOpen"] = true;
						}
					}
				}
			}
		}
	}

	if (mPlayer->IsInsideZone(mRadioHurtZone)) {
		mPlayer->Damage(0.05, false);
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("radioactive"));
	}

	mpSteamValve->Update();
	mpExtemeSteam->Update();
	mpExtemeSteam->SetPower(1.0f - mpSteamValve->GetClosedCoeffecient());

	UpdatePowerupSequence();
	UpdateThermiteSequence();

	if (mPowerOn && mpPowerSparks) {
		mpPowerSparks->Update();

		if (!mpPowerSparks->IsAlive()) {
			mpPowerSparks.reset();
		}
	}

	if (mSteamHissSound->IsPlaying() && mSteamPS) {
		static float steamParticleSize = 0.15f;

		mSteamPS->SetPointSize(steamParticleSize);

		if (steamParticleSize > 0) {
			steamParticleSize -= 0.0005f;
		}
		else {
			mSteamPS.reset();
		}
	}

	if (mSteamDisabled) {
		mExtremeSteamBlock->SetPosition(ruVector3(1000, 1000, 1000));
		mSteamPS.reset();
	} else {
		if (mPlayer->IsInsideZone(mZoneExtremeSteamHurt)) {
			mPlayer->Damage(0.6);
		}
	}

	if (mpSteamValve->IsDone()) {		
		mSteamDisabled = true;
	}

	if (mPlayer->IsInsideZone(mZoneNewLevelLoad)) {
		Level::Change(L4Sewers);
	}
}

void LevelResearchFacility::UpdateThermiteSequence() {
	auto & player = Level::Current()->GetPlayer();
	if (player->GetInventory()->GetItemSelectedForUse()) {
		if (mThermiteItemPlace->IsPickedByPlayer()) {
			if (ruInput::IsKeyHit(player->mKeyUse)) {
				bool placed = mThermiteItemPlace->PlaceItem(player->GetInventory()->GetItemSelectedForUse()->GetType());
				if (placed) {
					if (mThermiteItemPlace->GetPlaceType() == Item::Type::AluminumPowder) {
						mThermiteSmall->Show();
						mThermiteItemPlace->SetPlaceType(Item::Type::FerrumOxide);
					}
					else if (mThermiteItemPlace->GetPlaceType() == Item::Type::FerrumOxide) {
						mThermiteBig->Show();
						mThermiteItemPlace->SetPlaceType(Item::Type::Lighter);
					}
					else if (mThermiteItemPlace->GetPlaceType() == Item::Type::Lighter) {
						mMeshLockAnimation.SetEnabled(true);
						mMeshAnimation.SetEnabled(true);

						mThermiteSmall->Hide();
						mThermiteBig->Hide();

						mBurnSound = ruSound::Load3D("data/sounds/burn.ogg");
						mBurnSound->SetPosition(mThermiteSmall->GetPosition());
						mBurnSound->Play();

						mThermiteItemPlace->SetPlaceType(Item::Type::Unknown);

						mThermitePS = ruParticleSystem::Create(150);
						mThermitePS->SetPosition(mThermiteSmall->GetPosition());
						mThermitePS->SetTexture(ruTexture::Request("data/textures/particles/p1.png"));
						mThermitePS->SetType(ruParticleSystem::Type::Box);
						mThermitePS->SetSpeedDeviation(ruVector3(-0.001, 0.001, -0.001), ruVector3(0.001, 0.009, 0.001));
						mThermitePS->SetColorRange(ruVector3(255, 255, 255), ruVector3(255, 255, 255));
						mThermitePS->SetPointSize(0.045f);
						mThermitePS->SetBoundingBox(ruVector3(-0.2, 0.0, -0.2), ruVector3(0.2, 0.4, 0.2));
						mThermitePS->SetParticleThickness(20.5f);
						mThermitePS->SetAutoResurrection(false);
						mThermitePS->SetLightingEnabled(true);
					}
				}
			}
			player->GetHUD()->SetAction(player->mKeyUse, player->GetLocalization()->GetString("placeReactive"));
		}
	}
}

void LevelResearchFacility::UpdatePowerupSequence() {
	if (fuseInsertedCount < 3) {
		fuseInsertedCount = 0;

		for (int iFuse = 0; iFuse < 3; iFuse++) {
			shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFuse];
			if (pFuse->GetPlaceType() == Item::Type::Unknown) {
				fuseInsertedCount++;
			}
		}
	}

	auto & player = Level::Current()->GetPlayer();

	if (player->GetInventory()->GetItemSelectedForUse()) {
		for (int iFuse = 0; iFuse < 3; iFuse++) {
			shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFuse];
			if (pFuse->IsPickedByPlayer()) {
				player->GetHUD()->SetAction(player->mKeyUse, player->GetLocalization()->GetString("insertFuse"));
			}
		}

		if (ruInput::IsKeyHit(player->mKeyUse)) {
			for (int iFusePlace = 0; iFusePlace < 3; iFusePlace++) {
				shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFusePlace];

				if (pFuse->IsPickedByPlayer()) {
					bool placed = pFuse->PlaceItem(player->GetInventory()->GetItemSelectedForUse()->GetType());

					if (placed) {
						fuseModel[iFusePlace]->Show();
						pFuse->SetPlaceType(Item::Type::Unknown);
					}
				}
			}
		}
	}

	if (fuseInsertedCount >= 3) {
		if (player->mNearestPickedNode == powerLever) {
			player->GetHUD()->SetAction(player->mKeyUse, player->mLocalization.GetString("powerUp"));

			if (ruInput::IsKeyHit(player->mKeyUse) && !mPowerOn) {
				

				mPowerLamp->SetColor(ruVector3(0, 255, 0));

				mLeverSound->Play();

				mpPowerSparks = unique_ptr<Sparks>(new Sparks(mPowerLeverSnd, ruSound::Load3D("data/sounds/sparks.ogg")));

				mPowerLeverOnModel->Show();
				mPowerLeverOffModel->Hide();

				mPowerOn = true;

				player->GetHUD()->SetObjective(mLocalization.GetString("objectiveTryToFindExit"));
			}
		}
	}
}

inline void LevelResearchFacility::Proxy_GiveCrowbar() {
	mPlayer->AddItem(Item::Type::Crowbar);
}

inline void LevelResearchFacility::Proxy_GiveFe2O3() {
	mPlayer->AddItem(Item::Type::FerrumOxide);
}

inline void LevelResearchFacility::Proxy_GiveAl() {
	mPlayer->AddItem(Item::Type::AluminumPowder);
}

inline void LevelResearchFacility::Proxy_GiveFuse() {
	mPlayer->AddItem(Item::Type::Fuse);
}

void LevelResearchFacility::CreatePowerUpSequence() {
	AddInteractiveObject(Item::GetNameByType(Item::Type::Fuse), make_shared<InteractiveObject>(GetUniqueObject("Fuse1")), ruDelegate::Bind(this, &LevelResearchFacility::Proxy_GiveFuse));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Fuse), make_shared<InteractiveObject>(GetUniqueObject("Fuse2")), ruDelegate::Bind(this, &LevelResearchFacility::Proxy_GiveFuse));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Fuse), make_shared<InteractiveObject>(GetUniqueObject("Fuse3")), ruDelegate::Bind(this, &LevelResearchFacility::Proxy_GiveFuse));

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

	if (enemyPresented) {
		if (s.IsLoading()) {
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
	if (!mStages["EnterObjectiveNeedOpenDoorOntoFloor"]) {
		auto & player = Level::Current()->GetPlayer();
		player->GetHUD()->SetObjective(mLocalization.GetString("objectiveNeedOpenDoorOntoFloor"));
		mStages["EnterObjectiveNeedOpenDoorOntoFloor"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterNeedCrowbarZone()
{
	if (!mStages["EnterObjectiveNeedCrowbar"]) {
		auto & player = Level::Current()->GetPlayer();
		player->GetHUD()->SetObjective(mLocalization.GetString("objectiveNeedCrowbar"));
		mStages["EnterObjectiveNeedCrowbar"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterRemovePathBlockingMeshZone()
{
	if (!mStages["PassedThroughBlockingMesh"]) {
		mLockedDoor->SetLocked(false);
		mStages["PassedThroughBlockingMesh"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterExaminePlaceZone()
{
	if (!mStages["EnterObjectiveExaminePlace"]) {
		auto & player = Level::Current()->GetPlayer();
		player->GetHUD()->SetObjective(mLocalization.GetString("objectiveExaminePlace"));
		mStages["EnterObjectiveExaminePlace"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterRestorePowerZone()
{
	if (!mStages["EnterObjectiveRestorePowerZone"]) {
		auto & player = Level::Current()->GetPlayer();
		player->GetHUD()->SetObjective(mLocalization.GetString("objectiveRestorePower"));
		mStages["EnterObjectiveRestorePowerZone"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterSteamActivateZone() {
	if (!mStages["EnterSteamActivateZone"]) {
		mSteamPS = ruParticleSystem::Create(35);
		mSteamPS->SetPosition(mSmallSteamPosition->GetPosition());
		mSteamPS->SetTexture(ruTexture::Request("data/textures/particles/p1.png"));
		mSteamPS->SetType(ruParticleSystem::Type::Stream);
		mSteamPS->SetSpeedDeviation(ruVector3(-0.0015, 0.08, -0.0015), ruVector3(0.0015, 0.2, 0.0015));
		mSteamPS->SetBoundingRadius(0.4f);
		mSteamPS->SetColorRange(ruVector3(255, 255, 255), ruVector3(255, 255, 255));
		mSteamPS->SetPointSize(0.15f);
		mSteamPS->SetParticleThickness(1.5f);
		mSteamPS->SetLightingEnabled(true);

		mSteamHissSound->Attach(mSteamPS);
		mSteamHissSound->Play();
		mStages["EnterSteamActivateZone"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterSpawnEnemyZone()
{
	if (!mStages["EnemySpawned"]) {
		CreateEnemy();
		mStages["EnemySpawned"] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterNeedPassThroughMeshZone()
{
	if (!mStages["NeedPassThroughMesh"]) {
		auto & player = Level::Current()->GetPlayer();
		player->GetHUD()->SetObjective(mLocalization.GetString("objectiveNeedPassThroughMesh"));
		mStages["NeedPassThroughMesh"] = true;
	}
}
