#include "Precompiled.h"

#include "LevelMine.h"
#include "Player.h"
#include "Pathfinder.h"
#include "Utils.h"

LevelMine::LevelMine(const unique_ptr<PlayerTransfer> & playerTransfer) : Level(playerTransfer) {
	mTypeNum = 3;

	LoadLocalization("mine.loc");

	LoadSceneFromFile("data/maps/mine.scene");

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());

	ruVector3 placePos = GetUniqueObject("PlayerPosition")->GetPosition();
	ruVector3 playerPos = mPlayer->GetCurrentPosition();

	mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective1"));

	AddSheet("Note1", mLocalization.GetString("note1Desc"), mLocalization.GetString("note1"));
	AddSheet("Note2", mLocalization.GetString("note2Desc"), mLocalization.GetString("note2"));
	AddSheet("Note3", mLocalization.GetString("note3Desc"), mLocalization.GetString("note3"));
	AddSheet("Note4", mLocalization.GetString("note4Desc"), mLocalization.GetString("note4"));
	AddSheet("Note5", mLocalization.GetString("note5Desc"), mLocalization.GetString("note5"));
	AddSheet("Note6", mLocalization.GetString("note6Desc"), mLocalization.GetString("note6"));
	AddSheet("Note7", mLocalization.GetString("note7Desc"), mLocalization.GetString("note7"));
	AddSheet("Note8", mLocalization.GetString("note8Desc"), mLocalization.GetString("note8"));

	mStoneFallZone = GetUniqueObject("StoneFallZone");

	mLiftButton = GetUniqueObject("Lift1Screen");

	mNewLevelZone = GetUniqueObject("NewLevel");

	ruSound::SetAudioReverb(10);

	AddSound(mMusic = ruSound::LoadMusic("data/music/chapter2.ogg"));

	mConcreteWall = GetUniqueObject("ConcreteWall");
	mDeathZone = GetUniqueObject("DeadZone");
	mDetonator = GetUniqueObject("Detonator");

	AddSound(mAlertSound = ruSound::Load3D("data/sounds/alert.ogg"));
	mAlertSound->Attach(mDetonator);

	AddSound(mExplosionSound = ruSound::Load3D("data/sounds/blast.ogg"));
	mExplosionSound->SetReferenceDistance(10);

	mDetonatorActivated = 0;

	mExplosionFlashAnimator = 0;

	AddInteractiveObject(Item::GetNameByType(Item::Type::Pistol), make_shared<InteractiveObject>(GetUniqueObject("Pistol")), ruDelegate::Bind(this, &LevelMine::Proxy_GivePistol));

	AutoCreateBulletsByNamePattern("Bullet?([[:digit:]]+)");

	// Create detonator places
	AddItemPlace(mDetonatorPlace[0] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace1"), Item::Type::Explosives));
	AddItemPlace(mDetonatorPlace[1] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace2"), Item::Type::Explosives));
	AddItemPlace(mDetonatorPlace[2] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace3"), Item::Type::Explosives));
	AddItemPlace(mDetonatorPlace[3] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace4"), Item::Type::Explosives));

	mWireModels[0] = GetUniqueObject("WireModel1");
	mWireModels[1] = GetUniqueObject("WireModel2");
	mWireModels[2] = GetUniqueObject("WireModel3");
	mWireModels[3] = GetUniqueObject("WireModel4");

	mDetonatorModels[0] = GetUniqueObject("DetonatorModel1");
	mDetonatorModels[1] = GetUniqueObject("DetonatorModel2");
	mDetonatorModels[2] = GetUniqueObject("DetonatorModel3");
	mDetonatorModels[3] = GetUniqueObject("DetonatorModel4");

	mExplosivesModels[0] = GetUniqueObject("ExplosivesModel1");
	mExplosivesModels[1] = GetUniqueObject("ExplosivesModel2");
	mExplosivesModels[2] = GetUniqueObject("ExplosivesModel3");
	mExplosivesModels[3] = GetUniqueObject("ExplosivesModel4");

	mFindItemsZone = GetUniqueObject("FindItemsZone");

	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/mine/ambientmine1.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/mine/ambientmine2.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/mine/ambientmine3.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/mine/ambientmine4.ogg"));
	AddAmbientSound(ruSound::Load3D("data/sounds/ambient/mine/ambientmine5.ogg"));

	mExplosionTimer = ruTimer::Create();
	mBeepSoundTimer = ruTimer::Create();
	mBeepSoundTiming = 1.0f;

	CreateItems();

	mReadyExplosivesCount = 0;

	AddLadder("LadderBegin", "LadderEnd", "LadderEnter", "LadderBeginLeavePoint", "LadderEndLeavePoint");
	AddDoor("Door1", 90);
	AddDoor("Door3", 90);
	AddDoor("DoorToAdministration", 90);
	AddDoor("Door6", 90);
	AddDoor("DoorToDirectorsOffice", 90);
	AddDoor("DoorToResearchFacility", 90);	

	AddKeypad("Keypad1", "Keypad1Key0", "Keypad1Key1", "Keypad1Key2", "Keypad1Key3", "Keypad1Key4", "Keypad1Key5", "Keypad1Key6", "Keypad1Key7", "Keypad1Key8", "Keypad1Key9", "Keypad1KeyCancel", AddDoor("StorageDoor", 90), "7854");
	AddKeypad("Keypad2", "Keypad2Key0", "Keypad2Key1", "Keypad2Key2", "Keypad2Key3", "Keypad2Key4", "Keypad2Key5", "Keypad2Key6", "Keypad2Key7", "Keypad2Key8", "Keypad2Key9", "Keypad2KeyCancel", AddDoor("DoorToResearchFacility", 90), "1689");
	AddKeypad("Keypad3", "Keypad3Key0", "Keypad3Key1", "Keypad3Key2", "Keypad3Key3", "Keypad3Key4", "Keypad3Key5", "Keypad3Key6", "Keypad3Key7", "Keypad3Key8", "Keypad3Key9", "Keypad3KeyCancel", AddDoor("DoorMedical", 90), "9632");		

	mMusic->Play();

	mStages["EnterRockFallZoneWallExp"] = false;
	mStages["EnterScreamerDone"] = false;
	mStages["EnterScreamer2Done"] = false;
	mStages["ConcreteWallExp"] = false;
	mStages["FindObjectObjectiveSet"] = false;
	mStages["FoundObjectsForExplosion"] = false;

	// create paths
	Path path;
	BuildPath(path, "Path");
	Path pathOnUpperLevel;
	BuildPath(pathOnUpperLevel, "PathOnUpperLevel");
	Path pathUpperRight;
	BuildPath(pathUpperRight, "PathUpperRight");
	Path pathUpperLeft;
	BuildPath(pathUpperLeft, "PathUpperLeft");
	Path pathToRoom;
	BuildPath(pathToRoom, "PathToRoom");
	Path pathUpperRightTwo;
	BuildPath(pathUpperRightTwo, "PathUpperRightTwo");
	Path pathLeft;
	BuildPath(pathLeft, "PathLeft");

	// cross-path edges
	path.mVertexList[17]->AddEdge(pathOnUpperLevel.mVertexList[0]);
	pathOnUpperLevel.mVertexList[6]->AddEdge(pathUpperRight.mVertexList[0]);
	pathOnUpperLevel.mVertexList[8]->AddEdge(pathUpperLeft.mVertexList[0]);
	path.mVertexList[18]->AddEdge(pathToRoom.mVertexList[0]);
	pathOnUpperLevel.mVertexList[12]->AddEdge(pathUpperRightTwo.mVertexList[0]);
	path.mVertexList[13]->AddEdge(pathLeft.mVertexList[0]);

	// concatenate all paths
	vector<GraphVertex*> allPaths;
	allPaths.insert(allPaths.end(), path.mVertexList.begin(), path.mVertexList.end());
	allPaths.insert(allPaths.end(), pathOnUpperLevel.mVertexList.begin(), pathOnUpperLevel.mVertexList.end());
	allPaths.insert(allPaths.end(), pathUpperRight.mVertexList.begin(), pathUpperRight.mVertexList.end());
	allPaths.insert(allPaths.end(), pathUpperLeft.mVertexList.begin(), pathUpperLeft.mVertexList.end());
	allPaths.insert(allPaths.end(), pathToRoom.mVertexList.begin(), pathToRoom.mVertexList.end());
	allPaths.insert(allPaths.end(), pathUpperRightTwo.mVertexList.begin(), pathUpperRightTwo.mVertexList.end());
	allPaths.insert(allPaths.end(), pathLeft.mVertexList.begin(), pathLeft.mVertexList.end());

	vector< GraphVertex* > patrolPoints;
	patrolPoints.push_back(path.mVertexList.front());
	patrolPoints.push_back(path.mVertexList.back());

	patrolPoints.push_back(pathOnUpperLevel.mVertexList.front());
	patrolPoints.push_back(pathOnUpperLevel.mVertexList.back());

	patrolPoints.push_back(pathUpperRight.mVertexList.front());
	patrolPoints.push_back(pathUpperRight.mVertexList.back());

	patrolPoints.push_back(pathUpperLeft.mVertexList.front());
	patrolPoints.push_back(pathUpperLeft.mVertexList.back());

	patrolPoints.push_back(pathUpperRightTwo.mVertexList.front());
	patrolPoints.push_back(pathUpperRightTwo.mVertexList.back());

	patrolPoints.push_back(pathToRoom.mVertexList.front());
	patrolPoints.push_back(pathToRoom.mVertexList.back());

	patrolPoints.push_back(pathLeft.mVertexList.front());
	patrolPoints.push_back(pathLeft.mVertexList.back());

	mEnemy = unique_ptr<Enemy>(new Enemy(allPaths, patrolPoints));
	mEnemy->SetPosition(GetUniqueObject("EnemyPosition")->GetPosition());
	
	mExplosivesDummy[0] = GetUniqueObject("ExplosivesModel5");
	mExplosivesDummy[1] = GetUniqueObject("ExplosivesModel6");
	mExplosivesDummy[2] = GetUniqueObject("ExplosivesModel7");
	mExplosivesDummy[3] = GetUniqueObject("ExplosivesModel8");

	mExplodedWall = GetUniqueObject("ConcreteWallExploded");
	mExplodedWall->Hide();

	mExplosionFlashPosition = GetUniqueObject("ExplosionFlash");

	DoneInitialization();
}

LevelMine::~LevelMine() {

}

void LevelMine::Show() {
	Level::Show();
}

void LevelMine::Hide() {
	Level::Hide();

	mMusic->Pause();
}

void LevelMine::DoScenario() {
	mMusic->Play();

	mEnemy->Think();

	ruEngine::SetAmbientColor(ruVector3(0.01, 0.01, 0.01));

	PlayAmbientSounds();

	if (!mStages["EnterRockFallZoneWallExp"]) {
		if (mPlayer->IsInsideZone(mStoneFallZone)) {
			ruSceneNode::FindByName("StoneFall")->Unfreeze();

			mStages["EnterRockFallZoneWallExp"] = true;
		}
	}

	if (mPlayer->mNearestPickedNode == mLiftButton) {
		mPlayer->GetHUD()->SetAction(ruInput::Key::None, mLocalization.GetString("brokenLift"));
	}

	if (!mStages["FindObjectObjectiveSet"]) {
		if (!mStages["FoundObjectsForExplosion"]) {
			if (mPlayer->IsInsideZone(mFindItemsZone)) {
				mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective2"));

				mStages["FindObjectObjectiveSet"] = true;
			}
		}
		else {
			mStages["FindObjectObjectiveSet"] = true;
		}
	}

	if (mExplosionFlashAnimator) {
		mExplosionFlashAnimator->Update();
	}

	if (!mStages["ConcreteWallExp"]) {
		if (mPlayer->mNearestPickedNode == mDetonator) {
			mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mLocalization.GetString("detonator"));

			if (ruInput::IsKeyHit(mPlayer->mKeyUse) && mReadyExplosivesCount >= 4 && !mDetonatorActivated) {
				mDetonatorActivated = true;

				mExplosionTimer->Restart();;
			}
		}

		if (mDetonatorActivated) {
			if (mExplosionTimer->GetElapsedTimeInSeconds() >= 10.0f) {
				mDetonatorActivated = false;

				mExplosionSound->Play();

				mExplosionSound->SetPosition(mConcreteWall->GetPosition());

				mStages["ConcreteWallExp"] = true;

				mConcreteWall->SetPosition(ruVector3(10000, 10000, 10000));

				CleanUpExplodeArea();

				ruVector3 vec = (mConcreteWall->GetPosition() - mPlayer->GetCurrentPosition()).Normalize() * 20;

				mExplodedWall->Show();

				mExplosionFlashLight = ruPointLight::Create();
				mExplosionFlashLight->Attach(mExplosionFlashPosition);
				mExplosionFlashLight->SetColor(ruVector3(255, 200, 160));
				mExplosionFlashAnimator = unique_ptr<LightAnimator>(new LightAnimator(mExplosionFlashLight, 0.25, 30, 1.1));
				mExplosionFlashAnimator->SetAnimationType(LightAnimator::AnimationType::Off);

				// dust
				mExplosionDustParticleSystem = ruParticleSystem::Create(400);
				mExplosionDustParticleSystem->SetPosition(mExplosivesDummy[0]->GetPosition() - ruVector3(0, 2.5, 0));
				mExplosionDustParticleSystem->SetTexture(ruTexture::Request("data/textures/particles/p1.png"));
				mExplosionDustParticleSystem->SetType(ruParticleSystem::Type::Box);
				mExplosionDustParticleSystem->SetLightingEnabled(false);
				mExplosionDustParticleSystem->SetAutoResurrection(false);
				mExplosionDustParticleSystem->SetPointSize(0.8f);
				mExplosionDustParticleSystem->SetSpeedDeviation(ruVector3(-0.0008, 0.0, -0.0008), ruVector3(0.0008, 0.005, 0.0008));
				mExplosionDustParticleSystem->SetBoundingBox(ruVector3(-1, 0, -3), ruVector3(1, 3, 3));
				mExplosionDustParticleSystem->SetColorRange(ruVector3(130, 130, 130), ruVector3(150, 150, 150));

				if (mPlayer->IsInsideZone(mDeathZone)) {
					mPlayer->Damage(1000);
				}
			}

			if (mBeepSoundTimer->GetElapsedTimeInSeconds() > mBeepSoundTiming) { // every 1 sec
				mBeepSoundTiming -= 0.05f;
				mBeepSoundTimer->Restart();
				mAlertSound->Play(false);
			}
		}
	}

	if (mPlayer->IsInsideZone(mNewLevelZone)) {
		Level::Change(LevelName::L3ResearchFacility);

		return;
	}

	UpdateExplodeSequence();
}

inline void LevelMine::Proxy_GiveExplosives() {
	mPlayer->AddItem(Item::Type::Explosives);
}

inline void LevelMine::Proxy_GiveDetonator() {
	mPlayer->AddItem(Item::Type::Detonator);
}

inline void LevelMine::Proxy_GiveWires() {
	mPlayer->AddItem(Item::Type::Wires);
}

inline void LevelMine::Proxy_GivePistol() {
	mPlayer->AddUsableObject(new Weapon);
}

inline void LevelMine::Proxy_GiveFuel() {
	mPlayer->AddItem(Item::Type::FuelCanister);
}

inline void LevelMine::Proxy_GiveSyringe() {
	mPlayer->AddUsableObject(new Syringe);
}

void LevelMine::UpdateExplodeSequence() {
	if (mReadyExplosivesCount < 4) {
		mReadyExplosivesCount = 0;

		for (int i = 0; i < 4; i++) {
			shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

			if (dp->GetPlaceType() == Item::Type::Unknown) {
				mReadyExplosivesCount++;
			}

			if (mReadyExplosivesCount >= 4) {
				mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective4"));
			}
		}
	}

	static int totalNeededObjects = 0;
	if (totalNeededObjects < 12) {
		totalNeededObjects = 0;
		totalNeededObjects += mPlayer->GetInventory()->GetItemCount(Item::Type::Wires);
		totalNeededObjects += mPlayer->GetInventory()->GetItemCount(Item::Type::Explosives);
		totalNeededObjects += mPlayer->GetInventory()->GetItemCount(Item::Type::Detonator);
		if (totalNeededObjects >= 12) {
			mStages["FindObjectObjectiveSet"] = true;
			mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective3"));
		}
	}

	if (mPlayer->GetInventory()->GetItemSelectedForUse()) {
		for (int i = 0; i < 4; i++) {
			shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

			if (dp->IsPickedByPlayer()) {
				mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->mLocalization.GetString("putItem"));
			}
		}

		if (ruInput::IsKeyHit(mPlayer->mKeyUse)) {
			for (int i = 0; i < 4; i++) {
				shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

				if (dp->IsPickedByPlayer()) {
					bool placed = dp->PlaceItem(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType());

					if (placed) {
						// 1st: Explosives
						// 2nd: Detonator
						// 3rd: Wires
						// 4th: Ready to explode
						if (dp->GetPlaceType() == Item::Type::Explosives) {
							mExplosivesModels[i]->Show();
							dp->SetPlaceType(Item::Type::Detonator);
						} else if (dp->GetPlaceType() == Item::Type::Detonator) {
							mDetonatorModels[i]->Show();
							dp->SetPlaceType(Item::Type::Wires);
						} else if (dp->GetPlaceType() == Item::Type::Wires) {
							mWireModels[i]->Show();
							dp->SetPlaceType(Item::Type::Unknown);
						}
					}
				}
			}
		}
	}
}

void LevelMine::CleanUpExplodeArea() {
	for (int i = 0; i < 4; i++) {
		mDetonatorPlace[i]->mObject->SetPosition(ruVector3(1000, 1000, 1000));
		mWireModels[i]->Hide();
		mExplosivesModels[i]->Hide();
		mDetonatorModels[i]->Hide();
		mExplosivesDummy[i]->Hide();
	}
}

void LevelMine::CreateItems() {
	// Create explosives
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives1")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveExplosives));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives2")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveExplosives));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives3")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveExplosives));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives4")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveExplosives));

	// Create detonators
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator1")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveDetonator));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator2")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveDetonator));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator3")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveDetonator));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator4")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveDetonator));

	// Create wires
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire1")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveWires));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire2")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveWires));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire3")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveWires));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire4")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveWires));

	AddInteractiveObject(Item::GetNameByType(Item::Type::FuelCanister), make_shared<InteractiveObject>(GetUniqueObject("Fuel1")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveFuel));
	AddInteractiveObject(Item::GetNameByType(Item::Type::FuelCanister), make_shared<InteractiveObject>(GetUniqueObject("Fuel2")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveFuel));
	AddInteractiveObject(Item::GetNameByType(Item::Type::FuelCanister), make_shared<InteractiveObject>(GetUniqueObject("Fuel3")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveFuel));

	AddInteractiveObject(Item::GetNameByType(Item::Type::Syringe), make_shared<InteractiveObject>(GetUniqueObject("Syringe")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveSyringe));
	AddInteractiveObject(Item::GetNameByType(Item::Type::Syringe), make_shared<InteractiveObject>(GetUniqueObject("Syringe2")), ruDelegate::Bind(this, &LevelMine::Proxy_GiveSyringe));
}

void LevelMine::OnSerialize(SaveFile & s) {
	s & mDetonatorActivated;
	s & mBeepSoundTiming;
	mEnemy->Serialize(s);
}
